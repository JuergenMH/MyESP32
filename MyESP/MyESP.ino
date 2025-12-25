#include <stdio.h>
#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <Adafruit_SHT31.h>
#include "globals.h"

// ----------------------------------------------------------------------------
// My ESP main module 
// contains Arduino setup and loop functions
// Board select: ESP32C3 Dev Module
// ----------------------------------------------------------------------------
const char HelloStr1[] = "\nMyEsp32 V1.0, System start";
const char HelloStr2[] = "Setup finished.";

// ----------------------------------------------------------------------------
void HandleConfiguration(void)
{
  MyConfig_ReadConfig();            // initial eprom read
  while (EE_Valid != ConfigState)   // if not OK...
  {
    MyConfig_PerformConfigDialog(); // enter config dialog
    MyConfig_ReadConfig();          // read eeprom againg
  }                                 // until content is valid
}

// ----------------------------------------------------------------------------
void DisplayTempAndHumidity(void)
{
  char Tmp[5], Hum[5];
  MySHT31_ReadTValues();                                        // update data
  display.setTextSize(1);                                       // medium font size  
  display.drawLine(Lin_XStrt, Lin_YAD, 
                   Lin_XLen,  Lin_YAD, Lin_Col);                // draw line above 
  snprintf(Tmp, 5, "%f\n", MySHT31_Temperature);                // temperature string
  snprintf(Hum, 5, "%f\n", MySHT31_Humidity);                   // humidity string
  display.setCursor(T_XPos, T_YPos); display.print(Tmp); display.print((char)247);
  display.setCursor(H_XPos, H_YPos); display.print(Hum); display.print("%");
}

// ----------------------------------------------------------------------------
// Helper function to display Hour, Minute or Second with leading zero
void GetStrWLZ(char* ValueString, uint8_t Value, int x, int y)
{
  snprintf(ValueString,  4, "%2d", Value);      // value to string string
  if ((char)ValueString[0] == 32)               // leading blank?
    ValueString[0] = '0';                       // if yes, replace by 0
  display.setCursor(x, y);                      // set cursor to output pos  
  display.print(ValueString);                   // finally display time (h, m, s)
}

// ----------------------------------------------------------------------------
// Display time horizontally (big display) or vertical (small display)
void DisplayTime(void)
{
  char Hr[3], Min[3], Sec[3];
  display.setTextSize(TSize);                               // medium font size
  display.drawLine(Lin_XStrt, Lin_YTim, 
                   Lin_XLen,  Lin_YTim, Lin_Col);           // draw line above 
  GetStrWLZ(Hr,  AppHour,   TH_XPos, TH_YPos);              // display hour
  GetStrWLZ(Min, AppMinute, TM_XPos, TM_YPos);              // display minute
  GetStrWLZ(Sec, AppSecond, TS_XPos, TS_YPos);              // display second
}

// ----------------------------------------------------------------------------
void DrawBitmaps(void)
{
  display.drawBitmap(WL_XSTART,  WL_YSTART,  WiFi_Logo, WL_WIDTH,  WL_HEIGHT,  1);
  display.drawBitmap(RTC_XSTART, RTC_YSTART, RTC_Logo,  RTC_WIDTH, RTC_HEIGHT, 1);
}

// ----------------------------------------------------------------------------
void DisplayIPAddress(void)
{
  #ifdef DisType_1                                          // only on big display
    if (WLANState == WLAN_Connected)                        // and if connected
    {
      display.setTextSize(IP_Size);                         // small font size
      display.setCursor(IP_X, IP_Y);                        // logo area
      display.print(WiFi.localIP());                        // print IP address
    }
  #endif
}

// ----------------------------------------------------------------------------
// System setup
// running from arduino setup() and in case of user request
// ----------------------------------------------------------------------------
void SystemSetup(void)
{
  SPLF("");                 // ensure to start in a new line..
  SPLF(HelloStr1);          // and give welcome message and version
  HandleConfiguration();    // Read configuration, start configurator if necessary
  MyWifiHandler_Init();     // Connect WiFi in case of WiFi is enabled
  MyNTPHandler_Init();      // Establish NTP serive WLAN connected and NTP enabled
  MyTimeHandler_Init();     // init function of logical time handler
  MyRTC_Init();             // Init DS3231 RTC; get NTP time of possible  
  MySHT31_Init();           // SHT31 Temperature and humidity module    
  MySystem_Init();          // OS HW Timer configuration
  MyOLED_Init();            // initialise the configured OLED
  MyTimeHandler_Init();     // initialise the background time handler
  MySystem_StartTimer();    // OS HW Timer start
  SPLF(HelloStr2);          // setup finished
}

// ----------------------------------------------------------------------------
// The main application is a standard arduino style application triggert by 
// one software timer. This application runs in addition to the background
// tasks handled by the system scheduler
// the background is executed if the MainApplication is not running
// ----------------------------------------------------------------------------
void MainApplication(void)        // visible user application
{
  // SPLF("Main Application running"); // for debug only ...
  display.clearDisplay();       // first step display memory clear
  DisplayTempAndHumidity();     // update temperature and humdity
  DisplayTime();                // update the time
  DrawBitmaps();                // all bitmaps in the upper area
  DisplayIPAddress();           // display IP on big display and if WLAN is connectd
  display.display();            // last step: update the display with all the new stuff
}

// ----------------------------------------------------------------------------
void MainBackground(void)       // executed if main is not running
{
  uint8_t UserInput;            // user input on serial console
  if (Serial.available())       // check character received 
  {
    UserInput = Serial.read();  // get character from user
    switch (UserInput)
    {
      case STRG_C:
        MySystem_StopTimer();           // first: stop hw timer
        MyConfig_PerformConfigDialog(); // no execute configuration
        MyConfig_ReadConfig();          // read eeprom againg
        MySystem_StartTimer();          // enable HW timer again
        break;
      case STRG_X: 
        MySystem_StopTimer();           // first: stop hw timer
        SystemSetup();                  // then call all other setup stuff
        break;
    }
  }
}

void setup() 
{
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  delay(2000);              // establish serial monitor
  SystemSetup();            // call all other setup stuff
}

// ----------------------------------------------------------------------------
void loop() 
{
  MySystem_Function();                          // SW Timer, OS, scheduler 
  if (0 == SW_Timer_1)                          // SW Timer elapsed?
  {
    SW_Timer_1 = 200;                           // reload to main tic = 200ms
    switch (MainSchedule)
    {
      case 0: MyWifiHandler_Function(); break;  // handle Wifi background 
      case 1: MyNTPHandler_Function();  break;  // handle NTP background
      case 2: MyTimeHandler_Function(); break;  // handle logical time
      case 3: break;
      case 4: MainApplication(); break;         // arduino style main application
    }
    //MainSchedule++;                           // next Main FSM state
    if (++MainSchedule == 5)                    // scheduler round robin
      MainSchedule = 0;
    //Serial.println(MainSchedule) ;            // for debugging only
  }
  else
  {
    MainBackground();                           // if nothing else to do ...
  }
}

// ----------------------------------------------------------------------------
//  end of main module
// ----------------------------------------------------------------------------
