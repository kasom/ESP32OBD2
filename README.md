# ESP32OBD2
An embedded Javascript platform with OBD-II (CAN) and touch screen LCD support.
In the current version, it can retrieves some interesting information such as 
battery SoC/SoH, HV insulation resistance, etc. and display it on the LCD.

  ![Wiring 1](docs/running.png)

***Work in Progress***
- Needs better documents
- Better UI (menu, icon, etc)
- ISO-TP
- DTC view/clear
- Graphs/visualization
- Local logging on SD card or LittleFS
- The empty space on the left side of the screen is for menu buttons

***Compiling***
- Requires Arduino IDE, version 1.8.19
- Requires esp32 by Espressif Systems, version 2.0.11
- Requires GFX_Library for Arduino by Moon On Our Nation, version 1.3.7
- Requires TAMC_GT911 by TAMC, version 1.0.2
- Must increase the stack size in sdkconfig.h
  
   ~/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/tools/sdk/esp32s3/qio_opi/include/sdkconfig.h
  
    #define CONFIG_ARDUINO_LOOP_STACK_SIZE 16384


***Hardware***
- ESP32S3-8048S050 (5 inch, 800x480 LCD, 16MB flash, 8MB PSRAM)
- CAN bus transceiver module (3.3V)
- OBD2 to Mini USB cable
- Mini USB breakout board

***Wiring***
- Remove 120 Ohm resister between CAN-H and CAN-L from the CAN bus transceiver (if available)
- Connect pin 6 and pin 14 of OBD2 connector to CAN-H and CAN-L of the transceiver, respectively
- Connect pin 14 of OBD2 connector to CAN-L of the transceiver
- From P4 of ESP32S3 board, connect IO17 and IO18 to TX and RX of the transceiver, respectively
- Connect supply power/ground

  ![Wiring 1](docs/wiring1.png)
  ![Wiring 2](docs/wiring2.png)
  ![Wiring 3](docs/wiring3.png)
  ![Wiring 4](docs/wiring4.png)
  
