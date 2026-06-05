#include "shell.hpp"
#include "dispatcher.hpp"
#include "parser/tokenizer.hpp"
#include "parser/parser.hpp"

#include <iostream>
#include <string>

void Shell::run()
{
  Dispatcher dispatcher;
  while (true)
  {
    std::cout << "db> ";

    std::string input;
    if (!std::getline(std::cin, input))
    {
      break;
    }
    if (input.empty())
    {
      continue;
    }
    try
    {
      Tokenizer tokenizer(input);
      Parser parser(tokenizer.tokenize());
      dispatcher.dispatch(parser.parse());
    }
    catch (const std::exception &err)
    {
      std::cout << "Error: " << err.what() << '\n';
    }
  }
  std::cout << "Bye!\n";
}