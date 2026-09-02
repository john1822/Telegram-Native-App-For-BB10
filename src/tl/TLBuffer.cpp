#include "TLBuffer.h"
#include <string.h>

namespace Telegram {
namespace TL {

TLBuffer::TLBuffer() : m_offset(0) {}

TLBuffer::TLBuffer(const uint8_t* data, size_t size) : m_offset(0) {
    if (size > 0 && data) {
        m_buffer.append(reinterpret_cast<const char*>(data), static_cast<int>(size));
    }
}

TLBuffer::TLBuffer(const QByteArray& data) : m_buffer(data), m_offset(0) {}

const uint8_t* TLBuffer::data() const {
    return reinterpret_cast<const uint8_t*>(m_buffer.constData());
}

size_t TLBuffer::size() const {
    return static_cast<size_t>(m_buffer.size());
}

size_t TLBuffer::offset() const {
    return m_offset;
}

void TLBuffer::setOffset(size_t off) {
    m_offset = (off <= static_cast<size_t>(m_buffer.size())) ? off : static_cast<size_t>(m_buffer.size());
}

size_t TLBuffer::remaining() const {
    return (m_offset < static_cast<size_t>(m_buffer.size())) ? (static_cast<size_t>(m_buffer.size()) - m_offset) : 0;
}

void TLBuffer::clear() {
    m_buffer.clear();
    m_offset = 0;
}

const QByteArray& TLBuffer::buffer() const {
    return m_buffer;
}

void TLBuffer::writeInt32(int32_t val) {
    writeUInt32(static_cast<uint32_t>(val));
}

void TLBuffer::writeUInt32(uint32_t val) {
    uint8_t b[4];
    b[0] = static_cast<uint8_t>(val & 0xFF);
    b[1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    b[2] = static_cast<uint8_t>((val >> 16) & 0xFF);
    b[3] = static_cast<uint8_t>((val >> 24) & 0xFF);
    writeRaw(b, 4);
}

void TLBuffer::writeInt64(int64_t val) {
    writeUInt64(static_cast<uint64_t>(val));
}

void TLBuffer::writeUInt64(uint64_t val) {
    uint8_t b[8];
    for (int i = 0; i < 8; ++i) {
        b[i] = static_cast<uint8_t>((val >> (i * 8)) & 0xFF);
    }
    writeRaw(b, 8);
}

void TLBuffer::writeInt128(const uint8_t* val) {
    writeRaw(val, 16);
}

void TLBuffer::writeInt256(const uint8_t* val) {
    writeRaw(val, 32);
}

void TLBuffer::writeBytes(const uint8_t* bytes, size_t len) {
    if (len < 254) {
        uint8_t l = static_cast<uint8_t>(len);
        writeRaw(&l, 1);
        if (len > 0 && bytes) {
            writeRaw(bytes, len);
        }
        size_t pad = (4 - ((1 + len) % 4)) % 4;
        if (pad > 0) {
            uint8_t zeros[4] = {0, 0, 0, 0};
            writeRaw(zeros, pad);
        }
    } else {
        uint8_t prefix[4];
        prefix[0] = 0xFE;
        prefix[1] = static_cast<uint8_t>(len & 0xFF);
        prefix[2] = static_cast<uint8_t>((len >> 8) & 0xFF);
        prefix[3] = static_cast<uint8_t>((len >> 16) & 0xFF);
        writeRaw(prefix, 4);
        if (len > 0 && bytes) {
            writeRaw(bytes, len);
        }
        size_t pad = (4 - (len % 4)) % 4;
        if (pad > 0) {
            uint8_t zeros[4] = {0, 0, 0, 0};
            writeRaw(zeros, pad);
        }
    }
}

void TLBuffer::writeBytes(const QByteArray& bytes) {
    writeBytes(reinterpret_cast<const uint8_t*>(bytes.constData()), bytes.size());
}

void TLBuffer::writeString(const QString& str) {
    QByteArray utf8 = str.toUtf8();
    writeBytes(reinterpret_cast<const uint8_t*>(utf8.constData()), utf8.size());
}

void TLBuffer::writeRaw(const uint8_t* data, size_t len) {
    if (len > 0 && data) {
        m_buffer.append(reinterpret_cast<const char*>(data), static_cast<int>(len));
    }
}

bool TLBuffer::readInt32(int32_t& val) {
    uint32_t u;
    if (!readUInt32(u)) return false;
    val = static_cast<int32_t>(u);
    return true;
}

bool TLBuffer::readUInt32(uint32_t& val) {
    if (remaining() < 4) return false;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(m_buffer.constData()) + m_offset;
    val = static_cast<uint32_t>(p[0]) |
          (static_cast<uint32_t>(p[1]) << 8) |
          (static_cast<uint32_t>(p[2]) << 16) |
          (static_cast<uint32_t>(p[3]) << 24);
    m_offset += 4;
    return true;
}

bool TLBuffer::readInt64(int64_t& val) {
    uint64_t u;
    if (!readUInt64(u)) return false;
    val = static_cast<int64_t>(u);
    return true;
}

bool TLBuffer::readUInt64(uint64_t& val) {
    if (remaining() < 8) return false;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(m_buffer.constData()) + m_offset;
    val = 0;
    for (int i = 0; i < 8; ++i) {
        val |= (static_cast<uint64_t>(p[i]) << (i * 8));
    }
    m_offset += 8;
    return true;
}

bool TLBuffer::readInt128(uint8_t* out) {
    return readRaw(out, 16);
}

bool TLBuffer::readInt256(uint8_t* out) {
    return readRaw(out, 32);
}

bool TLBuffer::readBytes(QByteArray& out) {
    if (remaining() < 1) return false;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(m_buffer.constData()) + m_offset;
    uint8_t first = p[0];
    m_offset++;
    
    size_t len = 0;
    size_t pad = 0;
    if (first < 254) {
        len = first;
        pad = (4 - ((1 + len) % 4)) % 4;
    } else {
        if (remaining() < 3) return false;
        p = reinterpret_cast<const uint8_t*>(m_buffer.constData()) + m_offset;
        len = static_cast<size_t>(p[0]) |
              (static_cast<size_t>(p[1]) << 8) |
              (static_cast<size_t>(p[2]) << 16);
        m_offset += 3;
        pad = (4 - (len % 4)) % 4;
    }

    if (remaining() < (len + pad)) return false;
    out.clear();
    out.append(m_buffer.constData() + m_offset, static_cast<int>(len));
    m_offset += len + pad;
    return true;
}

bool TLBuffer::readString(QString& out) {
    QByteArray bytes;
    if (!readBytes(bytes)) return false;
    out = QString::fromUtf8(bytes.constData(), bytes.size());
    return true;
}

bool TLBuffer::readRaw(uint8_t* out, size_t len) {
    if (remaining() < len) return false;
    if (len > 0 && out) {
        memcpy(out, m_buffer.constData() + m_offset, len);
    }
    m_offset += len;
    return true;
}

} // namespace TL
} // namespace Telegram
