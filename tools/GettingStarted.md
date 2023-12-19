Tools are used to check integrity, status or functionality of the system.

HW_Test is a circuit used to reproduce a meter's signal in order to improve hardware, software or check if the pico is in working order.
Test conditions : A 384000 kHz sampled, recorded signal coming from a L16C6 meter is being reproduced on an Earfun DSD512 into the Vin pin. Vout pin is looked at through a logic analyzer for UART 7E1 @ 1200 bits/s.

UART_TEST.bin is a binary of TIC frames, which can be fed to a device (eg: USB <-> TTL) to recreate the frames simply and easily. Not every TIC word is present and the format is based on L16C6.Playing directly into pico allows someone to improve and test software or check if the pico is in working order.
Setup on linux looks something like :
'
stty -F /dev/ttyUSB0 1200 cs7 -cstopb parenb -parodd -opost
cat UART_TEST.bin > /dev/ttyUSB0
'
