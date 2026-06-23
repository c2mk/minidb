#pragma once

#include <string>
#include <variant>
#include <vector>

enum class DataType
{
  Int,
  Text
};

// If they were defined as structs, inserting values would be ugly
using Value = std::variant<std::monostate, int, std::string>;
using Row = std::vector<Value>;

struct ColumnSchema
{
  std::string name;
  DataType type;

  bool isPrimaryKey = false;
  bool isNotNull = false;
};

using Schema = std::vector<ColumnSchema>;

DataType parseDataType(const std::string &s);