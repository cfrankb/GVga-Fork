# About

This project is a game port to RP2040 (Raspberry Pico) microcontroller of an MS-DOS Game.

Current status is alpha. 90% of the core mechanic has been implemented.

## Original MS-DOS

![Image](techdocs/images/Screenshot_2022-10-19_21-01-20.png "icon")

## RP2040 VGA (GVGA library)

![Image](techdocs/images/20241221_100753-crop.jpg "icon")

Gameplay: https://www.youtube.com/watch?v=SPQwWkzFRpU

I am also releasing the source code for the map editor:
https://github.com/cfrankb/cs3-map-edit

Try the online version of the game:
https://cfrankb.itch.io/creepspread-iii

# Software required

RP2040 / Pico SDK

# Installation

This project is based in part on: https://github.com/drfrancintosh/GVga

```Shell
git clone https://github.com/cfrankb/GVga-Fork.git
cd app/cs3_vga
./vmake.sh
./flashdata.sh
./update.sh
```

# Hardware

## VGA Adaptor

An adaptor is needed to communicate with the VGA Hardware.


![Image](techdocs/images/20250202_211146-crop.jpg "icon")


kicad / gerber: [cfrankb/ESP32-S3-VGA-ADAPTOR](https://github.com/cfrankb/ESP32-S3-VGA-ADAPTOR)


## Gamepad

![Image](techdocs/images/20241222_072413-crop.jpg "icon")



| Aim   | GPIO Pin |
| ----- | -------- |
| UP    | 38       |
| DOWN  | 35       |
| LEFT  | 47       |
| RIGHT | 48       |

```
CGameController gamepad(GPIO_NUM_38, GPIO_NUM_35, GPIO_NUM_47, GPIO_NUM_48);
engine->attach(&gamepad);
```

The above configuration is for the ESP32S3. Pin out will vary per model.

