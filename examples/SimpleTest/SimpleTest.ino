// Test of the memFile library

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

EEPROM_SPI_WE mem = EEPROM_SPI_WE(CS);
MemFile mfile = MemFile(&mem);


void setup(void) {
  char str[] = "Hello World";
  int i;
  char line[64];

  
  SerialDebug.begin(115200);
  delay(1000);
  SerialDebug.println(F("SimpleTest (memFile)"));

  if (mem.init()) {
    SerialDebug.println(F("EEPROM initialized"));
  }

  // important parameters
  mem.setPageSize(EEPROM_PAGE_SIZE_256);
  mem.setMemorySize(EEPROM_KBITS_4096);

  mfile.begin();

  SerialDebug.println(F("Clearing everything"));
  mfile.clear();

  SerialDebug.print(F("Number of lines: "));
  SerialDebug.println(mfile.numberOfLines());

  SerialDebug.print(F("Number of characters: "));
  SerialDebug.println(mfile.numberOfChars());

  mfile.println("1st line, nothing fancy");

  mfile.println(F("2nd line with F macro"));
  
  mfile.println(F("Printing numbers: "));

  mfile.println(42, DEC);
  mfile.println(42, HEX);
  mfile.println(42, BIN);
  mfile.println(F("01234567890123456789012345678901234567890123456789012345678901234567890123456789"));
  for (i = 8; i <= 9; i++) {
    mfile.print("Line: ");
    mfile.println(i);
  }

#ifdef ARDUINO_ARCH_MEGAAVR
  mfile.printf("Simple printf test. Number: %d, String: %s\n\r", i+1, str);
  mfile.printf(F("Printf with F-Macro; %d / %s\n\r"), i+2, str);
#endif

  mfile.println(F("Last line!"));

  mfile.begin(); // Only necessary when we had a wrap around

  SerialDebug.print(F("Number of lines: "));
  SerialDebug.println(mfile.numberOfLines());

  SerialDebug.print(F("Number of characters: "));
  SerialDebug.println(mfile.numberOfChars());

  SerialDebug.print(F("Line 1: "));
  SerialDebug.println(mfile.getLine(1, line, 64));

  
  SerialDebug.print(F("Line 7: "));
  SerialDebug.println(mfile.getLine(7, line, 64));

  SerialDebug.print(F("line 10: "));
  SerialDebug.println(mfile.getLine(10, line, 64));

  SerialDebug.print(F("line 11: "));
  SerialDebug.println(mfile.getLine(11, line, 64));

  SerialDebug.print(F("line 12: "));
  SerialDebug.println(mfile.getLine(12, line, 64));

  SerialDebug.print(F("line 5: "));
  SerialDebug.println(mfile.getLine(5, line, 64));

  SerialDebug.print(F("Last line: "));
  SerialDebug.println(mfile.getLine(mfile.numberOfLines(), line, 64));
}

void loop(void) {}

