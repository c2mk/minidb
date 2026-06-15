#include "storage/types.hpp"
#include <algorithm>
#include <stdexcept>

DataType parseDataType(const std::string &s)
{
  std::string lower = s;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  if (lower == "int")
    return DataType::Int;
  if (lower == "text")
    return DataType::Text;

  throw std::runtime_error("Unknown data type: '" + s + "'");
}