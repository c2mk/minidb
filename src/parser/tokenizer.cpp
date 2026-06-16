#include "parser/tokenizer.hpp"

#include <cctype>
#include <iostream>
#include <unordered_map>
#include <algorithm>

// why static?
static const std::unordered_map<std::string, TokenType> keywords = {
    {"select", TokenType::Select},
    {"insert", TokenType::Insert},
    {"create", TokenType::Create},
    {"delete", TokenType::Delete},
    {"table", TokenType::Table},
    {"from", TokenType::From},
    {"where", TokenType::Where},
    {"into", TokenType::Into},
    {"values", TokenType::Values},
    {"and", TokenType::And},
    {"or", TokenType::Or}};

static std::string toLower(std::string s)
{
  // s intentionally not passed by reference
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

Tokenizer::Tokenizer(std::string_view input) : input_(input), pos_(0) {}

char Tokenizer::peek() const
{
  if (pos_ < input_.size())
    return input_[pos_];
  return '\0';
  // will do this
  // throw std::runtime_error("peek() out of bounds");
}

char Tokenizer::advance()
{
  if (pos_ < input_.size())
  {
    return input_[pos_++];
  }
  return '\0';
  // throw std::runtime_error("peek() out of bounds");
}

void Tokenizer::skipWhitespace()
{
  // why static cast, are there other types of casts
  while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_])))
  {
    pos_++;
  }
}

bool Tokenizer::atEnd() const
{
  return pos_ >= input_.size();
}

std::vector<Token> Tokenizer::tokenize()
{
  std::vector<Token> tokens;

  while (true)
  {
    skipWhitespace();

    if (atEnd())
      break;

    unsigned char ch = peek();

    if (std::isalpha(ch))
      tokens.push_back(makeIdentifierOrKeyword());

    else if (std::isdigit(ch))
      tokens.push_back(makeNumber());

    else if (ch == '\'')
      tokens.push_back(makeString());

    else
      tokens.push_back(makeSymbol());
  }
  // use designated initializers
  tokens.push_back(Token{.type = TokenType::EndOfFile, .lexeme = ""});
  return tokens;
}

Token Tokenizer::makeNumber()
{
  std::string lexeme = "";
  while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek())))
  {
    unsigned char ch = advance();
    if (std::isdigit(ch))
    {
      lexeme += ch;
    }
  }
  return Token{
      .type = TokenType::Integer,
      .lexeme = lexeme};
}

Token Tokenizer::makeString()
{
  // assumption:'anything here until next single quote'
  std::string lexeme;

  // consume opening quote '
  advance();

  while (!atEnd())
  {
    char c = static_cast<unsigned char>(peek());

    // closing quote
    if (c == '\'')
    {
      advance(); // consume closing quote
      break;
    }

    lexeme += advance();
  }

  return Token{
      .type = TokenType::String,
      .lexeme = lexeme};
}

Token Tokenizer::makeIdentifierOrKeyword()
{
  std::string lexeme;

  while (!atEnd())
  {
    char c = static_cast<unsigned char>(peek());

    if (std::isalnum(static_cast<unsigned char>(c)) || c == '_')
    {
      lexeme += advance();
    }
    else
    {
      break;
    }
  }

  // normalize for keyword check
  std::string lowered = toLower(lexeme);

  auto it = keywords.find(lowered);

  if (it != keywords.end())
  {
    return Token{
        .type = it->second,
        .lexeme = lexeme};
  }

  return Token{
      .type = TokenType::Identifier,
      .lexeme = lexeme};
}

Token Tokenizer::makeSymbol()
{
  char c = static_cast<unsigned char>(advance());
  switch (c)
  {
  case ',':
    return Token{
        .type = TokenType::Comma,
        .lexeme = ","};

  case ';':
    return Token{
        .type = TokenType::Semicolon,
        .lexeme = ";"};

  case '(':
    return Token{
        .type = TokenType::LeftParen,
        .lexeme = "("};

  case ')':
    return Token{
        .type = TokenType::RightParen,
        .lexeme = ")"};

  case '*':
    return Token{
        .type = TokenType::Star,
        .lexeme = "*"};

  case '=':
    return Token{
        .type = TokenType::Equal,
        .lexeme = "="};

  default:
    return Token{
        .type = TokenType::Unknown,
        .lexeme = std::string(1, c)};
  }
}