#include "Serializer.h"
#include <fstream>

Serializer::Serializer(int maxLength) :
  pos(0),
  maxLength(maxLength) {
  data = new byte[maxLength]();
}

Serializer::~Serializer() {
  delete[] data;
}

void Serializer::SaveFile(const char * fpath) {
  std::ofstream fout(fpath, std::ios::out | std::ios::binary);
  fout.write((char*)data, maxLength);
  fout.close();
}

void Serializer::LoadFile(const char * fpath) {
  std::ifstream fin(fpath, std::ios::in | std::ios::binary);
  fin.seekg(0, std::ios::end);
  int length = fin.tellg();
  if (length <= maxLength) {
    fin.seekg(0, std::ios::beg);
    fin.read((char*)data, length);
    fin.close();
  }
}

void Serializer::Reset() {
  pos = 0;
}
