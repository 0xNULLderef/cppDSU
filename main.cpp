#include "udp.hpp"
#include "dsu.hpp"

#include "util.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main() {
	try {
		WSASession session;
		UDPSocket socket;
		UDPAddress* addr = new UDPAddress("127.0.0.1", 26760);

		DSU* dsu = new DSU();

		uint8_t slotQuery[] = { 0x01, 0x00, 0x00, 0x00, 0x00 }; // query for 1 slot (slot 0)
		DSUPacket* send = new DSUPacket(dsu, TYPE_CONTROLLER_INFO, slotQuery, sizeof(slotQuery));
		socket.SendTo(addr, (const char*)send->m_puiPacketData, send->m_uiPacketLength);

		DSUPacket* recv = new DSUPacket(socket);
		DumpHex(recv->m_puiPacketData, recv->m_uiPacketLength);
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}