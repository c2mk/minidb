#pragma once

#include "catalog.hpp"
#include "persistence/binaryWriter.hpp"
#include "persistence/binaryReader.hpp"
#include "storage/table.hpp"

#include <string>

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

    void writeValue(
        BinaryWriter &writer,
        const Value &value);

    Value readValue(
        BinaryReader &reader);

    void writeRow(
        BinaryWriter &writer,
        const Row &row);

    Row readRow(
        BinaryReader &reader);

    void writeTableHeap(
        BinaryWriter &writer,
        const TableHeap &table);

    TableHeap readTableHeap(
        BinaryReader &reader);

    void writeCatalog(
        BinaryWriter &writer,
        const Catalog &catalog);

    Catalog readCatalog(
        BinaryReader &reader);
};