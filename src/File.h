#pragma once

#include "Number.h"

#include <fstream>
#include <string>
#include <vector>
#include <optional>

std::optional<uint8_t> read_u8(std::ifstream& file);
std::optional<uint16_t> read_u16(std::ifstream& file);
std::optional<uint32_t> read_u32(std::ifstream& file);
std::optional<uint64_t> read_u64(std::ifstream& file);
std::optional<Number> read_number(std::ifstream& file);
std::optional<std::string> read_string(std::ifstream& file);
std::optional<std::vector<char>> read_buffer(std::ifstream& file, uint32_t size);

bool write_u8(std::ofstream& file, uint8_t u8);
bool write_u16(std::ofstream& file, uint16_t u16);
bool write_u32(std::ofstream& file, uint32_t u32);
bool write_u64(std::ofstream& file, uint64_t u64);
bool write_number(std::ofstream& file, Number number);
bool write_string(std::ofstream& file, const std::string& string);
bool write_buffer(std::ofstream& file, const char* buffer, uint32_t size);

std::optional<std::ifstream> open_ifile(const std::string& filename);
std::optional<std::ofstream> open_ofile(const std::string& filename);
