#pragma once
#include <string>

enum class CommandType
{
  CreateTable,
  Insert,
  Select,
  Delete,
  Help,
  Quit,
  Unknown
  // these are enumerators, each gets assigned an integer
};

struct Command
{
  CommandType type;
  std::string raw;
};

Command classifyCommand(const std::string &input);