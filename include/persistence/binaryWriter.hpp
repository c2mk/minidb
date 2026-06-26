#pragma once

#include <cstdint>
#include <fstream>
#include <string>

class BinaryWriter
{
public:
  explicit BinaryWriter(const std::string &filename);

  // int32 because dont wanna depend on number ob bits int holds
  void writeUInt32(uint32_t value);
  void writeBool(bool value);
  void writeString(const std::string &value);

private:
  std::ofstream out_;
};