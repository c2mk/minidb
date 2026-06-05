#pragma once

#include <string>
#include <variant>
#include <vector>
#include <optional>

struct IntLiteral
{
  int value;
};

struct StringLiteral
{
  std::string value;
};

struct Column
{
  std::string name;
};

struct BinaryExp
{
  Column left;
  std::string op;
  std::variant<IntLiteral, StringLiteral, Column> right;
};

struct SelectStatement
{
  std::vector<std::string> columns;
  std::string table;

  std::optional<BinaryExp> where;
};

struct InsertStatement
{
  std::string table;
  std::vector<std::variant<IntLiteral, StringLiteral>> values;
};

struct ColumnDef
{
  std::string name;
  std::string type;
};

struct CreateTableStatement
{
  std::string table;
  std::vector<ColumnDef> columns;
};

struct DeleteStatement
{
  std::string table;

  bool hasWhere = false;
  std::optional<BinaryExp> where;
  // std::variant<std::monostate, BinaryExp> where;
  // monostate is a type that represents nothing
  // optional empty inside where
};

using Statement = std::variant<
    SelectStatement,
    InsertStatement,
    CreateTableStatement,
    DeleteStatement>;