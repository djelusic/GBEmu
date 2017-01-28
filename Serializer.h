#pragma once
#include "Types.h"
#include <iostream>

class Serializer {

private:

  byte *data;
  int maxLength;
  int pos;

public:

  Serializer(int maxLength);
  ~Serializer();
  
  template<typename T>
  void Serialize(T val) {
    int size = sizeof(T);
    if (pos + size > maxLength) {
      std::cout << "Serialize - max capacity exceeded." << std::endl;
      return;
    }
    for (int i = 0; i < size; i++) {
      data[pos++] = val & 0xFF;
      val >>= 8;
    }
  }

  template<typename T>
  T Deserialize(){
    int size = sizeof(T);
    if (pos + size > maxLength) {
      std::cout << "Deserialize - max capacity exceeded." << std::endl;
      return T();
    }
    T result{};
    for (int i = 0; i < size; i++) {
      result |= data[pos++] << (i * 8);
    }
    return result;
  }

  template<typename T>
  void SerializeArray(const T * srcArr, int size) {
      for (int i = 0; i < size; i++) {
        Serialize<T>(srcArr[i]);
      }
    }

  template<typename T>
  void DeserializeArray(T * destArr, int size) {
    for (int i = 0; i < size; i++) {
      destArr[i] = Deserialize<T>();
    }
  }

  void SaveFile(const char * fpath);
  void LoadFile(const char * fpath);
  byte * GetData();
  void SetData(const byte * data);
  void Reset();
};