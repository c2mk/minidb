#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <type_traits>

class BinaryWriter
{
public:
  explicit BinaryWriter(const std::string &filename);

  // int32 because dont wanna depend on number ob bits int holds
  void writeUInt32(uint32_t value);
  void writeBool(bool value);
  void writeString(const std::string &value);

  template <typename Enum>
  void writeEnum(Enum value)
  {
    static_assert(std::is_enum_v<Enum>,
                  "writeEnum requires an enum type");

    writeUInt32(static_cast<uint32_t>(value));
  }

private:
  std::ofstream out_;
};

// what about writeSchema?
// binary reader/writer is generic, so shouldnt know about DB