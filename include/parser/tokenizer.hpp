#pragma once

#include "token.hpp"

#include <string>
#include <string_view>
#include <vector>

void printTokens(const std::vector<Token> &tokens);

class Tokenizer
{
public:
  // why explicit
  explicit Tokenizer(std::string_view input);
  std::vector<Token> tokenize();

private:
  // why these
  char peek() const;
  char advance();
  bool atEnd() const;

  void skipWhitespace();

  Token makeIdentifierOrKeyword();
  Token makeSymbol();
  Token makeNumber();
  Token makeString();

private:
  std::string_view input_;
  std::size_t pos_;
};