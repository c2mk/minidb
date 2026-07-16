#include "persistence/binaryReader.hpp"
#include <stdexcept>

BinaryReader::BinaryReader(const std::string &filename) : in_(filename, std::ios::binary)
{
  if (!in_)
  {
    throw std::runtime_error("Failed to open File: " + filename);
  }
}

uint32_t BinaryReader::readUInt32()
{
  uint32_t value;

  // notice char* instead of const char* as in writer bcs we are filling memory instead of reading.
  in_.read(
      reinterpret_cast<char *>(&value),
      sizeof(value));

  if (!in_)
  {
    throw std::runtime_error("Failed reading uint32.");
  }

  return value;
}

int32_t BinaryReader::readInt32()
{
  int32_t value;

  in_.read(
      reinterpret_cast<char *>(&value),
      sizeof(value));

  return value;
}

bool BinaryReader::readBool()
{
  bool value;

  in_.read(
      reinterpret_cast<char *>(&value),
      sizeof(value));

  if (!in_)
  {
    throw std::runtime_error("Failed reading bool.");
  }

  return value;
}

std::string BinaryReader::readString()
{
  uint32_t length = readUInt32();

  std::string value(length, '\0');
  // why not std::string value;
  // read() writes directly into the string's buffer.
  // The string must already have allocated memory.
  // This creates exactly length bytes.
  in_.read(value.data(), length);
  // why is vallue.data() writable
  // value.data() returns char* since c++17 for a non-const string
  if (!in_)
  {
    throw std::runtime_error("Failed reading string.");
  }

  return value;
}

// DataType BinaryReader::readDataType()
// {
//   return static_cast<DataType>(readUInt32());
// }