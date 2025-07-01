#include "constants.h"

const std::map<DWORD, const char*> opcodeMap = {
    { 0x20080,   "X_CONNECT_REQ" },
    { 0x20081,   "X_HANDSHAKE_ACK" },
    { 0x2008A,   "X_LOGIN_RES" },
    { 0x3010001, "X_GAME_CON" },
    { 0x4000014, "X_LOGIN_RES2" },
    { 0x7000002, "X_NOTIF_SEND" },
    { 0x700000D, "X_RETURN_PLAYER" },
    { 0x50D0006, "X_PRIV_MSG" }
};