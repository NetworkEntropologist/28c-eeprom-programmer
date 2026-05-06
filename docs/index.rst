.. 28C EEPROM Programmer documentation master file, created by
   sphinx-quickstart on Tue May  5 13:37:33 2026.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

28C EEPROM Programmer documentation
===================================

An Aurduino-based programmer for the AT28Cx family of EEPROMS, including the AT28C16, AT28C64 and AT28C256. Currently no support 
is planned for the AT28C512, but this may change in future. However, this will require a hardware redesign, as this IC has a different 
(and incompatible) pinout compared to the rest of the family.

This project started as a way to learn more about EEPROM programming, but also as a simple, cost-effective tool to read and write these 
EEPROMs for use in various projects, including retro computing and hardware hacking. One of the core design goals of this project was 
to be easy enough that any hobbyist would be able to build. 


.. toctree::
   :maxdepth: 2
   :caption: Hardware Reference:

   defines
   shiftregister
   eeprom
   wozmon
   debugging
   

