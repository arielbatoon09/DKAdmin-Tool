#pragma once
#include <windows.h>

#pragma pack(push, 1)
struct PacketHeader {
    DWORD packetcrc;
    WORD packetnum;
    WORD packetsize;
    DWORD timestamp;
    DWORD packetopcode;
};

struct X_CONNECT_REQ : public PacketHeader { DWORD something; };
struct X_HANDSHAKE_ACK : public PacketHeader {
    DWORD dwSeverTick;
    DWORD dwClientTick;
    BYTE bCryptKeyLow;
    BYTE bPad1[3];
    BYTE bCryptKeyHi;
    BYTE bPad2[3];
    BYTE nUnknown[0x20];
};
struct X_GAME_CON : public PacketHeader { char szCharName[0x1C]; };
struct X_LOGIN_RES : public PacketHeader {
    DWORD dwSerialKey;
    DWORD dwResult;
    DWORD dwSessionSvrID;
    char AccNo[0x10];
};
struct X_LOGIN_RES2 : public PacketHeader { DWORD dwHellKnows; DWORD dwOtherSerialKey; };
struct X_NOTIF_SEND : public PacketHeader { WORD wMsgSize; WORD dwUnknown; DWORD wUnknown; char szMsg[800]; };
struct X_RETURN_PLAYER : public PacketHeader { char szCharName[0x20]; };
struct X_PRIV_MSG : public PacketHeader { BYTE bSenderName; BYTE bRecvName; BYTE msgSize; BYTE bUnknown; DWORD dwUnknown1; DWORD dwunknown2; };
#pragma pack(pop)