#pragma once

void DecryptData(unsigned char* PacketData, unsigned char* DecryptedData, int Length);
void DecodePacket(const unsigned char* data, int length);
void PrintHexAsciiDump(const unsigned char* data, int length);