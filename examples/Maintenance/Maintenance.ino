// This sketch can be used to maintain a memory file:
// you can print the first or last n lines, and you can clear the file

#define  HELPSTRING "Commands:\r\n" \
  "h,?       - help\r\n" \
  "p [<num>] - print first <num> or all lines\r\n" \
  "t <num>   - print last <num> lines\r\n" \
  "l         - print number of lines\r\n" \
  "c         - print number of characters\r\n" \
  "w <line>  - write line to the end of the memory file\r\n" \
  "d         - delete file"

#define LINELEN 255

#include <Arduino.h>
#include <EEPROM_SPI_WE.h>
#include <memFile.h>

#ifdef ARDUINO_ARCH_MEGAAVR
const uint16_t CS=PIN_PE3;
#define SerialDebug Serial3
#else
const uint16_t CS=10;
#define SerialDebug Serial
#endif

MemFile mfile = MemFile(EEPROM_KBITS_4096, EEPROM_PAGE_SIZE_256, CS);

char line[LINELEN+1] = { '\0' };

void setup(void)  {
  
  SerialDebug.begin(115200);
  delay(1000);
  SerialDebug.println(F("Maintenance (memFile)"));
  
  mfile.begin();
}

void loop(void) {
  SerialDebug.print(F("MemFile>"));
  readLine(line);
  switch (toupper(line[0])) {
  case 'H':
  case '?':
    SerialDebug.println(HELPSTRING);
    break;
  case 'D':
    delete_file();
    break;
  case 'P':
    print_lines(line);
    break;
  case 'T':
    print_tail(line);
    break;
  case 'L':
    SerialDebug.print(F("Number of lines: "));
    SerialDebug.println(mfile.numberOfLines());
    break;
  case 'C':
    SerialDebug.print(F("Number of characters: "));
    SerialDebug.println(mfile.numberOfChars());
    break;
  case 'W':
    write_line(line);
  case '\0':
    break;
  default:
    SerialDebug.print(F("Unknown command: "));
    SerialDebug.println(line);
    break;
  }
}

void delete_file(void) {
  char ch;
  SerialDebug.print(F("Do you really want to delete the memory file? [y/n]:"));
  while (!SerialDebug.available());
  if ((ch = toupper(SerialDebug.read())) == 'Y') {
    SerialDebug.write(ch);
    mfile.clear();
    SerialDebug.println(F("\nMemory file deleted"));
  } else {
    SerialDebug.println(F("\nNothing done!"));
  }
  delay(50);
  while (SerialDebug.available() && (SerialDebug.peek() == '\r' || SerialDebug.peek() == '\n')) SerialDebug.read();
}

void print_lines(char line[]) {
  uint32_t l = mfile.numberOfLines();
  uint32_t num = parseNum(&line[1]);
  uint32_t ix;
  if (num == 0) num = l;
  if (num > l) {
    SerialDebug.println(F("Please specify a number less than or equal to the number of total lines"));
    return;
  }
  for (ix = 1; ix <= num; ix++) 
    SerialDebug.println(mfile.getLine(ix, line, LINELEN));
}

void print_tail(char line[]) {
  uint32_t l = mfile.numberOfLines();
  uint32_t num = parseNum(&line[1]);
  uint32_t ix;
  if (num == 0) {
    SerialDebug.println(F("Please specify a positive number with the 'T' command"));
    return;
  }
  if (num > l) {
    SerialDebug.println(F("Please specify a number less than or equal to the number of total lines"));
    return;
  }  
  for (ix = l-num+1; ix <= l; ix++) {
    SerialDebug.println(mfile.getLine(ix, line, LINELEN));
  }
}

uint32_t parseNum(char line[]) {
  uint16_t ix=0;
  uint32_t result = 0;
    
  while (line[ix] == ' ' && line[ix] != '\0') ix++;
  while (line[ix] != '\0') {
    if (!isdigit(line[ix])) {
      SerialDebug.println(F("Only digits are allowed!"));
      return 0;
    }
    if (result*10+(line[ix] - '0') < result) {
      SerialDebug.println(F("Number is too large!"));
      return 0;
    }
    result = result*10+(line[ix] - '0');
    ix++;
  }
  return result;
}

void write_line(char line[]) {
  uint32_t ix = 1;

  while (line[ix] == ' ') ix++;
  mfile.println(&line[ix]);
}

/* ---------------------------- Read line & editing ------------------------*/



// read one line until a CR/LF is entered, do not accept more than LINELEN chars
// implement a few basic line editing commands
bool readLine(char *buf)
{
  char next = '\0';
  int i = 0, fill = 0;

  while (next != '\r' && next != '\n') {
    while (!SerialDebug.available());
    next = SerialDebug.read();
    if (next == '\x1b') { // filter out ESC-sequences
      while (!SerialDebug.available());
      next = SerialDebug.read();
      if (next == '[') { 
	while (!SerialDebug.available());
	next = 0;
	switch(SerialDebug.read()) {
	case 'A': next = 'A'-0x40;
	  break;
	case 'B': next = 'E'-0x40;
	  break;
	case 'C': next = 'F'-0x40;
	  break;
	case 'D': next = 'B'-0x40;
	}
      } 
    }
    switch (next) {
    case '\x7F':
    case '\b':
      if (i > 0) {
	i--;
	SerialDebug.write('\b');
	deleteChar(buf, i, fill);
      }
      break;
    case 'A'-0x40: // ^A - start of line
      moveCursor(line, i, fill, 0);
      break;
    case 'B'-0x40: // ^B - one char backwards
      if (i > 0) moveCursor(line, i, fill, i-1);
      break;
    case 'D'-0x40: // ^D - delete current char
      deleteChar(buf, i, fill);
      break;
    case 'E'-0x40: // ^E - end of line
      moveCursor(line, i, fill, fill);
      break;
    case 'F'-0x40: // ^F - one character forward
      if (i < fill) moveCursor(line, i, fill, i+1);
      break;
    case 'K'-0x40: // ^K - kill rest of line
      killRest(buf, i, fill);
      break;
    case 'P'-0x40: // ^P - previous line
      if (i == 0) {
	SerialDebug.print(buf);
	while (i < LINELEN && buf[i]) i++;
	fill = i;
      }
      break;
    default:
      if (next >= ' ' && next < '\x7F' && fill < LINELEN) {
	insertChar(buf, i, fill, next);
      }
      break;
    }
  }
  buf[fill] = '\0'; 
  SerialDebug.print("\r\n");
  delay(50);
  while (SerialDebug.available() && (SerialDebug.peek() == '\r' || SerialDebug.peek() == '\n')) SerialDebug.read();
  return true;
}


void moveCursor(char *line, int &cursor, int &fill, int target)
{
  if (target < 0 || target > fill) return; 
  if (target < cursor) {
    while (target < cursor) { 
      SerialDebug.write('\b');
      cursor--;
    }
  } else {
    while (target > cursor) {
      SerialDebug.write(line[cursor]);
      cursor++;
    }
  }
}


void deleteChar(char *line, int &cursor, int &fill)
{
  int i;
  for (i = cursor; i < fill-1; i++) line[i] = line[i+1];
  line[fill-1] = ' ';
  i = cursor;
  moveCursor(line, cursor, fill, fill);
  moveCursor(line, cursor, fill, i);
  fill--;
}


void killRest(char *line, int &cursor, int &fill)
{
  int i;
  for (i = cursor; i <= fill-1; i++) line[i] = ' ';
  i = cursor;
  moveCursor(line, cursor, fill, fill);
  moveCursor(line, cursor, fill, i);
  fill = i;
}

void insertChar(char *line, int &cursor, int &fill, char newch)
{
  int i;
  if (fill >= LINELEN) return;
  for (i = fill; i >= cursor; i--) line[i+1] = line[i];
  line[cursor] = newch;
  fill++;
  i = cursor;
  moveCursor(line, cursor, fill, fill);
  moveCursor(line, cursor, fill, i+1);
}

