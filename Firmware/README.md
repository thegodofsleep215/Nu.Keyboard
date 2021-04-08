# Firmware
## Disclaimer 
There may be more efficient ways of solving this problem. This worked for me to the point where I can't tell the difference between it and a manufactured keyboard.

## Looping
The program runs in a loop. Executing what I am assuming is as fast as the controller can go. On every loop your program will need to read which rows/columns have been pressed or depressed and then send the information on to the computer.

## I/O Types
It is important to understand the I/O types for pins. Since you wire a keyboard in a matrix and read rows and columns. Your firmware will need to switch pins constantly to ensure it is reading the correct value. The two input types are INPUT_PULLDOWN, and INPUT_PULLUP. The first is LOW when the switch is open and HIGH when the switch is closed. The other is the opposite. You'll also need to set the output of your other pin correctly. When using INPUT_PULLDOWN set your other pin mode to OUTPUT and write HIGH to it. When reading from an INPUT_PULLUP pin you'll need to set the other pin's mode to OUTPUT_OPENDRAIN and write LOW to it.

## Selecting Row and Columns 
This program tests each button individually. It iterates over each row for each column. It selects a column (minding I/O types talked about above) and reads from it. It then does the same for every row. Each time a row is read the program tests to see is a key has been pressed or depressed, saving the state to a buffer. After all columns and rows have been read key modifiers are set (ctrl, alt, gui...), LEDs are toggled, and the information is sent along to the computer.


Some simplified pseudo-code:
* Select Column
  * Read Column
  * Deselect Column
  * Select Row
    * Read Row
    * Toggle Press/Depress as needed
    * Deselct Row
* Set Modifiers
* Toggle LEDs
* Send to Computer

