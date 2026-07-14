#pragma once

#include "catalog.hpp"

#include <string>

// forward declaration instead of including
class BinaryWriter;
class BinaryReader;

class PersistenceManager
{
public:
  void saveCatalog(const Catalog &catalog,
                   const std::string &filename);

  Catalog loadCatalog(const std::string &filename);

  // private:
  void writeDataType(
      BinaryWriter &writer,
      DataType type);

  DataType readDataType(
      BinaryReader &reader);

  void writeSchema(
      BinaryWriter &writer,
      const Schema &schema);

  Schema readSchema(
      BinaryReader &reader);

  void writeColumnSchema(
      BinaryWriter &writer,
      const ColumnSchema &column);

  ColumnSchema readColumnSchema(
      BinaryReader &reader);
};