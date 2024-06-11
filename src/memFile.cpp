// This is a library that supports to write to an EEPROM in a cyclic manner. 
// One can write to the EEPROM in a file-like manner using all Print class methods.

#include <memFile.h>

void MemFile::begin(void) {
  init();
  setPageSize(_psize);
  setMemorySize(_size);
  _size = 256; // just for debugging!
  _start = 0;
  while (read(_start) != 0 && _start < _size) _start++;
  if (_start == _size) {  // no zero byte found -> initialize
    clear();
    return;
  }
  _next = _start+1;
  while (read(_next) != 0) _next = (_next + 1) % _size;
}

size_t MemFile::write(uint8_t c) {
  EEPROM_SPI_WE::write(_next, c);
  _next = (_next + 1) % _size;
  EEPROM_SPI_WE::write(_next, 0);
  return 1;
}

void MemFile::clear(void) {
  _start = 0;
  _next = 1;
  EEPROM_SPI_WE::write(_start, 0);
  EEPROM_SPI_WE::write(_next, 0);
}

uint32_t MemFile::numberOfChars(void) {
  if (_start == _next || read(0) != 0) return _size-1;
  else return _next-1;
}

uint32_t MemFile::numberOfLines(void) {
  long int cnt = 0;
  long int chcnt = 0;
  long int lastchcnt = -1;
  long int ix;
  uint8_t ch;

  if (_start == _next || read(0) != 0) ix = (_next + 1) % _size;
  else ix =  _start + 1;
  ch = read(ix);
    
  while (ch != 0) {
    if (chcnt >= _size) return 0; // no zero found!
    if (ch == '\n') {
      cnt++;
      lastchcnt = chcnt;
    }
    ix = (ix + 1) % _size;
    chcnt++;
    ch = read(ix);
  }
  if (chcnt != lastchcnt + 1) cnt++; // last line without LF
  return cnt;
}
  

char* MemFile::getLine(long int linenum, char line[], size_t len) {
  long int ix;
  long int chix = 0;
  long int cnt = 1;
  long int chcnt = 0;
  uint8_t ch;

  if (_start == _next || read(0) != 0) ix = (_next + 1) % _size;
  else ix =  (_start + 1)  % _size;
  ch  = read(ix);
  
  line[0] = 0;
  while (cnt < linenum) {
    if (ch == 0) return line;
    ix = (ix + 1) % _size;
    if (chcnt >= _size) return line;
    chcnt = chcnt + 1;
    ch = read(ix);
    if (ch == '\n') {
      cnt++;
      ix = (ix + 1) % _size;
      ch = read(ix);
    }
  }

  while (chix < len-1 && ch != '\n' && ch != 0) {
    if (ch == '\r') {
      ix = (ix + 1) % _size;
      ch = read(ix);
      continue;
    }
    line[chix++] = ch;
    ix = (ix + 1) % _size;
    ch = read(ix);
  }
  line[chix] = 0;
  return line;
}
