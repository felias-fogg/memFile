// This is a library that supports to write to an EEPROM in a cyclic manner. 
// One can write to the EEPROM in a file-like manner using all Print class methods.

#ifndef MEMFILE_H_
#define MEMFILE_H_

#include <Arduino.h>
#include <EEPROM_SPI_WE.h>

class MemFile : public Print
{
 public:
 MemFile(EEPROM_SPI_WE * mem) : _mem{mem}, _start{0}, _size{0}   {}
  void begin(void);
  virtual size_t write(uint8_t);

  uint32_t numberOfLines(void);
  uint32_t numberOfChars(void);
  char* getLine(uint32_t, char[], size_t);
  void clear(void);

 private:
  EEPROM_SPI_WE * _mem;
  uint32_t _start;
  uint32_t _next;
  uint32_t _size;
};

#endif
