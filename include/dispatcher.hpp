#pragma once
#include "command.hpp"

class Dispatcher
{
public:
  void dispatchCommand(const Command &cmd);
};