#include "command.hpp"
#include <string>

Command classifyCommand(const std::string &input)
{
  if (input == "help")
  {
    return Command{
        .type = CommandType::Help,
        .raw = input};
    // desginated initialization needs aggregates
  }
  if (input == "quit")
  {
    return Command{
        .type = CommandType::Quit,
        .raw = input};
  }
  if (input.starts_with("CREATE TABLE"))
  {
    return Command{
        .type = CommandType::CreateTable,
        .raw = input};
  }
  if (input.starts_with("INSERT INTO"))
  {
    return Command{
        .type = CommandType::Insert,
        .raw = input};
  }
  if (input.starts_with("SELECT"))
  {
    return Command{
        .type = CommandType::Select,
        .raw = input};
  }
  if (input.starts_with("DELETE"))
  {
    return Command{
        .type = CommandType::Delete,
        .raw = input};
  }
  return Command{
      .type = CommandType::Unknown,
      .raw = input};
}