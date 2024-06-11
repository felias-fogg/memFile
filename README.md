# memFile

This Arduino library provides an interface to EEPROMs. It supports writing sequentially in a cyclic manner. The library is mainly meant to support logging activity. The MemFile class defined here inherits from the EEPROM_SPI_WE class (from the EEPROM_SPI_WE library) and from the `Print` class. 

In addition to all methods supported by the Arduino `Print` class, which may vary according to the core you use, you have the following methods:

* `MemFile(esize, psize, cs, wp, sc)`  - the constructor, where `esize` is the memory size (see `EEPROM_SPI_WE.h` for the constants), `psize`is the page size (see `EEPROM_SPI_WE.h` for the constants), `cs` is the enable pin for SPI, `wp` is the write enable pin (optional), and `sc` is the SPI frequency (also optional);

- `begin()` - should be called in the beginning;
- `write(byte)` - this is the virtual method overriding the method from the `Print` class;
- `numberOfLines()` - number of lines stored in the file;
- `numberOfChars()` - number of characters in the file;
- `getLine(num, buf, size)` - returns the string (space provided by `buf`) of the `num` th line of size maximal `size` ; 
- `clear()` - delete the file.



## Examples

`SimpleTest` - simple test/example

`Maintenance` - Allows maintaining an EEPROM file
