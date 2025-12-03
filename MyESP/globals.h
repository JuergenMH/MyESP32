// ----------------------------------------------------------------------------
// My ESP definitions, global variables
// included by MyESP.ino (projekt main source file)
// ----------------------------------------------------------------------------
//#define SilentMode // 

// ----------------------------------------------------------------------------
//  Arduino pin definitions
#define I2C_SDA   8                   
#define I2C_SCL   9
#define LED_PIN   0

// Some commcon constants, magic nubers, ...
#define SSID_Len  33u           // standard = 32 chars + #0
#define PWD_Len   63u           // standard = 63 chars + #0

// These command are used a lot; 
// to make code more compact macros are defined for that
#define SP        Serial.print
#define SPLF      Serial.println
#define SW        Serial.write

// Screep OLED properties
// currently two OLEDs sizes are are supported
#define DisType_1   // #1: 128x64 pixel
//#define DisType_2   // #2: 128x32 pixel   

#ifdef DisType_1
  #define OLED_WIDTH  128
  #define OLED_HEIGHT 64
  #define OLED_OR     0
  #define OLED_ADR    0x3C
#endif

#ifdef DisType_2
  #define OLED_WIDTH  128
  #define OLED_HEIGHT 32
  #define OLED_OR     1
  #define OLED_ADR    0x3C
#endif

// ----------------------------------------------------------------------------
// Position of graphic elements (OLED #1 = big display)
// ----------------------------------------------------------------------------
#ifdef DisType_1                    // "big" display, 0°
  #define T_YPos    57              // Y Temperature
  #define T_XPos    10              // X Temperature
  #define H_YPos    57              // Y Humidity
  #define H_XPos    45              // X Humidity
  #define TSize     3               // Test size for time display
  #define TH_XPos   3               // X Stunde
  #define TH_YPos   25              // Y Stunde
  #define TM_XPos   45              // X Minute
  #define TM_YPos   25              // Y Minute
  #define TS_XPos   87              // X Sekunde
  #define TS_YPos   25              // Y Sekunde 
  #define Lin_XStrt 0               // X Position Line(s) start  
  #define Lin_XLen  OLED_WIDTH      // X Position Line(s) end, 90°
  #define Lin_YAD   53              // Y Position line above temperature and humidity
  #define Lin_YTim  19              // Y Positions line above time; hour, minute second
  
  #define IP_X      45              // local IP, currently only on big display
  #define IP_Y      5
  #define IP_Size   1
#endif

// ----------------------------------------------------------------------------
// Position of graphic elements (OLED #2 = small display)
// ----------------------------------------------------------------------------
#ifdef DisType_2                    // "small" display, 90°
  #define T_YPos    110             // Y Temperature
  #define T_XPos    2               // X Temperature
  #define H_YPos    120             // Y Humidity
  #define H_XPos    2               // X humidity 
  #define TSize     2               // Test size for time display
  #define TH_XPos   5               // X Stunde
  #define TH_YPos   30              // Y Stunde
  #define TM_XPos   5               // X Minute
  #define TM_YPos   55              // Y Minute
  #define TS_XPos   5               // X Sekunde
  #define TS_YPos   80              // Y Sekunde   
  #define Lin_XStrt 0               // X Position Line(s) start  
  #define Lin_XLen  OLED_HEIGHT     // X Position Line(s) end, 90°
  #define Lin_YAD   105             // Y Position line above temperature and humidity
  #define Lin_YTim  20              // Y Positions line above time; hour, minute second
#endif

#define Lin_Col     SSD1306_WHITE   // Line color

// ----------------------------------------------------------------------------
// Bitmaps
// ----------------------------------------------------------------------------
#define WL_XSTART   1
#define WL_YSTART   1 
#define WL_HEIGHT   11
#define WL_WIDTH    16
static const unsigned char PROGMEM WiFi_Logo[2*WL_HEIGHT] = 
{
  0b00000011,	0b11000000,
  0b00001110,	0b01110000,
  0b00011000,	0b00011000,
  0b00110011,	0b11001100,
  0b01101110,	0b01110110,
  0b01011000,	0b00011010,
  0b00110011,	0b11001100,
  0b00000110,	0b01100000,
  0b00001100,	0b00110000,
  0b00000101,	0b10100000,
  0b00000001,	0b10000000,
};

#define RTC_XSTART   20
#define RTC_YSTART   1 
#define RTC_HEIGHT   11
#define RTC_WIDTH    16
static const unsigned char PROGMEM RTC_Logo[2*WL_HEIGHT] = 
{
  0b11111111, 0b11111111,
  0b00000000, 0b00000000,
  0b11110111, 0b11001110,
  0b10010111, 0b11010001,
  0b10010001, 0b00100000,   
  0b11110001, 0b00100000,
  0b11000001, 0b00100000,
  0b10100001, 0b00110001,
  0b10010001, 0b00001110,
  0b00000000, 0b00000000,
  0b11111111, 0b11111111
};

// ----------------------------------------------------------------------------
// global variables and definitions
// ----------------------------------------------------------------------------
typedef struct
{
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
} TmEntryS;

typedef struct
{
  TmEntryS NTPTime;
  TmEntryS RTCTime;
  TmEntryS SystemTime;
} MyTimeS;

typedef struct                  // configuration data definition
{
  uint16_t  MagicNumber;        // alays 0xdead
  uint8_t   WlanEnabled;        // != 0 = enabled
  uint8_t   WlanSsid[SSID_Len]; // user defined strings
  uint8_t   WlanPwd[PWD_Len];   // 
  uint8_t   NtpEnabled;         // != 0 = enabled
  uint16_t  NtpPeriod;          // update period in ms
  uint16_t  NtpOffset;          // time zone offset for NTP reading
  uint8_t   RtcEnabled;         // != 0 = enabled
  uint8_t   STimeMode;          // summer time 0 = off, 1 = on, 2 = auto 
  uint8_t   SHT31Enabled;       // enable temp/humidity modul SHT31
  uint8_t   BME280Enabled;      // enable temp/humidity modul BME280
  uint8_t   SSD1306Enabled;     // enable display with SSD1306
  uint16_t  Checksum;           // to be calculated before store
} Config_s;

const Config_s DefaultConfig =  // used on empty or invalid EEPROM
{
  0xdead,                       // magic number
  1,                            // WLAN is enabled
  "YourSSID",                   // WLAN SSID home WLAN
  "YourWLANPWD",                // WLAN PWD 
  1,                            // NTP is enabled
  60000,                        // NTP update interval ever 60s
  3600,                         // 3600 for european standard time, Berlin
  1,                            // RTC is enabled
  0,                            // no summer time
  1,                            // SHT31 temp humidity modul enabled
  0,                            // BME280 temp humidity modul disabled
  1,                            // SSD1306 display is enabled
  0x00000000                    // must be calculated before store
};  

enum                            
{
  EE_NotRead,                   // init, EEPROM not read
  EE_Empty,                     // EEPROM seems to be empty
  EE_Invalid,                   // data found; checksum invalid
  EE_Valid                      // ;-)
} ConfigState = EE_NotRead;     // global config state

enum                            
{
  WLAN_NotConnected,            // not connected on startup
  WLAN_ConnectionTimeout,       // cnnection failed with timeout
  WLAN_Connected                // EEPROM seems to be empty
} WLANState = WLAN_NotConnected;// ;-)

const uint16_t ConfigLen     = sizeof DefaultConfig;
const uint16_t MyMagicNumber = 0xdead;

union                           // to hold configuration data set
{
  Config_s  ActConfig;          // "normal" access
  uint8_t   DBytes[ConfigLen];  // ony byte level for EEPROM driver
} ConfigUnion;

// some macros for easy union member access

#define MAGIC_NUMBER    ConfigUnion.ActConfig.MagicNumber
#define WLAN_ENABLED    ConfigUnion.ActConfig.WlanEnabled
#define WLAN_SSID       ConfigUnion.ActConfig.WlanSsid
#define WLAN_PWD        ConfigUnion.ActConfig.WlanPwd
#define NTP_ENABLED     ConfigUnion.ActConfig.NtpEnabled
#define NTP_PERIOD      ConfigUnion.ActConfig.NtpPeriod
#define NTP_OFFSET      ConfigUnion.ActConfig.NtpOffset
#define RTC_ENABLED     ConfigUnion.ActConfig.RtcEnabled
#define STIME_MODE      ConfigUnion.ActConfig.STimeMode
#define SHT31_ENABLED   ConfigUnion.ActConfig.SHT31Enabled
#define BME280_ENABLED  ConfigUnion.ActConfig.BME280Enabled
#define SSD1306_ENABLED ConfigUnion.ActConfig.SSD1306Enabled
#define CHECKSUM        ConfigUnion.ActConfig.Checksum

#define WLAN_CONNECTED  WLANState == WLAN_Connected
#define NTP_SERVER      "pool.ntp.org"

// global variables 
unsigned int      SW_Timer_1 = 0;
MyTimeS           MyTime;
WiFiUDP           ntpUDP;
NTPClient         timeClient(ntpUDP, NTP_SERVER, (uint16_t)NTP_OFFSET, (uint16_t)NTP_PERIOD); 
DS3231            MyRTC;
Adafruit_SSD1306  display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
Adafruit_SHT31    sht31 = Adafruit_SHT31();
float             MySHT31_Temperature;
float             MySHT31_Humidity;

// ----------------------------------------------------------------------------
//  end of include file
// ----------------------------------------------------------------------------

