#pragma once
#include "parser/ast.hpp"

class Dispatcher
{
public:
  void dispatch(const Statement &stmt);

private:
  void handleSelect(const SelectStatement &stmt);
  void handleInsert(const InsertStatement &stmt);
  void handleCreateTable(const CreateTableStatement &stmt);
  void handleDelete(const DeleteStatement &stmt);
};