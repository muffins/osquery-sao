# osquery-sao
Repo for the source code running on DEFCON 2024 osquery SAO badge


Take this code and use platform io or avrdude to flash to the osquery badge.

You also need to set the chip flags as follows, so run this command before you flash:
```
avrdude -c usbtiny -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
```