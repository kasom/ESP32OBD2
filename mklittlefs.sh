#!/bin/sh
~/Library/Arduino15/packages/esp32/tools/mklittlefs/3.0.0-gnu12-dc7f933/mklittlefs -c ./data -p 256 -b 4096 -s 7208960 ./ESP32OBD2.littlefs.bin
