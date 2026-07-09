# 28c-eeprom-programmer

![Hardware: CERN-OHL-S v2](https://img.shields.io/badge/hardware-CERN--OHL--S%20v2-blue)
![Firmware & Software: MIT](https://img.shields.io/badge/firmware%20%26%20software-MIT-green)
![Python 3.10.19](https://img.shields.io/badge/python-3.10.19-blue?logo=python&logoColor=white)

A parallel EEPROM programmer for the 28C family, built around an Arduino Nano. Simply drop an EEPROM into the ZIF socket, connect the Nano over USB, and burn a ROM image onto it from your computer.

If you've watched Ben Eater's 8-bit computer series, this will look familiar. That's where the idea came from. I wanted a proper PCB instead of the breadboard version, and a ZIF socket so I'm not bending chip pins every time I swap an EEPROM.

## About this project

This is a personal project that I'm putting it up in case someone else finds it useful or interesting. I'm not planning to write full documentation right now, and I might not ever get to it. So for the moment, the board files and the code are the documentation.

If you find a bug, or you get it working with a chip I haven't tested, feel free to open an issue or a pull request. Just don't expect a fast reply — this is a side thing I work on when I have time.

## Where this came from

For a number of years I have been interested in retrocomputing, when one day I stumbled across [Ben Eater's YouTube channel](https://www.youtube.com/@BenEater). Following his series on espeially the [6502 breadboard computer project](https://youtube.com/playlist?list=PLowKtXNTBypFbtuVMUVXNR0z1mu7dp7eH) I wanted to start building this for myself, or rather using this as inspiration to design and build my own 6501 computer, and this EEPROM Programmer seemed like the best place to start, as this would be a required tool for pretty much everything else. 

I have to mention at this point, two other related projects that I used heavily for both inspiration, as well as for insights into how other creators tackled this:

- **Ben Eater's [eeprom-programmer](https://github.com/beneater/eeprom-programmer)** — the one from his 8-bit computer series. Same core idea as mine: an Arduino driving the address lines through a pair of 74HC595 shift registers. If you've never seen it, watch his video first. He explains the whole thing better than I can here.
- **Erik van Zijst's [eeprom](https://github.com/erikvanzijst/eeprom)** — an Arduino Uno shield with a KiCad board and an interactive Python client. Seeing someone take the breadboard version and turn it into a proper PCB with host software was a big part of what pushed me to do my own board.

So thanks to both of them. My version borrows ideas from each and mixes in what I wanted out of it.

## Does it actually work?

Short answer? Yes. First test on the PCB, after soldering it, was to attempt burning a copy of Wozmon onto the IC, which checked out byte exact. Turns out Wozmon is a really great test, as the contents is very well known and documented all over the internet.

## What it does

- Writes a ROM image to a 28C-family EEPROM over USB serial
- Reads the chip back so you can verify the write

## Supported chips

Anything in the 28C family that follows the 28C64 pinout should work. That being said, I've only tested it with a 28C64 so far. So treat everything else as *should work* rather than *known good*. If you run it with a 28C16 or 28C256, I'd like to hear how it goes.

## Hardware / BOM

The main parts, leaving out the passives:

- 1x Arduino Nano driving the entire process
- 2x 74HC595 shift registers (serial in, parallel out) driving the address lines
- 1x 28-pin ZIF socket

## Repo layout

- `hardware/` — the full KiCad project (schematic and PCB)
- `firmware/` — the Arduino firmware for the Nano
- `software/` — the host-side Python that talks to the board

## Getting boards made

The KiCad project lives in `hardware/`. Open it in KiCad, export the Gerbers, and send them off to whichever fab you like. I used JLCPCB with a purple soldermask and had no trouble — DRC passed and the boards were fine.

## Flashing the firmware

The firmware in `firmware/` was written with [PlatformIO](https://platformio.org/), not the regular Arduino IDE. So the easiest way to build and upload is with PlatformIO (either the VS Code extension or the CLI). Open the `firmware/` folder as a PlatformIO project, plug in the Nano, and run the upload.

If you're using an older Nano clone, you may need to point the build at the old bootloader, otherwise the upload will fail. That's set in the `platformio.ini`.

This assumes you've used PlatformIO before. If you haven't, their [docs](https://docs.platformio.org/) cover installing it and uploading to a board better than I could here.

## Using the software

The host software is in `software/` and it's written in Python. Fair warning: it's still very much a work in progress. Because of that I'm not going to write down a fixed set of commands here that might change next week. Have a look in the `software/` folder and read the code to see how it works for now.

I'm planning to rewrite this side in Rust later on, so expect it to change.

## License

- Hardware (everything in `hardware/`): [CERN-OHL-S v2](https://ohwr.org/cern_ohl_s_v2.txt)
- Firmware and software (`firmware/` and `software/`): [MIT](https://opensource.org/licenses/MIT)
