#include "File.h"

std::optional<uint8_t> read_u8(std::ifstream& file)
{
	uint8_t u8;
	if (file.read(reinterpret_cast<char*>(&u8), 1))
	{
		return u8;
	}
	return {};
}

std::optional<uint16_t> read_u16(std::ifstream& file)
{
	std::optional<uint8_t> u8[2];
	u8[0] = read_u8(file);
	if (!u8[0].has_value())
	{
		return {};
	}
	u8[1] = read_u8(file);
	if (!u8[1].has_value())
	{
		return {};
	}
	return (uint16_t(u8[1].value()) << 8) | u8[0].value();
}

std::optional<uint32_t> read_u32(std::ifstream& file)
{
	std::optional<uint16_t> u16[2];
	u16[0] = read_u16(file);
	if (!u16[0].has_value())
	{
		return {};
	}
	u16[1] = read_u16(file);
	if (!u16[1].has_value())
	{
		return {};
	}
	return (uint32_t(u16[1].value()) << 16) | u16[0].value();
}

std::optional<uint64_t> read_u64(std::ifstream& file)
{
	std::optional<uint32_t> u32[2];
	u32[0] = read_u32(file);
	if (!u32[0].has_value())
	{
		return {};
	}
	u32[1] = read_u32(file);
	if (!u32[1].has_value())
	{
		return {};
	}
	return (uint64_t(u32[1].value()) << 32) | u32[0].value();
}

std::optional<Number> read_number(std::ifstream& file)
{
	std::optional<uint16_t> u16[2];
	u16[0] = read_u16(file);
	if (!u16[0].has_value())
	{
		return {};
	}
	u16[1] = read_u16(file);
	if (!u16[1].has_value())
	{
		return {};
	}
	return Number(u16[0].value()) / Number(u16[1].value());
}

std::optional<std::string> read_string(std::ifstream& file)
{
	std::optional<uint32_t> size = read_u32(file);
	if (!size.has_value())
	{
		return {};
	}
	auto buffer = read_buffer(file, size.value());
	if (!buffer.has_value())
	{
		return {};
	}
	return std::string(reinterpret_cast<char*>(buffer.value().data()), size.value());
}

std::optional<std::vector<std::byte>> read_buffer(std::ifstream& file, uint32_t size)
{
	std::vector<std::byte> buffer(size);
	if (file.read(reinterpret_cast<char*>(buffer.data()), size))
	{
		return buffer;
	}
	return {};
}

bool write_u8(std::ofstream& file, uint8_t u8)
{
	return bool(file.write(reinterpret_cast<char*>(&u8), 1));
}

bool write_u16(std::ofstream& file, uint16_t u16)
{
	if (write_u8(file, u16))
	{
		return write_u8(file, u16 >> 8);
	}
	return false;
}

bool write_u32(std::ofstream& file, uint32_t u32)
{
	if (write_u16(file, u32))
	{
		return write_u16(file, u32 >> 16);
	}
	return false;
}

bool write_u64(std::ofstream& file, uint64_t u64)
{
	if (write_u32(file, u64))
	{
		return write_u32(file, u64 >> 32);
	}
	return false;
}

bool write_number(std::ofstream& file, Number number)
{
	if (write_u16(file, number > 1.0L ? 65535 : std::round(65535.0L * number)))
	{
		return write_u16(file, number > 1.0L ? std::round(65535.0L / number) : 65535);
	}
	return false;
}

bool write_string(std::ofstream& file, const std::string& string)
{
	if (write_u32(file, string.size()))
	{
		return write_buffer(file, reinterpret_cast<const std::byte*>(string.c_str()), string.size());
	}
	return false;
}

bool write_buffer(std::ofstream& file, const std::byte* buffer, uint32_t size)
{
	return bool(file.write(reinterpret_cast<const char*>(buffer), size));
}

std::optional<std::ifstream> open_ifile(const std::string& filename)
{
	std::ifstream file;
	file.open(filename, std::ios::binary);
	if (file)
	{
		return file;
	}
	return {};
}

std::optional<std::ofstream> open_ofile(const std::string& filename)
{
	std::ofstream file;
	file.open(filename, std::ios::binary);
	if (file)
	{
		return file;
	}
	return {};
}
