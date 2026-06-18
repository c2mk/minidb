#include "parser/tokenizer.hpp"
#include "parser/parser.hpp"

#include "catalog.hpp"
#include "executor.hpp"

#include <iostream>

int main()
{
  Catalog catalog;
  Executor executor(catalog);

  std::vector<std::string> queries{
      "CREATE TABLE users(id INT, name TEXT);",
      "INSERT INTO users VALUES(1,'Alice');",
      "SELECT * FROM users;"};

  for (const auto &sql : queries)
  {
    try
    {
      Tokenizer tokenizer(sql);
      auto tokens =
          tokenizer.tokenize();

      Parser parser(std::move(tokens));

      auto stmt =
          parser.parse();

      auto result =
          executor.execute(stmt);

      std::cout
          << "SUCCESS: "
          << sql
          << '\n';
    }
    catch (const std::exception &e)
    {
      std::cout
          << "ERROR: "
          << e.what()
          << '\n';
    }
  }
}