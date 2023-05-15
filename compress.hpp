#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <xieite/math/minimalBits.hpp>

namespace xieite::hashes {
	[[nodiscard]]
	constexpr std::string compress(const std::string_view string) noexcept {
		constexpr std::size_t mapSize = std::numeric_limits<unsigned char>::max() + 1;
		std::array<std::pair<char, std::size_t>, mapSize> frequencies;
		for (std::size_t i = 0; i < mapSize; ++i) {
			frequencies[i].first = i;
			frequencies[i].second = 0;
		}
		for (const char character : string) {
			++frequencies[static_cast<unsigned char>(character)].second;
		}
		std::sort(frequencies.begin(), frequencies.end(), [](const std::pair<char, std::size_t> frequency1, const std::pair<char, std::size_t> frequency2) noexcept -> bool {
			return frequency1.second > frequency2.second;
		});
		std::array<char, mapSize> map;
		std::string result;
		for (std::size_t i = 0; i < mapSize; ++i) {
			map[static_cast<unsigned char>(frequencies[i].first)] = i;
			result += frequencies[i].first;
		}
		std::size_t currentBit = 0;
		char currentCharacter = 0;
		const std::size_t keySizeSize = xieite::math::minimalBits(std::numeric_limits<unsigned char>::digits - 1);
		for (const char character : string) {
			const std::size_t key = map[static_cast<unsigned char>(character)];
			const std::size_t keySize = xieite::math::minimalBits(key);
			for (std::size_t i = 0; i < (keySizeSize + keySize); ++i) {
				currentCharacter |= ((((i < keySizeSize) ? (keySize - 1) : key) >> (i - keySizeSize * (i >= keySizeSize))) & 1) << currentBit;
				currentBit = (currentBit + 1) % std::numeric_limits<unsigned char>::digits;
				if (!currentBit) {
					result += currentCharacter;
					currentCharacter = 0;
				}
			}
		}
		return result;
	}
}
