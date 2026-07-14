#pragma once

#include <fstream>
#include <cstdint>
#include <string>
#include <type_traits>

class BinaryReader
{
public:
  explicit BinaryReader(const std::string &filename);
  uint32_t readUInt32();
  bool readBool();
  std::string readString();

  template <typename Enum>
  Enum readEnum()
  {
    static_assert(std::is_enum_v<Enum>,
                  "readEnum requires an enum type");

    return static_cast<Enum>(readUInt32());
  }

private:
  std::ifstream in_;
};