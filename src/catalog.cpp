#include "catalog.hpp"
#include <stdexcept>

void Catalog::createTable(const std::string &name, Schema schema)
{
  if (tableExists(name))
    throw std::runtime_error("Table '" + name + "' already exists");

  // second one first default constructs a TableHeap and then assignes TableHeap(std::move(schema))
  tables_.emplace(name, TableHeap(std::move(schema)));
  // tables_[name] = TableHeap(std::move(schema));
}

// The two getTable overloads have identical bodies and it feels wrong. This problem has a name: const/non-const overload duplication. The canonical solution is the Scott Meyers pattern
TableHeap &Catalog::getTable(const std::string &name)
{
  auto it = tables_.find(name);
  if (it == tables_.end())
    throw std::runtime_error("Table '" + name + "' does not exist.");
  return it->second;
}

const TableHeap &Catalog::getTable(const std::string &name) const
{
  auto it = tables_.find(name);
  if (it == tables_.end())
    throw std::runtime_error("Table '" + name + "' does not exist.");
  return it->second;
}

bool Catalog::tableExists(const std::string &name) const
{
  return tables_.contains(name);
}