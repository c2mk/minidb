#include "persistence/persistenceManager.hpp"

#include "persistence/binaryWriter.hpp"
#include "persistence/binaryReader.hpp"

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