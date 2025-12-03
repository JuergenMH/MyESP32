#include <EEPROM.h>
// ----------------------------------------------------------------------------
// My ESP EEProm und Configuration module 
// try to read configuration from EEPROM
// in case of no config found the configuration functions for OLED connected to SPI
// for ESP32 preferred datatypes refer to
// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/preferences.html
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Low level functions with direct access to the EEPROM
// ----------------------------------------------------------------------------
void DumpEEProm(void)       // read EEPROM and perform dump to serial console
{
  uint16_t  i;
  EEPROM.begin(ConfigLen);
  #ifndef SilentMode
    SPLF("Dump EEProm conent: ");
  #endif  
  for (i=0; i<ConfigLen; i++) 
  {
    SP(ConfigUnion.DBytes[i]);    
    SP(" ");
  }
  EEPROM.end();
}
 
// ----------------------------------------------------------------------------
void ReadEEProm(void)       // read EEPROM to config data union
{
  uint16_t  i;
  uint16_t  CS = 0;
  #ifndef SilentMode
    SP("Read config from eeprom,");
  #endif  

  EEPROM.begin(ConfigLen); 
  for (i=0; i<ConfigLen; i++) 
  {
    ConfigUnion.DBytes[i] = EEPROM.read(i);  
  }
  EEPROM.end();

  // first check EEPROM empty?
  if ((0x0000 == MAGIC_NUMBER) || (0xffff == MAGIC_NUMBER)) 
    {
      ConfigState = EE_Empty;
      #ifndef SilentMode
        SP("EEProm seems to be empty!");
      #endif  
    }
    else 
    {
      // not empty, test the checksum
      for (i=0; i<ConfigLen-2; i++) 
        CS += ConfigUnion.DBytes[i];  
      if (CS == CHECKSUM)
      {
        ConfigState = EE_Valid;
        #ifndef SilentMode
          SP("EEProm content valid!");
        #endif  
      }
      else
      {
        ConfigState = EE_Invalid;
        #ifndef SilentMode
          SPLF("EEProm content invalid!");
          SP("CS calculated : "); SPLF(CS);
          SP("CS read cfg   : "); SPLF(CHECKSUM);
        #endif  
      }
    }
}

// ----------------------------------------------------------------------------
void WriteEEProm(void)      // write config data union to the  EEPROM
{
  uint16_t  i;
  uint16_t  CS = 0;
  EEPROM.begin(ConfigLen);
  #ifndef SilentMode
    SP("\nWrite config to eeprom started, ");
  #endif  
  // data write loop (without CS at the end!)
  for (i=0; i<ConfigLen-2; i++) 
  {
    EEPROM.write(i, ConfigUnion.DBytes[i]);  
    CS += ConfigUnion.DBytes[i];
  }
  EEPROM.write(ConfigLen-2, (uint8_t)(CS & 0xFF));  // write low byte CS
  EEPROM.write(ConfigLen-1, (uint8_t)(CS >> 8));    // write high byte CS 
  #ifndef SilentMode
    SPLF(" finished.");
  #endif  
  EEPROM.commit();
  EEPROM.end();
}

// ----------------------------------------------------------------------------
//  Helper function for the configuration menu
// ----------------------------------------------------------------------------
void PString(unsigned char* MyString, uint16_t Len, uint8_t LF)
{
  uint8_t i = 0;
  uint8_t data;
  uint8_t finished = 0;
  while (0 == finished)
  {
    data = MyString[i++];
    if ((0 == data) || (i == Len)) 
      finished = 1;
    else
    if ((30 < data) && (data < 127))
      SW(data);     // valid ascii data
  }  
  if (LF) SPLF("");  
}

void DumpConfiguration(void)
{
  SP("\n");
  SP("Magic number                   : "); SPLF(MAGIC_NUMBER);
  SP("WLAN enabled, SSID, PW         : "); SP(WLAN_ENABLED);  SP(", "); 
                                           PString(WLAN_SSID, SSID_Len, false); SP(", ");PString(WLAN_PWD, PWD_Len, true);
  SP("NTP enabled, period[ms], offset: "); SP(NTP_ENABLED);   SP(", "); SP(NTP_PERIOD); SP(", "); SPLF(NTP_OFFSET); 
  SP("RTC enabled, summer time mode  : "); SP(RTC_ENABLED);   SP(", "); SPLF(STIME_MODE);
  SP("SHT31, BME280, SSD1306 enabled : "); SP(SHT31_ENABLED); SP(", "); SP(BME280_ENABLED);  SP(", ");SPLF(SSD1306_ENABLED);  
  SP("Checksum, config state         : "); SP(CHECKSUM);      SP(", "); SPLF(ConfigState);
}

// ----------------------------------------------------------------------------
uint8_t SReadChar(void)           // read one character e.g. menu select
{
  uint8_t UsrInput;
  while(0 == Serial.available()); // wait until character received 
  UsrInput = Serial.read();       // read character
  SP((char)UsrInput);             // perform echo
  return UsrInput;                // and return received character
}

uint8_t SReadFlag(char* FlagName, uint8_t TheFlag)
{                                 // read a flag; e.g. enable / disable item
  uint8_t UserInput;
  SP("\nCurrent "); 
  SP(FlagName); 
  SP(" state is: "); 
  SP(TheFlag);
  SP(", set new state (0/1) :");
  UserInput = SReadChar();
  SP(", "); SP(FlagName); SP(" is now");
  if ('0' ==  UserInput)
  {
    SPLF(" disabled");
    TheFlag = 0;
  }
  if ('1' == UserInput)
  {
    SPLF(" enabled");
    TheFlag = 1;
  }
  return TheFlag;
}

void SReadString(uint8_t* Target, uint8_t MaxLen)
{                               // read a complete string, e.g. WLAN password
  uint8_t NewStr[MaxLen+1];     // +1 because of #0 at the end
  uint8_t Finished = 0;         // flag for input loop
  uint8_t Dest = 0;             // points to pos for new chars
  uint8_t UserInput;            // one character read from user

  while (0 == Finished)
  {
    UserInput = SReadChar();
    if ((char)13 == UserInput)  // user finished the string input?
    {
      // input finished, set flag and copy string to target
      Finished = 1;             
      (void)strcpy((char*)Target,(char*)NewStr);
    }
    else
    { 
      // input not finished, append character at the end of the string
      NewStr[Dest++] = UserInput;
      NewStr[Dest] = 0;
    }
    // Limit input length 
    if (MaxLen == Dest) Finished = 1;
  }
}

uint32_t SReadInteger(uint8_t MaxLen)
{               
  uint32_t  NewValue = 0;         // final vavlue given from user
  uint8_t   UserInput;            // one character read from user
  uint8_t   Finished = 0;         // flag for input loop
  uint8_t   Len = 0;
  while (0 == Finished)
  {
    UserInput = SReadChar();
    if (((char)13 == UserInput) || (Len >= MaxLen))
    {
      Finished = 1;
    }
    else
    {
      if (UserInput >= 48 && UserInput <= 57) 
      {
        NewValue = NewValue * 10 + (UserInput - 48);
        Len++;
      }
      else break;
    }
  }
  return NewValue;
}

void ShowConfigMenu(void)
{
  SPLF("\n-----------------------------------------------------------------------------------------");
  SPLF(" Manage configuration and store configuration to EEPROM   ");
  SPLF("-----------------------------------------------------------------------------------------");
  SPLF(" <a> dump configuration     <f> enable/disable NTP time       <k> enable/disable SHT31   ");
  SPLF(" <b> load default config    <g> set NTP update period         <l> enable/disable BME280  ");
  SPLF("                            <h> set NTP offset (time zone)    <m> enable/disable SSD130  ");
  SPLF(" <c> enable/disable WLAN                                                                 ");
  SPLF(" <d> set WLAN SSID          <i> enable/disable RTC            <s> Write config to EEPROM ");
  SPLF(" <e> set WLAN Password      <j> set summe time mode           <x> Exit conguration menu  ");
  SPLF("                                                              <z> Dump eeprom content    ");
  SPLF("-----------------------------------------------------------------------------------------");
  SP("Your selection please: ");
}

void LoadDefaults(void)
{
  ConfigUnion.ActConfig = DefaultConfig;           // config load
  SPLF("\nDefault configuration loaded");
}

void SetNTPPeriod(void)
{
  SP("\nNew NTP period [ms]: ");
  NTP_PERIOD = SReadInteger(6);
  SPLF("");
}
void SetNTPZone(void)
{
  SP("\nNew NTP time offset (time zone) [s]: ");
  NTP_OFFSET = SReadInteger(5);
  SPLF("");
}

void SetTimeMode(void)
{
  SP("\nSummer time [0: off, 1:on, 2:auto]: ");
  STIME_MODE = SReadInteger(2);
  SPLF("");
}

// ----------------------------------------------------------------------------
//  Functions with interface to other modules
// ----------------------------------------------------------------------------
void MyConfig_PerformConfigDialog(void)
{
  uint8_t UserInput;
  uint8_t UserExit = 0;
  while (0 == UserExit)
  {
    ShowConfigMenu();
    UserInput = SReadChar();
    switch (UserInput)
    {
      case 'a': DumpConfiguration(); break;
      case 'b': LoadDefaults();      break;
      case 'd': SP("\nNew SSID: "); SReadString(WLAN_SSID, SSID_Len);  break;
      case 'e': SP("\nNew PWD:  "); SReadString(WLAN_PWD, PWD_Len);    break;
      case 'g': SetNTPPeriod();  break;
      case 'h': SetNTPZone();    break;
      case 'j': SetTimeMode();   break;
      case 'c': WLAN_ENABLED    = SReadFlag("WLAN",    WLAN_ENABLED);    break;
      case 'f': NTP_ENABLED     = SReadFlag("NTP" ,    NTP_ENABLED);     break;
      case 'i': RTC_ENABLED     = SReadFlag("RTC",     RTC_ENABLED);     break;
      case 'k': SHT31_ENABLED   = SReadFlag("SHT31",   SHT31_ENABLED);   break;
      case 'l': BME280_ENABLED  = SReadFlag("BME280",  BME280_ENABLED);  break;
      case 'm': SSD1306_ENABLED = SReadFlag("SSD1306", SSD1306_ENABLED); break;
      case 's': WriteEEProm(); break;
      case 'x': SPLF(" exit configuration menu."); UserExit = 1; break;
      case 'z': DumpEEProm(); break;
    }
  }
}

// ----------------------------------------------------------------------------
void MyConfig_ReadConfig(void)
{
  ReadEEProm();
}

// ----------------------------------------------------------------------------
