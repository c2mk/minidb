#include "storage/table.hpp"

#include <iostream>

int main()
{
  Schema schema{
      {"id", DataType::Int},
      {"name", DataType::Text}};

  TableHeap table(schema);

  Row r1{
      1,
      std::string("Alice")};

  Row r2{
      2,
      std::string("Bob")};

  table.insertRow(std::move(r1));
  table.insertRow(std::move(r2));

  std::cout
      << "Row count: "
      << table.rows().size()
      << '\n';

  table.deleteRows({0});

  std::cout
      << "After delete: "
      << table.rows().size()
      << '\n';
}