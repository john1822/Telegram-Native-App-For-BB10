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
    ID_PING_DELAY_DISCONNECT  = 0xf3427b8c,
    ID_NEW_SESSION_CREATED    = 0x9ec20908,
    ID_BAD_SERVER_SALT        = 0xedab447b,
    ID_BAD_MSG_NOTIFICATION   = 0xa7eff811,
    ID_MSGS_ACK               = 0x62d6b459,

    ID_VECTOR                 = 0x1cb5c415,

    // Core Help Methods for Diagnostic Verification
    ID_HELP_GET_NEAREST_DC    = 0x1fb33026,
    ID_NEAREST_DC             = 0x8e1a1775,
    ID_HELP_GET_CONFIG        = 0xc4f918e0,
    ID_CONFIG                 = 0x330b4067,

    // Phase 2: Authentication RPC Methods and Types
    ID_AUTH_SEND_CODE         = 0xa677244f,
    ID_AUTH_SENT_CODE         = 0x5e002502,
    ID_CODE_SETTINGS          = 0xad253d78,
    ID_AUTH_RESEND_CODE       = 0xcae47523,

    ID_AUTH_SENT_CODE_TYPE_APP         = 0x3dbb5986,
    ID_AUTH_SENT_CODE_TYPE_SMS         = 0xc000b4c0,
    ID_AUTH_SENT_CODE_TYPE_CALL        = 0x5353e5a7,
    ID_AUTH_SENT_CODE_TYPE_FLASH_CALL  = 0xab03c6d9,
    ID_AUTH_SENT_CODE_TYPE_MISSED_CALL = 0x820064e8,
    ID_AUTH_SENT_CODE_TYPE_EMAIL_CODE  = 0xf450f59b,
    ID_AUTH_SENT_CODE_TYPE_FIREBASE    = 0xe57b1432,

    ID_AUTH_SIGN_IN           = 0x8d52a951,
    ID_AUTH_LOG_OUT           = 0x3e72ba19,
    ID_AUTH_AUTHORIZATION     = 0x2ea2c0d4,
    ID_AUTH_AUTHORIZATION_CD  = 0xcd0509a6,
    ID_AUTH_SIGN_UP_REQUIRED  = 0x44747e9a,

    // QR Code Login
    ID_AUTH_EXPORT_LOGIN_TOKEN = 0xb7e085fe,
    ID_AUTH_LOGIN_TOKEN        = 0x629f1980,
    ID_AUTH_LOGIN_TOKEN_MIGRATE_TO = 0x068e42d9,
    ID_AUTH_LOGIN_TOKEN_SUCCESS    = 0x390d5c5e,

    // 2FA Cloud Password
    ID_ACCOUNT_GET_PASSWORD    = 0x548a30f5,
    ID_ACCOUNT_PASSWORD        = 0x957b50fb,
    ID_PASSWORD_KDF_ALGO_SHA256 = 0x3a912d4a,
    ID_PASSWORD_KDF_ALGO_UNKNOWN = 0xd45ab096,
    ID_AUTH_CHECK_PASSWORD     = 0xd18b4d16,
    ID_INPUT_CHECK_PASSWORD_SRP = 0xd27ff082,

    // Phase 3 & 4: Dialogs & Messaging
    ID_MESSAGES_GET_DIALOGS    = 0xa0f4cb4f,
    ID_MESSAGES_DIALOGS        = 0x15ba6c40,
    ID_MESSAGES_DIALOGS_SLICE  = 0x71e094f3,
    ID_MESSAGES_DIALOGS_NOT_MODIFIED = 0xf0e3e596,
    ID_MESSAGES_GET_HISTORY    = 0x4423e6c5,
    ID_MESSAGES_SEND_MESSAGE   = 0x545cd15a,
    ID_MESSAGES_SENT_MESSAGE   = 0xd1f4ee35,
    ID_MESSAGES_MESSAGES       = 0x8c718e87,
    ID_MESSAGES_MESSAGES_SLICE = 0x3a20ecb8,
    ID_MESSAGES_CHANNEL_MESSAGES = 0xc776ba4e,

    ID_INPUT_PEER_EMPTY        = 0x7f3b18ea,
    ID_INPUT_PEER_SELF         = 0x7da07ec9,
    ID_INPUT_PEER_CHAT         = 0x35a95cb9,
    ID_INPUT_PEER_USER         = 0xdde8a54c,
    ID_INPUT_PEER_CHANNEL      = 0x27bcbbfc,

    ID_DIALOG                  = 0xd58a08c6,
    ID_DIALOG_FOLDER           = 0x714c7a79,

    ID_PEER_USER               = 0x59511722,
    ID_PEER_CHAT               = 0x36c6019a,
    ID_PEER_CHANNEL            = 0xa2a5371e,

    ID_USER                    = 0x31774388,
    ID_USER_EMPTY              = 0xd3bc4b7a,

    ID_CHAT                    = 0x41cbf256,
    ID_CHAT_EMPTY              = 0x29562764,
    ID_CHAT_FORBIDDEN          = 0x06592a1a,
    ID_CHANNEL                 = 0x83d3b767,
    ID_CHANNEL_FORBIDDEN       = 0x17d493d5,

    ID_MESSAGE                 = 0x3ae56482,
    ID_MESSAGE_EMPTY           = 0x90a6ca84,
    ID_MESSAGE_SERVICE         = 0x2b085862,

    // Real-Time Updates
    ID_UPDATES                 = 0x74ae4240,
    ID_UPDATES_COMBINED        = 0x78d4dec1,
    ID_UPDATE_SHORT_MESSAGE    = 0x313bc7f8,
    ID_UPDATE_SHORT_CHAT_MESSAGE = 0x4d6deea8,
    ID_UPDATE_NEW_MESSAGE      = 0x9a1caff9,
    ID_UPDATE_NEW_CHANNEL_MESSAGE = 0x51b2727a,

    // Phase 5: Media Subsystem Constructors
    ID_UPLOAD_GET_FILE            = 0xbe5335be,
    ID_UPLOAD_FILE                = 0x096a18d5,
    ID_UPLOAD_FILE_CDN_REDIRECT   = 0x0f592c47,

    ID_INPUT_PEER_PHOTO_FILE_LOCATION = 0x37257e99,
    ID_INPUT_PHOTO_FILE_LOCATION      = 0x40181ffe,
    ID_INPUT_DOCUMENT_FILE_LOCATION   = 0xbad07584,

    ID_STORAGE_FILE_UNKNOWN       = 0xaa963b05,
    ID_STORAGE_FILE_PARTIAL       = 0x40bc6f52,
    ID_STORAGE_FILE_JPEG          = 0x007efe0e,
    ID_STORAGE_FILE_GIF           = 0xcae1aadf,
    ID_STORAGE_FILE_PNG           = 0x0a4f63c0,
    ID_STORAGE_FILE_PDF           = 0xae1e508d,
    ID_STORAGE_FILE_MP3           = 0x528a0690,
    ID_STORAGE_FILE_MOV           = 0x4b09ebbc,
    ID_STORAGE_FILE_MP4           = 0xb330d69a,
    ID_STORAGE_FILE_WEBP          = 0x1081464c,

    ID_USER_PROFILE_PHOTO         = 0x82d1f706,
    ID_CHAT_PHOTO                 = 0x1c6e1c11,
    ID_PHOTO                      = 0xfb197a65,
    ID_PHOTO_EMPTY                = 0x2331b22d,
    ID_DOCUMENT                   = 0x8fd4e4d8,
    ID_DOCUMENT_EMPTY             = 0x36f8c871,

    ID_MESSAGE_MEDIA_PHOTO        = 0x695150d7,
    ID_MESSAGE_MEDIA_DOCUMENT     = 0x4cf4d72d,
    ID_MESSAGE_MEDIA_UNSUPPORTED  = 0x9f84f49e
};

} // namespace TL
} // namespace Telegram

#endif // TELEGRAM_TL_TYPES_H
