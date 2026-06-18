#pragma once

#include <string>
#include <string_view>

enum class TokenType
{
  // Keywords
  Select,
  Insert,
  Create,
  Delete,
  Table,
  Into,
  From,
  Where,
  Values,
  And,
  Or,

  // Identifiers and Literals
  Identifier,
  Integer,
  String,

  // punctuation / operators
  Comma,
  Semicolon,
  LeftParen,
  RightParen,
  Star,
  Equal,
  Less,
  Greater,
  LessEqual,
  GreaterEqual,
  NotEqual,

  // EndOfFile,
  Unknown
};

struct Token
{
  TokenType type;
  std::string lexeme;
};
