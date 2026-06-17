#pragma once

#include "catalog.hpp"
#include "executor.hpp"
#include "parser/ast.hpp"

class Dispatcher
{
public:
  Dispatcher();

  void dispatch(const Statement &stmt);

private:
  Catalog catalog_;
  Executor executor_;
};