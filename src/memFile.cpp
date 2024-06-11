// This is a library that supports to write to an EEPROM in a cyclic manner. 
// One can write to the EEPROM in a file-like manner using all Print class methods.

#include <memFile.h>

void MemFile::begin(void) {
  init();
  setPageSize(_psize);
  setMemorySize(_size);
  // _size = EEPROM_KBITS_2; // just for debugging!
  _start = 0;
  _bix = 0;
  while (read(_start) != 0 && _start < (long int)(_size)) _start++;
  if (_start == (long int)(_size)) {  // no zero byte found -> initialize
    clear();
    return;
  }
  _next = _start+1;
  while (read(_next) != 0) _next = (_next + 1) % (long int)(_size);
}

size_t MemFile::write(uint8_t c) {
  if (c == 0) return 1;
  _buf[_bix++] = c;
  if (_next + _bix == (long int)(long int)(_size)) {
    _buf[_bix] = 0;
    writeEEPROM(_next, _buf, _bix);
    EEPROM_SPI_WE::write(0,0);
    _next = 0;
    _bix = 0;
  } else {
    if (_bix == MFBSIZE || c == '\n') {
      _buf[_bix++] = 0;
      writeEEPROM(_next, _buf, _bix);
      _next = _next + _bix-1;
      _bix = 0;
    }
  }
  return 1;
}

void MemFile::clear(void) {
  _start = 0;
  _next = 1;
  EEPROM_SPI_WE::write(_start, 0);
  EEPROM_SPI_WE::write(_next, 0);
}

uint32_t MemFile::numberOfChars(void) {
  if (_start == _next || read(0) != 0) return (long int)(_size)-1;
  else return _next-1;
}

uint32_t MemFile::numberOfLines(void) {
  long int cnt = 0;
  long int chcnt = 0;
  long int lastchcnt = -1;
  long int ix;
  uint8_t ch;

  if (_start == _next || read(0) != 0) ix = (_next + 1) % (long int)(_size);
  else ix =  _start + 1;
  ch = read(ix);
    
  while (ch != 0) {
    if (chcnt >= (long int)(_size)) return 0; // no zero found!
    if (ch == '\n') {
      cnt++;
      lastchcnt = chcnt;
    }
    ix = (ix + 1) % (long int)(_size);
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

  if (_start == _next || read(0) != 0) ix = (_next + 1) % (long int)(_size);
  else ix =  (_start + 1)  % (long int)(_size);
  ch  = read(ix);
  
  line[0] = 0;
  while (cnt < linenum) {
    if (ch == 0) return line;
    ix = (ix + 1) % (long int)(_size);
    if (chcnt >= (long int)(_size)) return line;
    chcnt = chcnt + 1;
    ch = read(ix);
    if (ch == '\n') {
      cnt++;
      ix = (ix + 1) % (long int)(_size);
      ch = read(ix);
    }
  }

  while (chix < len-1 && ch != '\n' && ch != 0) {
    if (ch == '\r') {
      ix = (ix + 1) % (long int)(_size);
      ch = read(ix);
      continue;
    }
    line[chix++] = ch;
    ix = (ix + 1) % (long int)(_size);
    ch = read(ix);
  }
  line[chix] = 0;
  return line;
}
