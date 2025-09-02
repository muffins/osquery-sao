# osquery-sao

Repo for the source code running on DEFCON 2024 osquery SAO badge

Take this code and use platform io or avrdude to flash to the osquery badge.

You also need to set the chip flags as follows, so run this command before you flash:

```bash
avrdude -c usbtiny -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
```

## BOM

- [1 x ATTiny85](https://www.digikey.com/en/products/detail/microchip-technology/ATTINY85-20PU/735469?s=N4IgTCBcDaIIIBUEEkByBNAHAVgLRgAYAFAVRAF0BfIA)
- [1 x 470 Ohm surface mount resistor](https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RPC2512JT470R/1963733)
- [1 x JST Angle battery connector](https://www.digikey.com/en/products/detail/jst-sales-america-inc/S2B-PH-SM4-TB/926655)
- [1 x Switch](https://www.digikey.com/en/products/detail/c-k/JS102011JCQN/6137628)
- [1 x LiPo battery](https://www.digikey.com/en/products/detail/adafruit-industries-llc/328/5054542)
- [1 x SAO Connector](https://www.tindie.com/products/twinkletwinkie/10-pack-of-sao-female-keyed-connectors/)
- [8 x NeoPixel 5050 (WS2812 SMD LEDs)](https://www.adafruit.com/product/1655)
