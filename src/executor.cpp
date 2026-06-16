#include "storage/types.hpp"
#include "parser/ast.hpp"

namespace
{
  // anonymous namespace
  // Because these functions are implementation details of this translation unit.
  // Without the anonymous namespace, they have external linkage.

  std::optional<size_t> findColumnIndex(const Schema &schema, const std::string &columnName)
  {
    for (size_t i = 0; i < schema.size(); ++i)
    {
      if (schema[i].name == columnName)
      {
        return i;
      }
    }
    return std::nullopt;
  }

  bool compareValues(const Value &lhs, const Value &rhs, const std::string &op)
  {
    // Every type inside the variant gets a number based on its position.
    if (lhs.index() != rhs.index())
      return false;

    if (std::holds_alternative<int>(lhs))
    {
      int left = std::get<int>(lhs);
      int right = std::get<int>(rhs);

      if (op == "=")
        return left == right;
      else if (op == "<")
        return left < right;
      else if (op == ">")
        return left > right;

      return false;
    }
    const std::string &left =
        std::get<std::string>(lhs);

    const std::string &right =
        std::get<std::string>(rhs);

    if (op == "=")
      return left == right;
    else if (op == "<")
      return left < right;
    else if (op == ">")
      return left > right;

    return false;
  }
}

bool evaluateWhere(const Row &row, const Schema &schema, const BinaryExp &expr)
{
  auto leftIndex = findColumnIndex(schema, expr.left.name);
  if (!leftIndex)
    return false;

  const Value &leftValue = row[leftIndex.value()];

  Value rightValue;

  if (std::holds_alternative<IntLiteral>(expr.right))
  {
    rightValue = std::get<IntLiteral>(expr.right).value;
  }
  else if (std::holds_alternative<StringLiteral>(expr.right))
  {
    rightValue =
        std::get<StringLiteral>(expr.right).value;
  }
  else if (std::holds_alternative<Column>(expr.right))
  {
    const auto &columnName = std::get<Column>(expr.right);
    auto rightIndex = findColumnIndex(schema, columnName.name);

    if (!rightIndex)
      return false;

    rightValue = row[rightIndex.value()];
  }
  return compareValues(leftValue, rightValue, expr.op);
}