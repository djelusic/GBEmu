#include "Serializer.h"

Serializer::Serializer() {
}

Serializer::~Serializer() {
}

void Serializer::SerializeByte(const byte & val) {
}

byte Serializer::DeserializeByte() {
  return byte();
}

void Serializer::SerializeWord(const word & val) {
}

word Serializer::DeserializeWord() {
  return word();
}

void Serializer::SerializeByteArray(const byte * srcArr, int size) {
}

void Serializer::DeserializeByteArray(byte * destArr, int size) {
}

void Serializer::SaveFile(const char * fpath) {
}

void Serializer::LoadFile(const char * fpath) {
}
