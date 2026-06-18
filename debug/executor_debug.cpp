#include "catalog.hpp"
#include "executor.hpp"

#include <iostream>

int main()
{
  Catalog catalog;
  Executor executor(catalog);

  try
  {
    CreateTableStatement create{
        .table = "users",
        .columns = {
            {"id", "INT"},
            {"name", "TEXT"}}};

    executor.execute(create);

    InsertStatement insert{
        .table = "users",
        .values = {
            IntLiteral{1},
            StringLiteral{"Alice"}}};

    executor.execute(insert);

    SelectStatement select{
        .columns = {"*"},
        .table = "users"};

    auto result =
        executor.execute(select);

    std::cout
        << "Rows: "
        << result.rows.size()
        << '\n';
  }
  catch (const std::exception &e)
  {
    std::cout
        << e.what()
        << '\n';
  }
}