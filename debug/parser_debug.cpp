#include "parser/tokenizer.hpp"
#include "parser/parser.hpp"

#include <iostream>

int main()
{
  // std::string sql = "SELECT name, age FROM users WHERE age = 21;";
  // std::string sql = "CREATE TABLE t (id INT PRIMARY KEY,name TEXT NOT NULL);";
  std::string sql = "CREATE TABLE t (id INT NOT NULL PRIMARY KEY);";
  // std::string sql = "CREATE TABLE t (id INT PRIMARY);";

  try
  {
    Tokenizer tokenizer(sql);
    auto tokens = tokenizer.tokenize();

    std::cout << "TOKENS\n";
    for (const auto &token : tokens)
    {
      std::cout
          << static_cast<int>(token.type)
          << " : "
          << token.lexeme
          << '\n';
    }

    Parser parser(std::move(tokens));
    auto stmt = parser.parse();

    std::cout << "\nPARSE SUCCESS\n";
  }
  catch (const std::exception &e)
  {
    std::cout
        << "ERROR: "
        << e.what()
        << '\n';
  }
}