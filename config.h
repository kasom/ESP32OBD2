// Uncomment either of these

//#define HEADLESS
#define ESP32_8048S050
//#define ESP32_8048S070

#if (defined ESP32_8048S050) || (defined ESP32_8048S070)
# define HAS_DISPLAY
#endif

// Enable WiFi
// Put wifissid.txt and wifipass.txt on an SD card.
// These are WiFi SSID, WiFi password
#define USE_WIFI

#ifdef USE_WIFI
  // Logging through UDP, requires USE_WIFI
  // Put loghost.txt and logport.txt on an SD card.
  // These are UDP log server's IP and port, respectively.
# define UDP_LOG

#endif // USE_WIFI
// ------------------------------------------------------------------
// Common
// ------------------------------------------------------------------

#define FIRMWARE_UPDATE_FILE_NAME "/ESP32OBD2.ino.esp32s3.bin"
#define FIRMWARE_BACKUP_FILE_NAME "/ESP32OBD2.ino.esp32s3.bak"

#define FIRMWARE2_UPDATE_FILE_NAME "/ESP32OBD2.ino.bin"
#define FIRMWARE2_BACKUP_FILE_NAME "/ESP32OBD2.ino.bak"

#define LITTLEFS_UPDATE_FILE_NAME "/ESP32OBD2.littlefs.bin"
#define LITTLEFS_BACKUP_FILE_NAME "/ESP32OBD2.littlefs.bak"


// LED/Backlight control
#define PWM_FREQUENCY 200
#define PWM_RESOLUTION 8

#define LOCAL_UDP_PORT 2345
#define WEB_SERVER_PORT 80

#define OUR_NAME "OBD2"

#define DEFAULT_STA_MODE_EN true
#define DEFAULT_AP_MODE_EN true

#define DEFAULT_SOFT_AP_SSID OUR_NAME
#define DEFAULT_SOFT_AP_PASS "EV for Everyone"

#define DEFAULT_STA_AP_SSID ".."
#define DEFAULT_STA_AP_PASS "........"

#define WIFI_AUTO_RECONNECT_INTERVAL_MS 30000

#define CAN_SEND_BUFFER_SIZE 16

// ------------------------------------------------------------------
#ifdef HEADLESS


#define LED_R_PIN   4
#define LED_G_PIN   16
#define LED_B_PIN   17
#define BUZZER_PIN 26

#define LED_R_PWM_CHANNEL 0
#define LED_G_PWM_CHANNEL 1
#define LED_B_PWM_CHANNEL 2

#define LDR_INPUT_PIN 34

// SD Card interface
#define SD_MOSI GPIO_NUM_23
#define SD_MISO GPIO_NUM_19
#define SD_CLK  GPIO_NUM_18
#define SD_CS   GPIO_NUM_5

#define DEFAULT_CAN_TX GPIO_NUM_22
#define DEFAULT_CAN_RX GPIO_NUM_35

#endif

// ------------------------------------------------------------------
#ifdef ESP32_8048S050

#define LCD_BL_PWM_CHANNEL 3
#define LCD_BL_PIN 2

// SD Card interface
#define SD_MOSI GPIO_NUM_11
#define SD_MISO GPIO_NUM_13
#define SD_CLK  GPIO_NUM_12
#define SD_CS   GPIO_NUM_10

#define TFT_WIDTH 800
#define TFT_HEIGHT 480

#define USE_TAMC_GT911

// Capacitive touch (TAMC_GT911) 
#define TOUCH_SDA  19
#define TOUCH_SCL  20
#define TOUCH_INT -1
#define TOUCH_RST 38
#define TOUCH_WIDTH  800
#define TOUCH_HEIGHT 480

#define TFT_PREFER_SPEED 14000000 // Mine is not stable at 16000000
//#define TFT_PREFER_SPEED 16000000 // The demo use this value
#define TFT_HSYNC_POLARITY 0
#define TFT_VSYNC_POLARITY 0
#define TFT_PCLK_ACTIVE_NEG 1

#define TP_DEFAULT_ROTATION ROTATION_NORMAL
#define TFT_DEFAULT_ROTATION 2 // SD card slot on top
#define TFT_IO_B4 1
#define TFT_IO_B1 3
#define TFT_IO_G5 4
#define TFT_IO_G0 5
#define TFT_IO_G1 6
#define TFT_IO_G2 7
#define TFT_IO_B0 8
#define TFT_IO_B3 9
#define TFT_IO_R4 14
#define TFT_IO_G3 15
#define TFT_IO_G4 16
#define TFT_IO_R3 21
#define TFT_IO_HSYNC 39
#define TFT_IO_DE 40
#define TFT_IO_VSYNC 41
#define TFT_IO_PCLK 42
#define TFT_IO_R0 45
#define TFT_IO_B2 46
#define TFT_IO_R2 47
#define TFT_IO_R1 48
#define TFT_HSYNC_FRONT_PORCH 8
#define TFT_HSYNC_PULSE_WIDTH 4
#define TFT_HSYNC_BACK_PORCH 8
#define TFT_VSYNC_FRONT_PORCH 8
#define TFT_VSYNC_PULSE_WIDTH 4
#define TFT_VSYNC_BACK_PORCH 8

#define DEFAULT_CAN_TX GPIO_NUM_17
#define DEFAULT_CAN_RX GPIO_NUM_18

#endif

// ------------------------------------------------------------------
#ifdef ESP32_8048S070

#define LCD_BL_PWM_CHANNEL 3
#define LCD_BL_PIN 2

#define TFT_WIDTH 800
#define TFT_HEIGHT 480

#define USE_TAMC_GT911

// Capacitive touch (TAMC_GT911) 
#define TOUCH_SDA  19
#define TOUCH_SCL  20
#define TOUCH_INT -1
#define TOUCH_RST 38
#define TOUCH_WIDTH  800
#define TOUCH_HEIGHT 480

#define TFT_PREFER_SPEED 14000000 // Mine is not stable at 16000000
//#define TFT_PREFER_SPEED 16000000 // The demo use this value
#define TFT_HSYNC_POLARITY 0
#define TFT_VSYNC_POLARITY 0
#define TFT_PCLK_ACTIVE_NEG 1

#define TP_DEFAULT_ROTATION ROTATION_INVERTED
#define TFT_DEFAULT_ROTATION 0 // SD Card slot on top
#define TFT_IO_G5 1
#define TFT_IO_G2 3
#define TFT_IO_B4 4
#define TFT_IO_B3 5
#define TFT_IO_B2 6
#define TFT_IO_B1 7
#define TFT_IO_G3 8
#define TFT_IO_G0 9
#define TFT_IO_R0 14
#define TFT_IO_B0 15
#define TFT_IO_G4 16
#define TFT_IO_R1 21
#define TFT_IO_HSYNC 39
#define TFT_IO_VSYNC 40
#define TFT_IO_DE 41
#define TFT_IO_PCLK 42
#define TFT_IO_R4 45
#define TFT_IO_G1 46
#define TFT_IO_R2 47
#define TFT_IO_R3 48
#define TFT_HSYNC_FRONT_PORCH 210
#define TFT_HSYNC_PULSE_WIDTH 30
#define TFT_HSYNC_BACK_PORCH 16
#define TFT_VSYNC_FRONT_PORCH 22
#define TFT_VSYNC_PULSE_WIDTH 13
#define TFT_VSYNC_BACK_PORCH 10

#define DEFAULT_CAN_TX GPIO_NUM_17
#define DEFAULT_CAN_RX GPIO_NUM_18

#endif
