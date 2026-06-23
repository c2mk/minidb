#include "storage/types.hpp"
#include "parser/ast.hpp"
#include "executor.hpp"
#include <iostream>

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

  std::optional<size_t> findPrimaryKeyColumn(const Schema &schema)
  {
    for (size_t i = 0; i < schema.size(); ++i)
    {
      if (schema[i].isPrimaryKey)
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
      throw std::runtime_error(
          "Type mismatch in comparison");

    if (std::holds_alternative<int>(lhs))
    {
      int left = std::get<int>(lhs);
      int right = std::get<int>(rhs);

      if (op == "=")
        return left == right;
      if (op == "<")
        return left < right;
      if (op == ">")
        return left > right;

      throw std::runtime_error(
          "Unsupported operator: " + op);
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

    throw std::runtime_error(
        "Unsupported operator: " + op);
  }

  void validateWhereColums(const Schema &schema, const BinaryExp &expr)
  {
    // Ensures all column references exist before row scanning.
    // double lookups for leftIndex feels wrong tho, one here one in evaluateWhere
    auto leftIndex =
        findColumnIndex(schema, expr.left.name);

    if (!leftIndex)
    {
      throw std::runtime_error(
          "Unknown column: " + expr.left.name);
    }

    if (std::holds_alternative<Column>(expr.right))
    {
      const auto &column =
          std::get<Column>(expr.right);

      auto rightIndex =
          findColumnIndex(schema, column.name);

      if (!rightIndex)
      {
        throw std::runtime_error(
            "Unknown column: " + column.name);
      }
    }
  }

  bool evaluateWhere(const Row &row, const Schema &schema, const BinaryExp &expr)
  {
    auto leftIndex = findColumnIndex(schema, expr.left.name);

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

      rightValue = row[rightIndex.value()];
    }
    return compareValues(leftValue, rightValue, expr.op);
  }

  void validateColumnCount(const Schema &schema, const Row &row)
  {
    if (schema.size() != row.size())
      throw std::runtime_error("Column count mismatch.");
  }

  void validateInsertTypes(const Schema &schema, const Row &row)
  {
    for (size_t i = 0; i < schema.size(); ++i)
    {
      const auto &column = schema[i];
      const auto &value = row[i];

      if (std::holds_alternative<std::monostate>(value))
      {
        continue;
      }

      switch (column.type)
      {
      case DataType::Int:
        if (!std::holds_alternative<int>(value))
          throw std::runtime_error("Type mismatch for column '" + column.name + "'");
        break;
      case DataType::Text:
        if (!std::holds_alternative<std::string>(value))
          throw std::runtime_error("Type mismatch for column '" + column.name + "'");
      }
    }
  }

  void validateNotNull(const Schema &schema, const Row &row)
  {
    for (size_t i = 0; i < schema.size(); i++)
    {
      const auto &col = schema[i];
      const auto &val = row[i];
      if (col.isNotNull && std::holds_alternative<std::monostate>(val))
      {
        throw std::runtime_error("Column '" + col.name + "' cannot be NULL");
      }
    }
  }

  void validatePrimaryKeyUniqueness(const TableHeap &table, const Row &row)
  {
    const Schema &schema = table.schema();

    const auto pkIdx = findPrimaryKeyColumn(schema);

    if (!pkIdx)
    {
      return;
    }
    const auto &rows = table.rows();
    for (const auto &existingRow : rows)
    {
      if (compareValues(existingRow[pkIdx.value()], row[pkIdx.value()], "="))
      {
        throw std::runtime_error("Duplicate primary key value");
      }
    }
  }
}

// Remember: Catalog& catalog_ must be initialized in the initializer list because references cannot be reseated.
Executor::Executor(Catalog &catalog) : catalog_(catalog) {}

QueryResult Executor::execute(const Statement &stmt)
{
  // throw deep catch high
  try
  {
    if (std::holds_alternative<CreateTableStatement>(stmt))
    {
      return executeCreate(std::get<CreateTableStatement>(stmt));
    }
    if (std::holds_alternative<InsertStatement>(stmt))
    {
      return executeInsert(
          std::get<InsertStatement>(stmt));
    }

    if (std::holds_alternative<SelectStatement>(stmt))
    {
      return executeSelect(
          std::get<SelectStatement>(stmt));
    }

    if (std::holds_alternative<DeleteStatement>(stmt))
    {
      return executeDelete(
          std::get<DeleteStatement>(stmt));
    }
    return QueryResult::error(
        "Unknown statement type");
  }
  catch (const std::exception &e)
  {
    return QueryResult::error(e.what());
  }
}

QueryResult Executor::executeCreate(
    const CreateTableStatement &stmt)
{
  Schema schema;

  size_t primaryKeyCount = 0;

  for (const auto &column : stmt.columns)
  {
    primaryKeyCount += column.isPrimaryKey;

    schema.push_back(ColumnSchema{
        .name = column.name,
        .type = parseDataType(column.type),
        .isPrimaryKey = column.isPrimaryKey,
        .isNotNull = column.isNotNull});
  }

  if (primaryKeyCount > 1)
  {
    return QueryResult::error(
        "Only one PRIMARY KEY column is supported");
  }

  catalog_.createTable(
      stmt.table,
      std::move(schema));

  return QueryResult::ok("Table Created.");
}

QueryResult Executor::executeInsert(const InsertStatement &stmt)
{
  TableHeap &table = catalog_.getTable(stmt.table);

  Row row;
  for (const auto &value : stmt.values)
  {
    if (std::holds_alternative<IntLiteral>(value))
    {
      row.push_back(std::get<IntLiteral>(value).value);
      // maybe shouldve done using IntLiteral = int value?
    }
    else if (std::holds_alternative<StringLiteral>(value))
    {
      row.push_back(std::get<StringLiteral>(value).value);
    }
    else if (std::holds_alternative<NullLiteral>(value))
    {
      row.push_back(std::monostate{});
    }
  }

  const Schema &schema = table.schema();
  validateColumnCount(schema, row);
  validateInsertTypes(schema, row);
  validateNotNull(schema, row);
  validatePrimaryKeyUniqueness(table, row);

  table.insertRow(std::move(row));

  return QueryResult::ok("1 row inserted");
}

QueryResult Executor::executeSelect(const SelectStatement &stmt)
{
  TableHeap &table = catalog_.getTable(stmt.table);

  const Schema &schema = table.schema();
  // why get schema this way, encapsulation?

  std::vector<size_t> projectedIndices;
  std::vector<std::string> columnNames;

  if (stmt.columns.size() == 1 && stmt.columns[0] == "*")
  {
    // yet to implement case of "*" in parser
    for (size_t i = 0; i < schema.size(); i++)
    {
      projectedIndices.push_back(i);
      columnNames.push_back(schema[i].name);
    }
  }
  else
  {
    for (auto const &columnName : stmt.columns)
    {
      auto index = findColumnIndex(schema, columnName);

      if (!index)
      {
        return QueryResult::error("Unknown column: " + columnName);
      }

      projectedIndices.push_back(index.value());
      columnNames.push_back(columnName);
    }
  }

  std::vector<Row> resultRows;
  if (stmt.where)
    validateWhereColums(schema, stmt.where.value());
  for (auto const &row : table.rows())
  {
    bool keep = true;

    if (stmt.where)
    {
      keep = evaluateWhere(row, schema, stmt.where.value());
    }

    if (!keep)
      continue;

    Row projectedRow;

    for (size_t index : projectedIndices)
    {
      projectedRow.push_back(row[index]);
    }

    resultRows.push_back(std::move(projectedRow));
  }
  return QueryResult::table(std::move(columnNames), std::move(resultRows));
}

QueryResult Executor::executeDelete(const DeleteStatement &stmt)
{
  TableHeap &table = catalog_.getTable(stmt.table);

  const Schema &schema = table.schema();

  std::vector<std::size_t> rowsToDelete;

  if (stmt.where)
    validateWhereColums(schema, stmt.where.value());

  for (size_t i = 0; i < table.rows().size(); ++i)
  {
    bool shouldDelete = true;

    const Row &row = table.rows()[i];

    if (stmt.where)
    {
      shouldDelete = evaluateWhere(row, schema, stmt.where.value());
    }

    if (shouldDelete)
    {
      rowsToDelete.push_back(i);
    }
  }

  size_t deleted = table.deleteRows(rowsToDelete);

  return QueryResult::ok(std::to_string(deleted) + " rows deleted.");
}
