#include <storage/table.hpp>

// why move
// std::move(schema) in the constructor — when you pass Schema schema by value into the constructor, you already have a local copy. std::move transfers ownership of that copy's internal memory into schema_ instead of copying it again. Without it, you'd copy the vector twice for no reason. This is a standard pattern for constructors that take ownership of data.
TableHeap::TableHeap(Schema schema) : schema_(std::move(schema)) {}

void TableHeap::insertRow(const Row &row)
{
  rows_.push_back(row);
}

const std::vector<Row> &TableHeap::rows() const
{
  return rows_;
}
const Schema &TableHeap::schema() const
{
  return schema_;
};