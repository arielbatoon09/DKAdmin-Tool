#include <iostream>
#include "crypto.h"
#include "table.h"
#include "packet_structs.h"
#include "constants.h"
#include <iomanip>

BYTE CurrentFlagByte = 0x0F;

void PrintHexAsciiDump(const unsigned char* data, int length) {
	std::cout << "[Hex + ASCII Dump]\n";
	for (int i = 0; i < length; i += 16) {
		std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
		for (int j = 0; j < 16; ++j) {
			if (i + j < length)
				std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)data[i + j] << " ";
			else
				std::cout << "   ";
		}
		std::cout << "  ";
		for (int j = 0; j < 16; ++j) {
			if (i + j < length) {
				char c = data[i + j];
				std::cout << (isprint(static_cast<unsigned char>(c)) ? c : '.');
			}
		}
		std::cout << "\n";
	}

	std::cout << "[Decrypted ASCII] ";
	for (int i = 0; i < length; ++i) {
		char c = data[i];
		std::cout << (isprint(static_cast<unsigned char>(c)) ? c : '.');
	}
	std::cout << "\n";
}

void DecryptData(unsigned char* PacketData, unsigned char* DecryptedData, int Length) {
	unsigned long DwordTableVal = 0xFFFFFFFF;
	long ByteTableVal = 0;
	long DecryptionFlagByte = PacketData[4] ^ 0x19;
	long CurrentByte = 0;

	for (int i = 0; i < Length; i++) {
		CurrentByte = (PacketData[i] ^ DwordTableVal) & 0xFF;
		DwordTableVal = (DwordTableVal >> 8) ^ Table[CurrentByte];
		ByteTableVal = ByteTable[i & 0xFF];
		DecryptedData[i] = static_cast<unsigned char>((ByteTableVal ^ PacketData[i]) ^ DecryptionFlagByte);
	}
}

void DecodePacket(const unsigned char* data, int length) {
	const PacketHeader* hdr = reinterpret_cast<const PacketHeader*>(data);
	std::cout << "[Decoded] Opcode=0x" << std::hex << hdr->packetopcode;
		auto it = opcodeMap.find(hdr->packetopcode);
	if (it != opcodeMap.end())
		std::cout << " (" << it->second << ")";
	std::cout << std::dec << "\n";

	std::cout << "[Opcode Bytes] "
		<< std::hex << (int)data[10] << " "
		<< (int)data[11] << " "
		<< (int)data[12] << " "
		<< (int)data[13] << "\n";

	PrintHexAsciiDump(data, length);

	if (hdr->packetopcode == 0x81000200) {
		BYTE lo = data[20];
		BYTE hi = data[21];
		CurrentFlagByte = lo ^ hi;
		std::cout << "  Updated FlagByte from response = 0x" << std::hex << (int)CurrentFlagByte << std::dec << "\n";

		switch (hdr->packetopcode) {
		case 0x20080: {
			const X_CONNECT_REQ* pkt = reinterpret_cast<const X_CONNECT_REQ*>(data);
			std::cout << "  X_CONNECT_REQ.something = " << pkt->something << "\n";
			break;
		}
		case 0x20081: {
			const X_HANDSHAKE_ACK* pkt = reinterpret_cast<const X_HANDSHAKE_ACK*>(data);
			std::cout << "  ServerTick=" << pkt->dwSeverTick << ", ClientTick=" << pkt->dwClientTick
				<< ", CryptKeyLow=0x" << std::hex << (int)pkt->bCryptKeyLow
				<< ", CryptKeyHi=0x" << (int)pkt->bCryptKeyHi << std::dec << "\n";
			break;
		}
		case 0x3010001: {
			const X_GAME_CON* pkt = reinterpret_cast<const X_GAME_CON*>(data);
			std::cout << "  CharName: " << pkt->szCharName << "\n";
			break;
		}
		case 0x2008A: {
			const X_LOGIN_RES* pkt = reinterpret_cast<const X_LOGIN_RES*>(data);
			std::cout << "  SerialKey=" << pkt->dwSerialKey << ", Result=" << pkt->dwResult
				<< ", SessionSvrID=" << pkt->dwSessionSvrID << ", AccNo=" << pkt->AccNo << "\n";
			break;
		}
		default:
			std::cout << "  No parser implemented.\n";
		}
	}
}