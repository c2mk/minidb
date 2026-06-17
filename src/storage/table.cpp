#include "storage/table.hpp"
#include <stdexcept>
// why move
// std::move(schema) in the constructor — when you pass Schema schema by value into the constructor, you already have a local copy. std::move transfers ownership of that copy's internal memory into schema_ instead of copying it again. Without it, you'd copy the vector twice for no reason. This is a standard pattern for constructors that take ownership of data.
TableHeap::TableHeap(Schema schema) : schema_(std::move(schema)) {}

void TableHeap::insertRow(Row row)
{
  if (row.size() != schema_.size())
    throw std::runtime_error("Column count mismatch");
  rows_.push_back(std::move(row));
}

const std::vector<Row> &TableHeap::rows() const
{
  return rows_;
}

const Schema &TableHeap::schema() const
{
  return schema_;
};

size_t TableHeap::deleteRows(const std::vector<size_t> &indices)
{
  // assumes sorted indices
  // important to erase in descending order
  for (auto it = indices.rbegin(); it != indices.rend(); ++it)
  {
    rows_.erase(rows_.begin() + *it);
  }
  return indices.size();
}