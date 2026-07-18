#include "persistence/persistenceManager.hpp"

void PersistenceManager::writeDataType(BinaryWriter &writer, DataType type)
{
  writer.writeEnum(type);
}

DataType PersistenceManager::readDataType(BinaryReader &reader)
{
  return reader.readEnum<DataType>();
}

void PersistenceManager::writeColumnSchema(BinaryWriter &writer, const ColumnSchema &column)
{
  // forward declaration is not enough here
  writer.writeString(column.name);
  writeDataType(writer, column.type);
  writer.writeBool(column.isPrimaryKey);
  writer.writeBool(column.isNotNull);
}

ColumnSchema PersistenceManager::readColumnSchema(BinaryReader &reader)
{
  ColumnSchema column;

  // can also do aggregate initialization
  column.name = reader.readString();

  column.type = readDataType(reader);

  column.isPrimaryKey = reader.readBool();

  column.isNotNull = reader.readBool();

  return column;
}

void PersistenceManager::writeSchema(BinaryWriter &writer, const Schema &schema)
{
  writer.writeUInt32(static_cast<uint32_t>(schema.size()));

  for (const auto &column : schema)
  {
    writeColumnSchema(writer, column);
  }
}

Schema PersistenceManager::readSchema(BinaryReader &reader)
{
  uint32_t columnCount = reader.readUInt32();

  Schema schema;
  // When you add elements to a dynamic container (e.g., using push_back), it will automatically reallocate memory and copy elements over when it runs out of space. Calling reserve(n) forces the container to allocate a contiguous block of memory large enough to hold at least n elements upfront. This eliminates the CPU overhead of multiple background reallocations.
  schema.reserve(columnCount);

  for (uint32_t i = 0; i < columnCount; i++)
  {
    schema.push_back(readColumnSchema(reader));
  }

  return schema;
}

// notice that currently we are serializing int using uint32_t
// but int isnt guaranteeed to be 32 bits, should refactor and do something like
// using Int32 = int32_t
void PersistenceManager::writeValue(BinaryWriter &writer, const Value &value)
{
  if (std::holds_alternative<std::monostate>(value))
  {
    writer.writeEnum(ValueType::Null);
  }
  else if (std::holds_alternative<int>(value))
  {
    writer.writeEnum(ValueType::Int);

    writer.writeInt32(static_cast<int32_t>(std::get<int>(value)));
  }
  else if (std::holds_alternative<std::string>(value))
  {
    writer.writeEnum(ValueType::Text);

    writer.writeString(std::get<std::string>(value));
  }
}

Value PersistenceManager::readValue(BinaryReader &reader)
{
  ValueType type = reader.readEnum<ValueType>();

  switch (type)
  {
  case ValueType::Null:
    return std::monostate{};

  case ValueType::Int:
  {
    int value = static_cast<int>(reader.readInt32());

    return value;
  }

  case ValueType::Text:
  {
    return reader.readString();
  }

  default:
    throw std::runtime_error("Unknown value type");
  }
}

void PersistenceManager::writeRow(
    BinaryWriter &writer,
    const Row &row)
{
  writer.writeUInt32(static_cast<uint32_t>(row.size()));

  for (const auto &value : row)
  {
    writeValue(writer, value);
  }
}

Row PersistenceManager::readRow(BinaryReader &reader)
{
  uint32_t valueCount = reader.readUInt32();

  Row row;

  row.reserve(valueCount);

  for (uint32_t i = 0; i < valueCount; i++)
  {
    row.push_back(readValue(reader));
  }

  return row;
}

void PersistenceManager::writeTableHeap(BinaryWriter &writer, const TableHeap &table)
{
  writeSchema(writer, table.schema());

  const auto &rows = table.rows();

  writer.writeUInt32(static_cast<uint32_t>(rows.size()));

  for (const auto &row : rows)
    writeRow(writer, row);
}

TableHeap PersistenceManager::readTableHeap(BinaryReader &reader)
{
  Schema schema = readSchema(reader);

  TableHeap table(std::move(schema));

  uint32_t rowCount = reader.readUInt32();

  for (uint32_t i = 0; i < rowCount; i++)
  {
    Row row = readRow(reader);
    table.insertRow(std::move(row));
  }

  return table;
  // Returning by value:
  // return table;
  // uses move semantics.
  // Modern C++ handles this efficiently (NRVO/move).
}

void PersistenceManager::writeCatalog(BinaryWriter &writer, const Catalog &catalog)
{
  const auto tableNames = catalog.tableNames();

  writer.writeUInt32(
      static_cast<uint32_t>(tableNames.size()));

  for (const auto &name : tableNames)
  {
    writer.writeString(name);

    writeTableHeap(
        writer,
        catalog.getTable(name));
  }
}

Catalog PersistenceManager::readCatalog(BinaryReader &reader)
{
  Catalog catalog;

  uint32_t tableCount = reader.readUInt32();

  for (uint32_t i = 0; i < tableCount; i++)
  {
    // ugly double work
    // add something like Catalog::addTable in the future

    std::string name = reader.readString();

    TableHeap table = readTableHeap(reader);

    catalog.createTable(name, table.schema());

    TableHeap &stored = catalog.getTable(name);

    for (const auto &row : table.rows())
    {
      stored.insertRow(row);
    }
  }

  return catalog;
}