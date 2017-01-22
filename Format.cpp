#include "Format.h"
#include <sstream>
#include <iomanip>

std::string Format::Byte(const byte & val) {
  std::stringstream ret;
  ret << std::setfill('0') << std::setw(2)
    << std::hex << (int)val;
  return ret.str();
}

std::string Format::Word(const word & val) {
  std::stringstream ret;
  ret << std::setfill('0') << std::setw(4)
    << std::hex << (int)val;
  return ret.str();
}
