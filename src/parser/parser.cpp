#include "parser/parser.hpp"

#include <vector>
#include <stdexcept>
#include <iostream>

// std::move makes it O(1), else copying would be costly
Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

const Token &Parser::peek() const
{
  if (pos_ >= tokens_.size())
    throw std::runtime_error("Unexpected end of input");

  return tokens_[pos_];
}

const Token &Parser::advance()
{
  if (pos_ >= tokens_.size())
    throw std::runtime_error("Unexpected end of input");
  return tokens_[pos_++];
}

bool Parser::atEnd() const
{
  return pos_ >= tokens_.size();
}

bool Parser::match(TokenType type)
{
  // later : throw rtes
  if (atEnd())
    return false;

  if (peek().type != type)
  {
    return false;
  }
  advance();
  return true;
}

Statement Parser::parse()
{
  Statement stmt = parseStatement();

  if (!match(TokenType::Semicolon))
  {
    throw std::runtime_error(
        "Expected ';' after statement");
  }

  if (!atEnd())
  {
    throw std::runtime_error(
        "Unexpected token after statement");
  }

  return stmt;
}

Statement Parser::parseStatement()
{
  const Token &token = peek();
  switch (token.type)
  {
  case TokenType::Select:
    return parseSelect();
  case TokenType::Insert:
    return parseInsert();
  case TokenType::Delete:
    return parseDelete();
  case TokenType::Create:
    return parseCreateTable();
  default:
    throw std::runtime_error("Unknown statement");
  }
}

BinaryExp Parser::parseBinaryExp()
{
  if (atEnd())
    throw std::runtime_error("Expected condition after WHERE");

  const Token &left = advance();
  if (left.type != TokenType::Identifier)
    throw std::runtime_error("Expected column in WHERE");

  const Token &op = advance();
  if (op.type != TokenType::Equal &&
      op.type != TokenType::Less &&
      op.type != TokenType::Greater)
    throw std::runtime_error("Invalid operator in WHERE");

  if (atEnd())
    throw std::runtime_error("Expected RHS in WHERE");

  const Token &right = advance();

  std::variant<IntLiteral, StringLiteral, Column> rhs;

  if (right.type == TokenType::Integer)
  {
    rhs = IntLiteral{std::stoi(right.lexeme)};
  }
  else if (right.type == TokenType::String)
  {
    rhs = StringLiteral{right.lexeme};
  }
  else if (right.type == TokenType::Identifier)
  {
    rhs = Column{right.lexeme};
  }
  else
  {
    throw std::runtime_error("Invalid RHS in WHERE");
  }
  return BinaryExp{
      .left = Column{left.lexeme},
      .op = op.lexeme,
      .right = rhs};
}

SelectStatement Parser::parseSelect()
{
  SelectStatement stmt;
  advance();

  while (true)
  {
    const Token &token = peek();

    if (token.type == TokenType::Star)
    {
      stmt.columns.push_back("*");
      advance();
      if (peek().type == TokenType::Comma)
      {
        throw std::runtime_error("Unexpected comma after *");
      }
    }
    else if (token.type == TokenType::Identifier)
    {
      stmt.columns.push_back(token.lexeme);
      advance();
    }
    else
    {
      throw std::runtime_error("Expected column name");
    }

    if (peek().type == TokenType::Comma)
    {
      advance(); // consume comma
    }
    else
    {
      break;
    }
  }
  if (!match(TokenType::From))
    throw std::runtime_error("Expected FROM");
  // table name
  if (peek().type != TokenType::Identifier)
    throw std::runtime_error("Expected table name");
  stmt.table = peek().lexeme;
  advance();

  if (match(TokenType::Where))
  {
    stmt.where = parseBinaryExp();
  }

  return stmt;
}

InsertStatement Parser::parseInsert()
{
  // INSERT INTO student VALUES (1, 'abc')

  InsertStatement stmt;

  advance(); // consume INSERT

  if (!match(TokenType::Into))
    throw std::runtime_error("Expected INTO");

  if (atEnd() || peek().type != TokenType::Identifier)
    throw std::runtime_error("Expected table name");

  stmt.table = advance().lexeme;

  if (!match(TokenType::Values))
    throw std::runtime_error("Expected VALUES");

  if (!match(TokenType::LeftParen))
    throw std::runtime_error("Expected '(' after VALUES");

  while (true)
  {
    if (atEnd())
      throw std::runtime_error("Unexpected end in VALUES list");

    const Token &t = peek();

    // parse value
    if (t.type == TokenType::Integer)
    {
      stmt.values.push_back(IntLiteral{std::stoi(t.lexeme)});
      advance();
    }
    else if (t.type == TokenType::String)
    {
      stmt.values.push_back(StringLiteral{t.lexeme});
      advance();
    }
    else
    {
      throw std::runtime_error("Invalid value in INSERT");
    }

    if (peek().type == TokenType::Comma)
    {
      advance();
      continue;
    }

    if (peek().type == TokenType::RightParen)
    {
      advance();
      break;
    }

    throw std::runtime_error(
        "Expected ',' or ')' in VALUES list");
  }

  return stmt;
}

DeleteStatement Parser::parseDelete()
{
  // DELETE FROM student WHERE id = 10

  DeleteStatement stmt;

  advance(); // DELETE

  if (!match(TokenType::From))
    throw std::runtime_error("Expected FROM after DELETE");

  if (atEnd() || peek().type != TokenType::Identifier)
    throw std::runtime_error("Expected table name");

  stmt.table = peek().lexeme;
  advance();

  // optional WHERE
  if (match(TokenType::Where))
  {
    stmt.where = parseBinaryExp();
  }

  return stmt;
}

CreateTableStatement Parser::parseCreateTable()
{
  // CREATE TABLE student (id INT, name TEXT)

  CreateTableStatement stmt;

  advance(); // CREATE

  if (!match(TokenType::Table))
    throw std::runtime_error("Expected TABLE after CREATE");

  if (atEnd() || peek().type != TokenType::Identifier)
    throw std::runtime_error("Expected table name");

  stmt.table = peek().lexeme;
  advance();

  if (!match(TokenType::LeftParen))
    throw std::runtime_error("Expected '(' after table name");

  while (true)
  {
    if (atEnd())
      throw std::runtime_error("Unexpected end in column list");

    // column name
    if (peek().type != TokenType::Identifier)
      throw std::runtime_error("Expected column name");

    ColumnDef col;
    col.name = peek().lexeme;
    advance();

    // column type
    if (atEnd() || peek().type != TokenType::Identifier)
      throw std::runtime_error("Expected column type");

    col.type = peek().lexeme;
    advance();

    stmt.columns.push_back(col);

    // comma or end
    if (peek().type == TokenType::Comma)
    {
      advance();
      continue;
    }

    if (peek().type == TokenType::RightParen)
    {
      advance();
      break;
    }

    throw std::runtime_error("Expected ',' or ')' in column list");
  }

  return stmt;
}