#pragma once

#include "catalog.hpp"
#include "result.hpp"
#include "parser/ast.hpp"

class Executor
{
public:
  explicit Executor(Catalog &catalog);

  QueryResult execute(const Statement &stmt);

private:
  QueryResult executeCreate(const CreateTableStatement &stmt);
  QueryResult executeInsert(const InsertStatement &stmt);
  QueryResult executeSelect(const SelectStatement &stmt);
  QueryResult executeDelete(const DeleteStatement &stmt);

private:
  Catalog &catalog_;
};