#include "parser/parser.hpp"

#include <vector>
#include <stdexcept>
#include <iostream>

// std::move makes it O(1), else copying would be costly
Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

Token Parser::peek() const
{
  if (pos_ >= tokens_.size())
    throw std::runtime_error("Unexpected end of input");

  return tokens_[pos_];
}

Token Parser::advance()
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
  if (atEnd())
    return false;

  if (advance().type == type)
  {
    return true;
  }
  return false;
}

Statement Parser::parse()
{
  return parseStatement();
}

Statement Parser::parseStatement()
{
  Token token = peek();
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

SelectStatement Parser::parseSelect()
{
  // for (auto token : tokens_)
  //   std::cout << token.lexeme << '\n';
  SelectStatement stmt;
  advance();

  while (true)
  {
    Token token = peek();
    if (token.type == TokenType::Identifier)
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
    std::cout << "hello" << '\n';
    if (atEnd())
      throw std::runtime_error("Expected condition after WHERE");

    Token left = advance();
    if (left.type != TokenType::Identifier)
      throw std::runtime_error("Expected column in WHERE");

    Token op = advance();
    if (op.type != TokenType::Equal &&
        op.type != TokenType::Less &&
        op.type != TokenType::Greater)
      throw std::runtime_error("Invalid operator in WHERE");

    if (atEnd())
      throw std::runtime_error("Expected RHS in WHERE");

    Token right = advance();
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

    stmt.where = BinaryExp{
        Column{left.lexeme},
        op.lexeme,
        rhs};
    // std::cout << stmt.table << '\n';
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

    Token t = peek();

    if (t.type == TokenType::RightParen)
    {
      advance();
      break;
    }

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

    // comma handling
    // safe for last token since last token should be right parantheses
    if (peek().type == TokenType::Comma)
      advance();
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
    stmt.hasWhere = true;

    if (atEnd())
      throw std::runtime_error("Expected condition after WHERE");

    Token left = advance();
    if (left.type != TokenType::Identifier)
      throw std::runtime_error("Expected column in WHERE");

    Token op = advance();
    if (op.type != TokenType::Equal &&
        op.type != TokenType::Less &&
        op.type != TokenType::Greater)
    {
      throw std::runtime_error("Invalid operator in WHERE");
    }

    if (atEnd())
      throw std::runtime_error("Expected RHS in WHERE");

    Token right = advance();

    stmt.where = BinaryExp{
        Column{left.lexeme},
        op.lexeme,
        StringLiteral{right.lexeme} // same simplification as SELECT
    };
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

    if (peek().type == TokenType::RightParen)
    {
      advance();
      break;
    }

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
    }
  }

  return stmt;
}