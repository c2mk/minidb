#include "result.hpp"

QueryResult QueryResult::ok(std::string msg)
{
  return {.success = true, .message = std::move(msg)};
  // unmentioned fields (columnNames, rows) are default-initialized to empty
}

QueryResult QueryResult::error(std::string msg)
{
  return {.success = false, .message = std::move(msg)};
}

QueryResult QueryResult::table(std::vector<std::string> columnNames,
                               std::vector<Row> rows,
                               std::string msg)
{
  return {
      .success = true,
      .message = std::move(msg),
      .columnNames = std::move(columnNames),
      .rows = std::move(rows)};
}