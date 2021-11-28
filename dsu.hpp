#pragma once

#include "udp.hpp"

#include <stdint.h>

#define TYPE_PROTOCOL_INFO 0x100000
#define TYPE_CONTROLLER_INFO 0x100001
#define TYPE_CONTROLLER_DATA 0x100002

// -- protocol and packet --

struct DSUPacketHeader {
	char m_szHeader[4];
	uint16_t m_uiVersion;
	uint16_t m_uiLength;
	uint32_t m_uiCRC32; // crc32 of entire packet with this field zero'd out
	uint32_t m_uiCID; // client id
};

class DSU {
public:
	DSU();
public:
	uint32_t m_uiCID;
};

class DSUPacket {
public:
	DSUPacket(DSU* dsu, uint32_t type, uint8_t* data, uint16_t length);
	DSUPacket(UDPSocket sock);
	~DSUPacket();

public:
	// entire packet
	uint8_t* m_puiPacketData;

	// packet structure
	DSUPacketHeader m_pHeader;
	uint32_t m_uiMessageType;
	uint8_t* m_puiData;

	// packet info
	uint8_t m_uiPacketLength;
};

// -- helper structs & enums for controller status --

enum SlotState {
	NOT_CONNECTED = 0,
	RESERVED, // ??
	CONNECTED
};

enum DeviceModel {
	NONE = 0,
	NO_GYRO,
	FULL_GYRO
};

enum ConnectionType {
	NONE = 0,
	USB,
	BLUETOOTH
};

struct MAC {
	uint8_t m_uiMac[6];
};

enum BatteryStatus {
	NONE = 0,
	DYING,
	LOW,
	MEDIUM,
	HIGH,
	FULL,
	CHARGING = 0xEE,
	CHARGED
};

struct ControllerInfo {
	uint8_t m_uiSlot;
	SlotState m_eSlotState;
	DeviceModel m_eDeviceModel;
	MAC m_sMAC;
	BatteryStatus m_eBatteryStatus;
};


class Controller {
	Controller(DSUPacket* packet);
	ControllerInfo m_sInfo;
};