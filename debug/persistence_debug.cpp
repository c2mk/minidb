#include "persistence/persistenceManager.hpp"
#include "persistence/binaryReader.hpp"
#include "persistence/binaryWriter.hpp"

#include <iostream>

int main()
{
  try
  {
    Schema originalSchema = {
        ColumnSchema{
            .name = "id",
            .type = DataType::Int,
            .isPrimaryKey = true,
            .isNotNull = true},
        ColumnSchema{
            .name = "name",
            .type = DataType::Text,
            .isPrimaryKey = false,
            .isNotNull = true},
        ColumnSchema{
            .name = "age",
            .type = DataType::Int,
            .isPrimaryKey = false,
            .isNotNull = false}};

    std::cout << "Original schema:\n";

    for (const auto &column : originalSchema)
    {
      std::cout
          << column.name
          << " "
          << static_cast<int>(column.type)
          << " PK="
          << column.isPrimaryKey
          << " NN="
          << column.isNotNull
          << '\n';
    }

    // WRITE
    // writeSchema is private currently, make it public to test

    PersistenceManager pm;
    {
      BinaryWriter writer("schema_test.bin");
      pm.writeSchema(writer, originalSchema);
    }

    std::cout << "\nWRITE SUCCESS\n";

    // READ
    // readSchema is private currently, make it public to test

    Schema loaded;
    {
      BinaryReader reader("schema_test.bin");
      loaded = pm.readSchema(reader);
    }
    for (const auto &column : loaded)
    {
      std::cout
          << column.name
          << " "
          << static_cast<int>(column.type)
          << " PK="
          << column.isPrimaryKey
          << " NN="
          << column.isNotNull
          << '\n';
    }
    std::cout << "\nREAD SUCCESS\n";
  }
  catch (const std::exception &e)
  {
    std::cout
        << "ERROR: "
        << e.what()
        << '\n';
  }
}