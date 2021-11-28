#pragma once

#include <stdint.h>

static uint32_t g_uiCrcTable[256];

namespace CRC32 {
	static void generate_table(uint32_t(&table)[256]) {
		uint32_t polynomial = 0xEDB88320;
		for (uint32_t i = 0; i < 256; i++) {
			uint32_t c = i;
			for (size_t j = 0; j < 8; j++) {
				if (c & 1) {
					c = polynomial ^ (c >> 1);
				} else {
					c >>= 1;
				}
			}
			table[i] = c;
		}
	}
	static void Init() {
		generate_table(g_uiCrcTable);
	}
	static uint32_t update(uint32_t initial, const void* buf, size_t len) {
		uint32_t c = initial ^ 0xFFFFFFFF;
		const uint8_t* u = static_cast<const uint8_t*>(buf);
		for (size_t i = 0; i < len; ++i) {
			c = g_uiCrcTable[(c ^ u[i]) & 0xFF] ^ (c >> 8);
		}
		return c ^ 0xFFFFFFFF;
	}
};
