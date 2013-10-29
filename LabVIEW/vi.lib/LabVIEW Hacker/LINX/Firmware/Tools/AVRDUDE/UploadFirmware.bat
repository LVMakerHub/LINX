echo off
avrdude -v -c stk500v2 -p pic32-340 -P COM5 -b 115200 -U flash:w:LIFCK.cpp.hex

pause