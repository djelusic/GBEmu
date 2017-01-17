#pragma once
#include "Types.h"

class Serializer {

private:

  byte *data;
  int pos;

public:

  Serializer();
  ~Serializer();
  
  void SerializeByte(const byte & val);
  byte DeserializeByte();
  void SerializeWord(const word & val);
  word DeserializeWord();
  void SerializeByteArray(const byte * srcArr, int size);
  void DeserializeByteArray(byte * destArr, int size);

  void SaveFile(const char * fpath);
  void LoadFile(const char * fpath);
};