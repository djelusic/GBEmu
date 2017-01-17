#pragma once
#include <cstdint>

typedef uint8_t byte;
typedef int8_t sbyte;
typedef uint16_t word;
typedef int16_t sword;

struct Color {
  byte R;
  byte G;
  byte B;
};

class Serializer;

struct Serializable {
  virtual void Serialize(const Serializer &s) = 0;
  virtual void Deserialize(const Serializer &s) = 0;
};