#include "persistence/binaryWriter.hpp"

#include <stdexcept>

BinaryWriter::BinaryWriter(const std::string &filename)
    : out_(filename, std::ios::binary)
{
  // out_ is operator overloaded
  if (!out_)
  {
    throw std::runtime_error("Failed to open file: " + filename);
  }
}

void BinaryWriter::writeUInt32(uint32_t value)
{
  // value has type uint32_t, but write needs const char*
  out_.write(
      reinterpret_cast<const char *>(&value),
      sizeof(value));

  if (!out_)
  {
    throw std::runtime_error("Failed writing uint32.");
  }
}

void BinaryWriter::writeBool(bool value)
{
  // serialize bool directly
  // although bool is implementation defined, its okay since 1byte for bool is basically ubiquitous
  // can prefer uint8_t for correctness
  out_.write(
      reinterpret_cast<const char *>(&value),
      sizeof(value));

  if (!out_)
  {
    throw std::runtime_error("Failed writing bool.");
  }
}

void BinaryWriter::writeString(const std::string &value)
{
  writeUInt32(static_cast<uint32_t>(value.size()));

  out_.write(value.data(), value.size());

  if (!out_)
  {
    throw std::runtime_error("Failed writing string.");
  }
}

// A note on portability

// This technique is fast and common, but it writes the machine's in-memory representation directly. That means the file depends on things like endianness (byte order). If you want a binary file format that works across all architectures, you usually choose a fixed byte order (often little-endian or network byte order) and serialize each byte explicitly rather than dumping the raw memory.
