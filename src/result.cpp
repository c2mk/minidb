#include "result.hpp"

QueryResult QueryResult::ok(std::string msg)
{
  // aggregate initialization
  return QueryResult{true, std::move(msg), {}, {}};
}

QueryResult QueryResult::error(std::string msg)
{
  return QueryResult{false, std::move(msg), {}, {}};
}

QueryResult QueryResult::table(std::vector<std::string> columnNames,
                               std::vector<Row> rows,
                               std::string msg)
{
  return QueryResult{true, std::move(msg), std::move(columnNames), std::move(rows)};
}