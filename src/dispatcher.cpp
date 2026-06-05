#include "dispatcher.hpp"

#include <iostream>
#include <string>

void Dispatcher::dispatchCommand(const Command &cmd)
{
  // notice how switch works with enum even though enum cant be implicitly cast
  switch (cmd.type)
  {
  case CommandType::Help:
    std::cout << "Help: CREATE, INSERT, SELECT, DELETE, EXIT\n";
    break;

  case CommandType::CreateTable:
    std::cout << "CREATE TABLE handler called\n";
    break;

  case CommandType::Insert:
    std::cout << "INSERT handler called\n";
    break;

  case CommandType::Select:
    std::cout << "SELECT handler called\n";
    break;

  case CommandType::Delete:
    std::cout << "DELETE handler called\n";
    break;

  default:
    std::cout << "Unknown command\n";
    break;
  }
}