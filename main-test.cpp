#include "parser/parser.hpp"
#include "parser/tokenizer.hpp"

#include <iostream>
#include <variant>
#include <type_traits>

int main()
{
  std::string input = "SELECT id, name FROM student";

  Tokenizer tokenizer(input);
  auto tokens = tokenizer.tokenize();
  // printTokens(tokens);
  Parser parser(tokens);
  Statement stmt = parser.parse();
  std::cout << "Parsed successfully\n";

  return 0;
}