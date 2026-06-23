#include "dispatcher.hpp"

#include <iostream>
#include <variant>

namespace
{
  void printValue(const Value &value)
  {
    if (std::holds_alternative<int>(value))
    {
      std::cout << std::get<int>(value);
    }
    else if (std::holds_alternative<std::string>(value))
    {
      std::cout << std::get<std::string>(value);
    }
    else if (std::holds_alternative<std::monostate>(value))
    {
      std::cout << "NULL";
    }
  }
}

Dispatcher::Dispatcher() : executor_(catalog_) {}

void Dispatcher::dispatch(const Statement &stmt)
{
  auto result = executor_.execute(stmt);

  if (!result.success)
  {
    std::cout << result.message << '\n';
    return;
  }

  if (result.columnNames.empty())
  {
    std::cout << result.message << '\n';
    return;
  }

  for (size_t i = 0; i < result.columnNames.size(); ++i)
  {
    std::cout << result.columnNames[i];

    if (i + 1 < result.columnNames.size())
    {
      std::cout << " | ";
    }
  }

  std::cout << '\n';

  for (const auto &row : result.rows)
  {
    for (size_t i = 0; i < row.size(); ++i)
    {
      printValue(row[i]);

      if (i + 1 < row.size())
      {
        std::cout << " | ";
      }
    }

    std::cout << '\n';
  }
}
