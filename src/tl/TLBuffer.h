#ifndef TELEGRAM_TL_BUFFER_H
#define TELEGRAM_TL_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <QByteArray>
#include <QString>

namespace Telegram {
namespace TL {

class TLBuffer {
public:
    TLBuffer();
    TLBuffer(const uint8_t* data, size_t size);
    TLBuffer(const QByteArray& data);

    const uint8_t* data() const;
    size_t size() const;
    size_t offset() const;
    void setOffset(size_t off);
    size_t remaining() const;
    void clear();

    const QByteArray& buffer() const;

    // Serialization (Writing)
    void writeInt32(int32_t val);
    void writeUInt32(uint32_t val);
    void writeInt64(int64_t val);
    void writeUInt64(uint64_t val);
    void writeInt128(const uint8_t* val);
    void writeInt256(const uint8_t* val);
    void writeBytes(const uint8_t* bytes, size_t len);
    void writeBytes(const QByteArray& bytes);
    void writeString(const QString& str);
    void writeRaw(const uint8_t* data, size_t len);

    // Deserialization (Reading)
    bool readInt32(int32_t& val);
    bool readUInt32(uint32_t& val);
    bool readInt64(int64_t& val);
    bool readUInt64(uint64_t& val);
    bool readInt128(uint8_t* out);
    bool readInt256(uint8_t* out);
    bool readBytes(QByteArray& out);
    bool readString(QString& out);
    bool readRaw(uint8_t* out, size_t len);

private:
    QByteArray m_buffer;
    size_t m_offset;
};

} // namespace TL
} // namespace Telegram

#endif // TELEGRAM_TL_BUFFER_H
