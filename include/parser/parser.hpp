#pragma once

#include "tokenizer.hpp"
#include "ast.hpp"

class Parser
{
public:
  // why explicit?
  explicit Parser(std::vector<Token> tokens);
  Statement parse();

private:
  std::vector<Token> tokens_;
  size_t pos_ = 0;

  // helpers
  // why const like this?
  Token peek() const;
  Token advance();
  bool match(TokenType type);
  bool atEnd() const;

  // grammar rules
  Statement parseStatement();
  SelectStatement parseSelect();
  InsertStatement parseInsert();
  DeleteStatement parseDelete();
  CreateTableStatement parseCreateTable();
  BinaryExp parseBinaryExp();
};