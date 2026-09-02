#ifndef TELEGRAM_TL_TYPES_H
#define TELEGRAM_TL_TYPES_H

#include <stdint.h>
#include <stddef.h>

namespace Telegram {
namespace TL {

// Constructor IDs for MTProto Handshake and Core RPC
enum ConstructorId {
    ID_REQ_PQ_MULTI           = 0xbe7e8ef1,
    ID_RESPQ                  = 0x05162463,
    ID_P_Q_INNER_DATA         = 0x83c95aec,
    ID_P_Q_INNER_DATA_DC      = 0xa9f55f95,
    ID_P_Q_INNER_DATA_TEMP_DC = 0x56fddf88,
    ID_REQ_DH_PARAMS          = 0xd712e4be,
    ID_SERVER_DH_PARAMS_OK    = 0xd0e8075c,
    ID_SERVER_DH_PARAMS_FAIL  = 0x79cb045d,
    ID_SERVER_DH_INNER_DATA   = 0xb5890dba,
    ID_CLIENT_DH_INNER_DATA   = 0x6643b654,
    ID_SET_CLIENT_DH_PARAMS   = 0xf5045f1f,
    ID_DH_GEN_OK              = 0x3bcbf734,
    ID_DH_GEN_RETRY           = 0x46dc1fb9,
    ID_DH_GEN_FAIL            = 0xa69dae02,

    ID_RPC_RESULT             = 0xf35c6d01,
    ID_RPC_ERROR              = 0x2144ca19,
    ID_GZIP_PACKED            = 0x3072cfa1,
    ID_MSG_CONTAINER          = 0x73f1f8dc,
    ID_PONG                   = 0x347773c5,
    ID_PING                   = 0x7abe77ec,
    ID_PING_DELAY_DISCONNECT  = 0xf342745b,
    ID_NEW_SESSION_CREATED    = 0x9ec20908,
    ID_BAD_SERVER_SALT        = 0xedab447b,
    ID_BAD_MSG_NOTIFICATION   = 0xa7eff811,
    ID_MSGS_ACK               = 0x62d6b459,

    ID_VECTOR                 = 0x1cb5c415,

    // Core Help Methods for Diagnostic Verification
    ID_HELP_GET_NEAREST_DC    = 0x1fb33026,
    ID_NEAREST_DC             = 0x8e1a1775,
    ID_HELP_GET_CONFIG        = 0xc4f918e0,
    ID_CONFIG                 = 0x330b4067
};

} // namespace TL
} // namespace Telegram

#endif // TELEGRAM_TL_TYPES_H
