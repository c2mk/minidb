#include "dispatcher.hpp"

#include <iostream>

void Dispatcher::dispatch(const Statement &stmt)
{
  if (std::holds_alternative<SelectStatement>(stmt))
  {
    handleSelect(std::get<SelectStatement>(stmt));
  }
  else if (std::holds_alternative<InsertStatement>(stmt))
  {
    handleInsert(std::get<InsertStatement>(stmt));
  }
  else if (std::holds_alternative<CreateTableStatement>(stmt))
  {
    handleCreateTable(std::get<CreateTableStatement>(stmt));
  }
  else if (std::holds_alternative<DeleteStatement>(stmt))
  {
    handleDelete(std::get<DeleteStatement>(stmt));
  }
}

void Dispatcher::handleSelect(const SelectStatement &stmt)
{
  std::cout << "SELECT statement\n";
  std::cout << stmt.table << '\n';
  for (auto column : stmt.columns)
    std::cout << column << " ";
  std::cout << '\n';
}

void Dispatcher::handleDelete(const DeleteStatement &stmt)
{
  std::cout << "DELETE statement\n";
  std::cout << stmt.table << '\n';
}

void Dispatcher::handleInsert(const InsertStatement &stmt)
{
  std::cout << "INSERT statement\n";
  std::cout << stmt.table << '\n';
}

void Dispatcher::handleCreateTable(const CreateTableStatement &stmt)
{
  std::cout << "CREATE TABLE statement\n";
  std::cout << stmt.table << '\n';
}