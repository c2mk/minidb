#pragma once

#include "storage/types.hpp"

class TableHeap
{
public:
  // why explicit
  // explicit on the constructor — this prevents C++ from silently using your constructor to convert a Schema into a TableHeap somewhere you didn't intend. Without explicit, a function expecting a TableHeap could silently accept a raw Schema and construct one behind your back. Always mark single-argument constructors explicit unless you specifically want implicit conversion.

  explicit TableHeap(Schema schema);
  void insertRow(const Row &row);
  // why const after
  const std::vector<Row> &rows() const;
  const Schema &schema() const;

private:
  Schema schema_;
  std::vector<Row> rows_;
};
