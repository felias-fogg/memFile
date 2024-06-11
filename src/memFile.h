// This is a library that supports to write to an EEPROM in a cyclic manner. 
// One can write to the EEPROM in a file-like manner using all Print class methods.

#ifndef MEMFILE_H_
#define MEMFILE_H_

#define MFBSIZE 32

#include <Arduino.h>
#include <EEPROM_SPI_WE.h>

class MemFile : public EEPROM_SPI_WE, public Print
{
 public:
 MemFile(eeprom_size_t esize, eeprom_pageSize psize, uint16_t cs, uint16_t wp=999, uint32_t sc = 8000000) :
  EEPROM_SPI_WE(cs, wp, sc), _start{0}, _next{0}, _psize{psize}, _size{esize}, _bix{0} {}
     
  void begin(void);
  virtual size_t write(uint8_t);

  uint32_t numberOfLines(void);
  uint32_t numberOfChars(void);
  char* getLine(long int, char[], size_t);
  void clear(void);

 private:
  long int _start;
  long int _next;
  eeprom_pageSize _psize;
  eeprom_size_t _size;
  uint8_t _buf[MFBSIZE+1];
  uint8_t _bix;
};

#endif
