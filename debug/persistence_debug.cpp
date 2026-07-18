#include "persistence/persistenceManager.hpp"
#include "persistence/binaryReader.hpp"
#include "persistence/binaryWriter.hpp"

#include <iostream>
#include <cassert>

void printValue(const Value &value)
{
  if (std::holds_alternative<int>(value))
  {
    std::cout << std::get<int>(value);
  }
  else if (std::holds_alternative<std::string>(value))
  {
    std::cout << '"' << std::get<std::string>(value) << '"';
  }
  else
  {
    std::cout << "NULL";
  }
}

void printTable(const TableHeap &table)
{
  const auto &schema = table.schema();

  std::cout << "\nSCHEMA\n";
  for (const auto &column : schema)
  {
    std::cout
        << column.name
        << " type="
        << static_cast<int>(column.type)
        << " PK="
        << column.isPrimaryKey
        << " NN="
        << column.isNotNull
        << '\n';
  }

  std::cout << "\nROWS\n";

  for (const auto &row : table.rows())
  {
    for (const auto &value : row)
    {
      printValue(value);
      std::cout << " | ";
    }
    std::cout << '\n';
  }
}

bool compareTables(
    const TableHeap &a,
    const TableHeap &b)
{
  if (a.schema().size() != b.schema().size())
    return false;

  for (size_t i = 0; i < a.schema().size(); i++)
  {
    const auto &ca = a.schema()[i];
    const auto &cb = b.schema()[i];

    if (ca.name != cb.name ||
        ca.type != cb.type ||
        ca.isPrimaryKey != cb.isPrimaryKey ||
        ca.isNotNull != cb.isNotNull)
    {
      return false;
    }
  }

  if (a.rows().size() != b.rows().size())
    return false;

  for (size_t i = 0; i < a.rows().size(); i++)
  {
    const auto &ra = a.rows()[i];
    const auto &rb = b.rows()[i];

    if (ra.size() != rb.size())
      return false;

    for (size_t j = 0; j < ra.size(); j++)
    {
      if (ra[j] != rb[j])
        return false;
    }
  }

  return true;
}

int main()
{
  try
  {
    PersistenceManager pm;

    Schema schema =
        {
            {.name = "id",
             .type = DataType::Int,
             .isPrimaryKey = true,
             .isNotNull = true},

            {.name = "name",
             .type = DataType::Text,
             .isPrimaryKey = false,
             .isNotNull = true},

            {.name = "age",
             .type = DataType::Int,
             .isPrimaryKey = false,
             .isNotNull = false}};

    TableHeap original(schema);

    original.insertRow(
        {1,
         std::string("Alice"),
         20});

    original.insertRow(
        {2,
         std::string("Bob"),
         std::monostate{}});

    original.insertRow(
        {-5,
         std::string(""),
         -100});

    std::cout << "ORIGINAL TABLE";
    printTable(original);

    // WRITE
    {
      BinaryWriter writer("full_table_test.bin");
      pm.writeTableHeap(writer, original);
    }

    std::cout << "\nWRITE SUCCESS\n";

    // READ
    TableHeap loaded =
        [&]()
    {
      BinaryReader reader("full_table_test.bin");
      return pm.readTableHeap(reader);
    }();

    std::cout << "\nLOADED TABLE";
    printTable(loaded);

    if (compareTables(original, loaded))
    {
      std::cout
          << "\n\nPASS: Tables identical after serialization\n";
    }
    else
    {
      std::cout
          << "\n\nFAIL: Tables differ\n";
    }
  }
  catch (const std::exception &e)
  {
    std::cout
        << "ERROR: "
        << e.what()
        << '\n';
  }
}