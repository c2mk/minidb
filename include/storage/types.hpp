#pragma once

#include <string>
#include <variant>
#include <vector>

// This answers:
// "What kind of data is this column supposed to contain?"
// Describes Schema
// can change name to ColumnType instead of DataType
// or logicaltype vs physicaltype
enum class DataType
{
  Int,
  Text
};

enum class ValueType
{
  // This answers:
  // "What is the actual thing stored in this cell right now?"
  // Runtime value representation
  Null,
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