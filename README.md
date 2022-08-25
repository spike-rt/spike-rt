[![Build](https://github.com/spike-rt/spike-rt/workflows/Build/badge.svg?branch=main)](https://github.com/spike-rt/spike-rt/workflows/Build/badge.svg?branch=main)

For Japanese document please see [README_ja.md](README_ja.md).
日本語版は，[README_ja.md](README_ja.md) を参照

:warning:**This is still pre-release and unstable. Breaking changes maybe introduced without notifications.**

# SPIKE-RT
An RTOS-based software platform for [LEGO® Education SPIKE™ Prime](https://education.lego.com/en-us/products/lego-education-spike-prime-set/45678#spike%E2%84%A2-prime).

## Features
- TOPPERS/ASP3 real-time operating system. 
  - A µITRON-like RTOS.
- C programming environment.
- Supported device:
  - Hub
    - 5x5 LED Matrix Display
    - Single LED
    - Button
    - Gyro Sensor
    - Speaker
    - Battery Charging
    - USB Serial
    - Bluetooth
  - PUP Device
    - Angular Motor
    - Color Sensor
    - Ultrasonic Sensor
    - Force Sensor
- Flash application by USB DFU．

## API
- ASP3 API
  - TOPPERS/ASP3 is an implementation of the TOPPERS 3rd-generation kernel (ITRON-based) specification, an extension of the µITRON4.0 specification.
  - Sorry, but the API document is [Japanaese](https://www.toppers.jp/docs/tech/tgki_spec-350.pdf) only for now. 
  - For English reference, [µITRON 4.0 document](http://www.ertl.jp/ITRON/SPEC/FILE/mitron-400e.pdf), the base of the TOPPERS specification, may be helpful.
- C standard library (Newlib)
- SPIKE API
  - An API for controlling device from applications.
  - See [the document](https://envzhu.github.io/spike-rt/en/html/modules.html)

## Usage
For more information, see [docs/en/](docs/en/).

## Contribution
See [docs/en/CONTRIBUTING.md](docs/en/CONTRIBUTING.md)

## Language
Both English and Japanese are available on GitHub.
