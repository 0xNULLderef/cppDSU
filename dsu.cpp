#include "dsu.hpp"

#include "crc32.hpp"

#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <climits>
#include <random>

#undef max // fuck you windows socket header for defining something this stupid

// should be bigger then max packet size (rn more or less 100)
#define RECV_BUF_SIZE 128

DSU::DSU() {
	CRC32::Init();
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist_u32(0, std::numeric_limits<uint32_t>::max());
	this->m_uiCID = dist_u32(rng);
}


DSUPacket::DSUPacket(DSU* dsu, uint32_t type, uint8_t* data, uint16_t length) {
	if (!dsu) throw std::invalid_argument("dsu = nullptr");
	if (!data) throw std::invalid_argument("data = nullptr");
	if (type != TYPE_PROTOCOL_INFO && type != TYPE_CONTROLLER_INFO && type != TYPE_CONTROLLER_DATA) throw std::invalid_argument("invalid type");
	this->m_puiData = (uint8_t*)malloc(length);
	memcpy(this->m_puiData, data, length);
	this->m_uiPacketLength = sizeof(DSUPacketHeader) + 4 + length;
	this->m_uiMessageType = type;
	this->m_puiPacketData = (uint8_t*)malloc(this->m_uiPacketLength);
	this->m_pHeader.m_szHeader[0] = 'D';
	this->m_pHeader.m_szHeader[1] = 'S';
	this->m_pHeader.m_szHeader[2] = 'U';
	this->m_pHeader.m_szHeader[3] = 'C';
	this->m_pHeader.m_uiVersion = 1001;
	this->m_pHeader.m_uiLength = 4 + length; // 4 + length of data since the type counts towards length
	this->m_pHeader.m_uiCID = dsu->m_uiCID;
	memcpy(this->m_puiPacketData, &this->m_pHeader, sizeof(DSUPacketHeader)); // set packet header
	memcpy((void*)((uintptr_t)this->m_puiPacketData + sizeof(DSUPacketHeader)), &type, 4); // set packet type
	memcpy((void*)((uintptr_t)this->m_puiPacketData + sizeof(DSUPacketHeader) + 4), data, length); // set packet data
	this->m_pHeader.m_uiCRC32 = CRC32::update(0, this->m_puiPacketData, this->m_uiPacketLength);
	memcpy((void*)((uintptr_t)this->m_puiPacketData + offsetof(DSUPacketHeader, m_uiCRC32)), &this->m_pHeader.m_uiCRC32, 4); // update packet crc
}

DSUPacket::DSUPacket(UDPSocket sock) {
	void* buf = malloc(RECV_BUF_SIZE);
	sock.RecvFrom((char*)buf, RECV_BUF_SIZE);
	memcpy(&this->m_pHeader, buf, sizeof(DSUPacketHeader));
	this->m_uiPacketLength = sizeof(DSUPacketHeader) + this->m_pHeader.m_uiLength;
	this->m_puiPacketData = (uint8_t*)malloc(this->m_uiPacketLength);
	memcpy(this->m_puiPacketData, buf, this->m_uiPacketLength);
	memcpy(&this->m_uiMessageType, (void*)((uintptr_t)buf + sizeof(DSUPacketHeader)), 4);
	this->m_puiData = (uint8_t*)malloc(this->m_pHeader.m_uiLength - 4);
	memcpy(this->m_puiData, (void*)((uintptr_t)buf + sizeof(DSUPacketHeader) + 4), this->m_pHeader.m_uiLength - 4);
}

DSUPacket::~DSUPacket() {
	free(this->m_puiPacketData);
}