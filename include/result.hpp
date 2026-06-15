#pragma once

#include "storage/types.hpp"

#include <string>
#include <vector>

struct QueryResult
{
  bool success;
  std::string message;
  std::vector<std::string> columnNames;
  std::vector<Row> rows;

  static QueryResult ok(std::string msg = "OK");
  static QueryResult error(std::string msg);
  static QueryResult table(std::vector<std::string> columnNames, std::vector<Row> rows, std::string msg = "");
};