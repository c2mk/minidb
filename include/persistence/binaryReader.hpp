#pragma once

#include <fstream>
#include <cstdint>
#include <string>

class BinaryReader
{
public:
  explicit BinaryReader(const std::string &filename);
  uint32_t readUInt32();
  bool readBool();
  std::string readString();

private:
  std::ifstream in_;
};