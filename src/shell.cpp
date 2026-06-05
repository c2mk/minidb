#include "shell.hpp"
#include "command.hpp"
#include "dispatcher.hpp"

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
    Command cmd = classifyCommand(input);
    if (cmd.type == CommandType::Quit)
    {
      break;
    }
    dispatcher.dispatchCommand(cmd);
  }
  std::cout << "Bye!\n";
}