#pragma once
#include "storage/table.hpp"
#include <unordered_map>

class Catalog
{
public:
  void createTable(const std::string &name, Schema schema);

  // Two versions of getTable, this is const overloading. C++ picks between them based on context. If you call getTable on a non-const Catalog, you get back a mutable TableHeap& and can call insertRow on it. If you call it on a const Catalog&, you get back a const TableHeap& and can only read
  TableHeap &getTable(const std::string &name);
  const TableHeap &getTable(const std::string &name) const;

  bool tableExists(const std::string &name) const;

private:
  std::unordered_map<std::string, TableHeap> tables_;
};