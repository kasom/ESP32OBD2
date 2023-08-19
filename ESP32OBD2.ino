const char info_text[] PROGMEM = R"rawliteral(
/*
  ESP32OBD2

  Version 1
  
  An embedded Javascript platform with OBD-II (CAN) and touch screen LCD support.

  https://github.com/kasom/ESP32OBD2

  Copyright 2023, Kasom
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to dealw
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  
*/
)rawliteral";

#include "config.h"

#ifdef UDP_LOG
#    include <WiFi.h>
#    include <WiFiUDP.h>
#endif
#include <esp_task_wdt.h>
#include <Update.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <driver/gpio.h>
#include <driver/twai.h>
#include <nvs_flash.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>

#include "TAMC_GT911.h"
#include "duk_config.h"
#include "duktape.h"

/*

 GFX_Library for Arduino by Moon On Our Nation, version 1.3.7
 https://github.com/moononournation/Arduino_GFX

 https://github.com/tamctec/gt911-arduino

 https://github.com/lorol/arduino-esp32fs-plugin


~/esptool/esptool.py --chip esp32s3 --port $USBPORT --baud $BAUD \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 80m --flash_size 16MB \
  0x0 ESP32OBD2.ino.bootloader.bin \
  0x8000 ESP32OBD2.ino.partitions.bin \
  0xe000 boot_app0.bin \
  0x10000 ESP32OBD2.ino.bin \
  0x910000 ESP32OBD2.littlefs.bin

 ~/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/tools/partitions
 ~/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/boards.txt

esp32s3.menu.PartitionScheme.large_spiffs_16MB=Large SPIFFS 16MB (4.5MB APP/6.93MB SPIFFS)
esp32s3.menu.PartitionScheme.large_spiffs_16MB.build.partitions=large_spiffs_16MB
esp32s3.menu.PartitionScheme.large_spiffs_16MB.upload.maximum_size=4718592

 OPI PSRAM

 /Applications/Xcode.app/Contents/Developer/usr/bin/pip3 install Pyserial
 cd ~/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/tools
 ln -s ~/esptool/esptool.py

 ~/Library/Arduino15/packages/esp32/hardware/esp32/2.0.11/tools/sdk/esp32s3/qio_opi/include/sdkconfig.h
#define CONFIG_ARDUINO_LOOP_STACK_SIZE 16384

*/

String version_string = String("Firmware compiled : ") + String(__DATE__ " " __TIME__ "\n") + "Built for " + "\n";

SPIClass *sd_spi=NULL;

#ifdef TOUCH_SCL
TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
#endif

#include <Arduino_GFX_Library.h>
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    TFT_IO_DE /* DE */, TFT_IO_VSYNC /* VSYNC */, TFT_IO_HSYNC /* HSYNC */, TFT_IO_PCLK /* PCLK */,
    TFT_IO_R0 /* R0 */, TFT_IO_R1 /* R1 */, TFT_IO_R2 /* R2 */, TFT_IO_R3 /* R3 */, TFT_IO_R4 /* R4 */,
    TFT_IO_G0 /* G0 */, TFT_IO_G1 /* G1 */, TFT_IO_G2 /* G2 */, TFT_IO_G3 /* G3 */, TFT_IO_G4 /* G4 */, TFT_IO_G5 /* G5 */,
    TFT_IO_B0 /* B0 */, TFT_IO_B1 /* B1 */, TFT_IO_B2 /* B2 */, TFT_IO_B3 /* B3 */, TFT_IO_B4 /* B4 */,
    TFT_HSYNC_POLARITY /* hsync_polarity */, TFT_HSYNC_FRONT_PORCH /* hsync_front_porch */, TFT_HSYNC_PULSE_WIDTH /* hsync_pulse_width */, TFT_HSYNC_BACK_PORCH /* hsync_back_porch */,
    TFT_VSYNC_POLARITY /* vsync_polarity */, TFT_VSYNC_FRONT_PORCH /* vsync_front_porch */, TFT_VSYNC_PULSE_WIDTH /* vsync_pulse_width */, TFT_VSYNC_BACK_PORCH /* vsync_back_porch */,
    TFT_PCLK_ACTIVE_NEG /* pclk_active_neg */, TFT_PREFER_SPEED /* prefer_speed */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    TFT_WIDTH /* width */, TFT_HEIGHT /* height */, rgbpanel);

void updateProgressCallBack(size_t currSize, size_t totalSize) {
  Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
}

#define DUK_ADD_FUNCTION(FUNCTION_NAME, NATIVE_FUNCTION, PARAM_COUNT) \
    duk_push_c_function(duk_ctx, NATIVE_FUNCTION, PARAM_COUNT);       \
    duk_put_global_string(duk_ctx, FUNCTION_NAME)

#define DUK_ADD_OBJ_FUNCTION(INDEX, FUNCTION_NAME, NATIVE_FUNCTION, PARAM_COUNT) \
    duk_push_c_function(duk_ctx, NATIVE_FUNCTION, PARAM_COUNT);                  \
    duk_put_prop_string(duk_ctx, INDEX, FUNCTION_NAME)

#define DUK_ADD_INT(INT_NAME, INT_VAL) \
    duk_push_int(duk_ctx, INT_VAL);    \
    duk_put_global_string(duk_ctx, INT_NAME)

#define DUK_ADD_OBJ_INT(INDEX, INT_NAME, INT_VAL) \
    duk_push_int(duk_ctx, INT_VAL);               \
    duk_put_prop_string(duk_ctx, INDEX, INT_NAME)

duk_context *duk_ctx;

duk_ret_t native_print(duk_context *duk_ctx) {
    duk_push_string(duk_ctx, " ");
    duk_insert(duk_ctx, 0);
    duk_join(duk_ctx, duk_get_top(duk_ctx) - 1);
    printf("%s", duk_to_string(duk_ctx, -1));

    return 0;
}

duk_ret_t native_println(duk_context *duk_ctx) {
    duk_push_string(duk_ctx, " ");
    duk_insert(duk_ctx, 0);
    duk_join(duk_ctx, duk_get_top(duk_ctx) - 1);
    printf("%s\n", duk_to_string(duk_ctx, -1));

    return 0;
}

duk_ret_t native_millis(duk_context *duk_ctx) {
    duk_push_int(duk_ctx, millis());
    return 1;
}

void registerNativeFunctions(duk_context *duk_ctx) {
    DUK_ADD_FUNCTION("print", native_print, DUK_VARARGS);
    DUK_ADD_FUNCTION("println", native_println, DUK_VARARGS);
    DUK_ADD_FUNCTION("millis", native_millis, 0);

    register_esp32_functions(duk_ctx);
    register_led_bl_functions(duk_ctx);
    register_tft_functions(duk_ctx);
    register_sd_functions(duk_ctx);
    register_twai_functions(duk_ctx);
    register_auth_functions(duk_ctx);
}

bool isJS(char *filename) {
    int8_t len = strlen(filename);

    if (filename[0] == '.') return false;  // hidden files

    if (strcmp(strlwr(filename + (len - 3)), ".js")) {
        return false;
    } else {
        return true;
    }
}

bool load_all_lib_js(duk_context *ctx) {
    File root = LittleFS.open("/firmware");
    if (!root) {
        gfx->println("Failed to open firmware directory");
        return false;
    }
    if (!root.isDirectory()) {
        gfx->println("Not a directory");
        return false;
    }
    File file = root.openNextFile();

    gfx->println("Listing files...");
    while (file) {
        if (file.isDirectory()) {
            gfx->setTextColor(YELLOW);
            gfx->println("Ignoring directory: ");
            gfx->println(file.name());
        } else {
            gfx->setTextColor(WHITE);
            gfx->print("  FILE: ");
            gfx->setTextColor(GREEN);
            gfx->println(file.name());
            gfx->setTextColor(WHITE);
            gfx->print("  SIZE: ");
            gfx->setTextColor(GREEN);
            gfx->print(file.size());
            if (isJS((char *)file.name())) {
                gfx->setTextColor(YELLOW);
                gfx->println(" Loading...");
                char *buffer = (char *)malloc(file.size() + 1);
                if (buffer == NULL) {
                    gfx->setTextColor(RED);
                    gfx->println("Can't allocate memory!");
                    return false;
                }

                size_t r = file.read((uint8_t *)buffer, file.size());
                if (r != file.size()) {
                    gfx->setTextColor(WHITE);
                    gfx->println("Read: ");
                    gfx->setTextColor(GREEN);
                    gfx->println(r);

                    return false;
                }
                buffer[r] = 0;
                duk_push_string(ctx, buffer);
                free(buffer);
                if (duk_peval(ctx) != 0) {
                    //printf("eval failed: %s\n(File: %s)\n", duk_safe_to_string(ctx, -1), file.name());
                    gfx->setTextColor(RED);
                    gfx->println(duk_safe_to_string(ctx, -1));
                    while(1);
                    // exit(1);
                    // Don't panic, just ignore it.
                }
            } else {
                gfx->setTextColor(RED);
                gfx->println("    Skipping.");
            }
        }
        file.close();
        file = root.openNextFile();
    }
    gfx->setTextColor(WHITE);
    gfx->println("Firmware loaded.");

    return true;
}

int ramInfo(int argc, char **argv) {
    printf("heap size: %d\n", ESP.getHeapSize());
    printf("heap free: %d\n", ESP.getFreeHeap());
    printf("heap min free: %d\n", ESP.getMinFreeHeap());
    printf("heap max alloc: %d\n", ESP.getMaxAllocHeap());

    printf("psram size: %d\n", ESP.getPsramSize());
    printf("psram free: %d\n", ESP.getFreePsram());
    printf("psram min free: %d\n", ESP.getMinFreePsram());
    printf("psram max alloc: %d\n", ESP.getMaxAllocPsram());

    return 0;
}

bool sdInit() {
   bool sdOk=false;
   
    gfx->print("Mounting SD card...");
    sd_spi = new SPIClass(HSPI);
    sd_spi->begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
     
    if (SD.begin(SD_CS,*sd_spi)) {
      gfx->setTextColor(GREEN);
      switch (SD.cardType()) {
        case CARD_MMC: gfx->println("MMC"); break;
        case CARD_SD: gfx->println("SD"); break;
        case CARD_SDHC: gfx->println("SDHC"); break;
        default: gfx->println("Unknown");
      }
      sdOk=true;
    } else {
      gfx->setTextColor(RED);
      gfx->println("Failed.");
      printf("SD card not available.\n");
    }
    gfx->setTextColor(WHITE);
    return sdOk;
}

void firmwareUpdate() {
    printf("\nSearch for firmware..");
    File firmware =  SD.open("/firmware.bin");
    if (firmware) {
      printf("found.\n");
      gfx->setTextColor(WHITE);
      gfx->println("Found /firmware.bin on SD card. Updating...");
      delay(2000);

      Update.onProgress(updateProgressCallBack);

      Update.begin(firmware.size(), U_FLASH);
      Update.writeStream(firmware);
      if (Update.end()){
          printf("Update finished!\n");
          gfx->setTextColor(GREEN);
          gfx->println("Success. Rebooting.");
      } else {
          gfx->setTextColor(RED);
          gfx->println("Failed. ");
          gfx->println(Update.getError());

          printf("Update error!\n%s\n",Update.getError());
      }

      firmware.close();

      SD.remove("/firmware.bak");

      if (SD.rename("/firmware.bin", "/firmware.bak")){
          printf("Firmware rename succesfully!\n");
      } else {
          printf("Firmware rename failed.\n");
      }
      delay(2000);

      ESP.restart();
    } else{
        printf("not found!\n");
    }
}

void dukInit() {
    duk_ctx = duk_create_heap_default();
    // never call duk_destroy_heap(duk_ctx);

    load_all_lib_js(duk_ctx);

    duk_push_global_object(duk_ctx);

    registerNativeFunctions(duk_ctx);

    duk_pop(duk_ctx);

    if (duk_get_global_string(duk_ctx, "setup")) {
        duk_int_t rc = duk_pcall(duk_ctx, 0);
        if (rc != DUK_EXEC_SUCCESS) {
            printf("error: %s\n", duk_safe_to_string(duk_ctx, -1));
        }
    }
    duk_pop(duk_ctx);

}

void littleFSSetup() {
    // check file system
    if (!LittleFS.begin("/littlefs")) {
        gfx->println("Formating & mounting LittleFS file system");

        LittleFS.format();
        LittleFS.begin("/littlefs");
    } else {
        gfx->println("LittleFS file system mounted.");
    }
}

void setup(void) {
 
    // esp_task_wdt_init(60, true);

    initLED_BL_LDR();

#ifdef TOUCH_SCL
    tp.begin();
    tp.setRotation(TP_DEFAULT_ROTATION);
#endif

    gfx->begin();
    gfx->setRotation(TFT_DEFAULT_ROTATION);
    gfx->fillScreen(BLACK);
    gfx->setTextSize(2 /* x scale */, 2 /* y scale */, 1 /* pixel_margin */);

    if (sdInit()) {
      firmwareUpdate();
      networkConfigUpdate();
    }
    
    littleFSSetup();

    dukInit();

#ifdef UDP_LOG
    networkStart();
#endif
}

void loop() {
    if (duk_get_global_string(duk_ctx, "loop")) {
        duk_int_t rc = duk_pcall(duk_ctx, 0);
        if (rc != DUK_EXEC_SUCCESS) {
            printf("error: %s\n", duk_safe_to_string(duk_ctx, -1));
            gfx->fillScreen(BLACK);
            gfx->setCursor(0,0);
            gfx->setTextColor(RED);
            gfx->println(duk_safe_to_string(duk_ctx, -1));
            while(1);

        }
    }
    duk_pop(duk_ctx);
#ifdef UDP_LOG
    networkCheck();
#endif
}
