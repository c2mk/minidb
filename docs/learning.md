Decide where validation lives

Keep TableHeap dumb.

Current TableHeap:

void TableHeap::insertRow(Row row)
{
if (row.size() != schema\_.size())
throw std::runtime_error("Column count mismatch");

rows\_.push_back(std::move(row));
}

That's good.

Storage should only defend its invariants.

Type checking belongs in the executor because it requires understanding schema semantics.
