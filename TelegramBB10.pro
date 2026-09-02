TEMPLATE = app
TARGET = TelegramBB10

CONFIG += cascades

INCLUDEPATH += src \
               src/crypto \
               src/tl \
               src/network \
               src/core \
               src/storage \
               src/controllers

device {
    QMAKE_CXXFLAGS += -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2
    QMAKE_CFLAGS += -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2
}
simulator {
    QMAKE_CXXFLAGS += -O2
    QMAKE_CFLAGS += -O2
}

LIBS += -lbbcascades \
        -lQtDeclarative \
        -lQtCore \
        -lQtGui \
        -lQtNetwork \
        -lQtSql \
        -lbps \
        -lsocket \
        -lcrypto \
        -lssl \
        -lz \
        -lsqlite3

HEADERS += \
    src/Config.h \
    src/crypto/CryptoEngine.h \
    src/crypto/QrGenerator.h \
    src/tl/TLBuffer.h \
    src/tl/TLTypes.h \
    src/network/TcpTransport.h \
    src/core/MTProtoSession.h \
    src/storage/SessionStorage.h \
    src/controllers/DiagnosticController.h \
    src/controllers/AuthController.h

SOURCES += \
    src/main.cpp \
    src/Config.cpp \
    src/crypto/CryptoEngine.cpp \
    src/crypto/QrGenerator.cpp \
    src/tl/TLBuffer.cpp \
    src/network/TcpTransport.cpp \
    src/core/MTProtoSession.cpp \
    src/storage/SessionStorage.cpp \
    src/controllers/DiagnosticController.cpp \
    src/controllers/AuthController.cpp

OTHER_FILES += \
    bar-descriptor.xml \
    assets/main.qml
