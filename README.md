# AZ-Touch-Pi0-Codelock

Simple Keypad / codelock example for [AZ-Touch Pi0](https://www.hwhardsoft.de/english/projects/az-touch-pi0) wall mount touch screen (ili9341 display) with Pi Zero. This project based on the [Raspberry-ili9340spi](https://github.com/nopnop2002/Raspberry-ili9340spi) project by [nopnop2002](https://github.com/nopnop2002) and contains a set of demo programs to test the touch and tft also.

![AZ-Touch Pi0](https://user-images.githubusercontent.com/3049858/79135621-eef4f080-7daf-11ea-97a6-8760266a50bb.jpg)

# Installation

1. Download the [latest release](https://drive.google.com/file/d/1fyo7i7ajvh9Jw3eJMkq-weTJ9HY11tWT/)
2. Unzip the downloaded file
3. Write the image to your SD card. See [here](https://www.raspberrypi.org/documentation/installation/installing-images/README.md) for details.
4. Boot your Raspberry Pi and wait for start.

# Configuration

##  Wifi settings
You can follow this [tutorial](https://www.raspberrypi.org/documentation/configuration/wireless/headless.md) to setting the Wifi headless. 

or you can use a Raspberry Pi (2/3/4) connected to Ethernet via Putty and SSH:
```bash
sudo raspi-config
--> 2 Network Options
--> N2 Wi-fi
``` 

## First test

for keypad example:

```bash
cd rpi
sudo raspi-config
``` 

for tft demo:


## compilation

```bash
cd rpi
sudo raspi-config
``` 



# License

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
