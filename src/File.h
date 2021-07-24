#pragma once

#include "Types.h"

#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <cstddef>

[[nodiscard]] extern std::optional<uint8_t> read_u8(std::ifstream& file);
[[nodiscard]] extern std::optional<uint16_t> read_u16(std::ifstream& file);
[[nodiscard]] extern std::optional<uint32_t> read_u32(std::ifstream& file);
[[nodiscard]] extern std::optional<uint64_t> read_u64(std::ifstream& file);
[[nodiscard]] extern std::optional<Number> read_number(std::ifstream& file);
[[nodiscard]] extern std::optional<std::string> read_string(std::ifstream& file);
[[nodiscard]] extern std::optional<std::vector<std::byte>> read_buffer(std::ifstream& file, uint32_t size);

extern bool write_u8(std::ofstream& file, uint8_t u8);
extern bool write_u16(std::ofstream& file, uint16_t u16);
extern bool write_u32(std::ofstream& file, uint32_t u32);
extern bool write_u64(std::ofstream& file, uint64_t u64);
extern bool write_number(std::ofstream& file, Number number);
extern bool write_string(std::ofstream& file, const std::string& string);
extern bool write_buffer(std::ofstream& file, const std::byte* buffer, uint32_t size);

[[nodiscard]] extern std::optional<std::ifstream> open_ifile(const std::string& filename);
[[nodiscard]] extern std::optional<std::ofstream> open_ofile(const std::string& filename);
