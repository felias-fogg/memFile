// This is a library that supports to write to an EEPROM in a cyclic manner. 
// One can write to the EEPROM in a file-like manner using all Print class methods.

#include <memFile.h>

void MemFile::begin(void) {
  _size = _mem->getMemorySize();
  //_size = 256; // just for debugging!
  _start = 0;
  while (_mem->read(_start) != 0 && _start < _size) _start++;
  if (_start == _size) {  // no zero byte found -> initialize
    clear();
    return;
  }
  _next = _start+1;
  while (_mem->read(_next) != 0) _next = (_next + 1) % _size;
}

size_t MemFile::write(uint8_t c) {
  _mem->write(_next, c);
  _next = (_next + 1) % _size;
  _mem->write(_next, 0);
  return 1;
}

void MemFile::clear(void) {
  _start = 0;
  _next = 1;
  _mem->write(_start, 0);
  _mem->write(_next, 0);
}

uint32_t MemFile::numberOfChars(void) {
  if (_start == _next || _mem->read(0) != 0) return _size-1;
  else return _next-1;
}

uint32_t MemFile::numberOfLines(void) {
  uint32_t cnt = 0;
  uint32_t chcnt = 0;
  uint32_t lastchcnt = -1;
  uint32_t ix;
  uint8_t ch;

  if (_start == _next || _mem->read(0) != 0) ix = (_next + 1) % _size;
  else ix =  _start + 1;
  ch = _mem->read(ix);
    
  while (ch != 0) {
    if (chcnt >= _size) return 0; // no zero found!
    if (ch == '\n') {
      cnt++;
      lastchcnt = chcnt;
    }
    ix = (ix + 1) % _size;
    chcnt++;
    ch = _mem->read(ix);
  }
  if (chcnt != lastchcnt + 1) cnt++; // last line without LF
  return cnt;
}
  

char* MemFile::getLine(uint32_t linenum, char line[], size_t len) {
  uint32_t ix;
  uint16_t chix = 0;
  uint32_t cnt = 1;
  uint32_t chcnt = 0;
  uint8_t ch;

  if (_start == _next || _mem->read(0) != 0) ix = (_next + 1) % _size;
  else ix =  (_start + 1)  % _size;
  ch  = _mem->read(ix);
  
  line[0] = 0;
  while (cnt < linenum) {
    if (ch == 0) return line;
    ix = (ix + 1) % _size;
    if (chcnt >= _size) return line;
    chcnt = chcnt + 1;
    ch = _mem->read(ix);
    if (ch == '\n') {
      cnt++;
      ix = (ix + 1) % _size;
      ch = _mem->read(ix);
    }
  }

  while (chix < len-1 && ch != '\n' && ch != 0) {
    if (ch == '\r') {
      ix = (ix + 1) % _size;
      ch = _mem->read(ix);
      continue;
    }
    line[chix++] = ch;
    ix = (ix + 1) % _size;
    ch = _mem->read(ix);
  }
  line[chix] = 0;
  return line;
}
