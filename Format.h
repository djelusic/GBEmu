#pragma once
#include <string>
#include "Types.h"

class Format {

public:

  static std::string Byte(const byte & val);
  static std::string Word(const word & val);

private:

  Format() {}

};