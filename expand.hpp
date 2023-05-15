#pragma once

#include <limits>
#include <string>
#include <string_view>
#include <xieite/macros/ASSERT.hpp>
#include <xieite/math/minimalBits.hpp>

#include <iostream>

namespace xieite::hashes {
	[[nodiscard]]
	/* constexpr */ std::string expand(const std::string_view string) noexcept {
		constexpr std::size_t mapSize = std::numeric_limits<unsigned char>::max() - std::numeric_limits<unsigned char>::min() + 1;
		const std::size_t stringSize = string.size();
		XIEITE_ASSERT(stringSize >= mapSize, "Cannot expand invalid archive");
		std::array<char, mapSize> map;
		for (std::size_t i = 0; i < mapSize; ++i) {
			map[i] = string[i];
		}
		std::string result;
		std::size_t currentKeySize = 0;
		std::size_t currentKeySizeBit = 0;
		char currentKey = 0;
		std::size_t currentKeyBit = 0;
		const std::size_t keySizeSize = xieite::math::minimalBits(std::numeric_limits<unsigned char>::digits - 1);
		for (const char character : string.substr(mapSize)) {
			for (std::size_t i = 0; i < std::numeric_limits<unsigned char>::digits; ++i) {
				if (currentKeySizeBit < keySizeSize) {
					currentKeySize |= ((character >> i) & 1) << keySizeSize;
					++currentKeySizeBit;
				} else {
					if (currentKeyBit <= currentKeySize) {
						currentKey |= ((character >> i) & 1) << currentKeyBit;
						currentKeyBit = (currentKeyBit + 1) % currentKeySize;
						if (!currentKeyBit) {
							result += map[static_cast<unsigned char>(currentKey)];
							currentKeySize = currentKeySizeBit = currentKey = 0;
						}
					}
				}
			}
		}
		return result;
	}
}
