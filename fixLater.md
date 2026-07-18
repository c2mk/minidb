raw strings everywhere, might need to get string_view

std::visit eventually, also templated maybe for operators like =,>,<

currently Schema = vector<ColumnSchema>, so have to write free functions like find
ColumnIndex, count PK, might want to make Schema a class and add these as methods

writeCatalog creates tableNames every time, too expensive, refactor that somwhow
