#include "shell.hpp"

#include <iostream>

int main()
{
  std::cout << "DBMS started\n";
  Shell shell;
  shell.run();
}