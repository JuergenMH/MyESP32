// ----------------------------------------------------------------------------
// My ESP time handler module 
// handles the different time sources (NTP, RTC, ESP) and provides one time
// for the application
// ----------------------------------------------------------------------------

void LoadAppTimeFromNTP(void)
{
  AppHour     = MyTime.NTPTime.Hour;
  AppMinute   = MyTime.NTPTime.Minute;
  AppSecond   = MyTime.NTPTime.Second;
  AppTSource  = TimeSource_NTP;
  // SPLF("App time update via NTP");
}

void LoadAppTimeFromRTC(void)
{
  MyRTC_ReadTime();                      // copy RTC data to data struct
  AppHour     = MyTime.RTCTime.Hour;
  AppMinute   = MyTime.RTCTime.Minute;
  AppSecond   = MyTime.RTCTime.Second;
  AppTSource  = TimeSource_RTC;
  // SPLF("App time update via RTC");
}

void LoadAppTimeFromESP(void)
{
  // TO DO
  AppTSource  = TimeSource_ESP;
}

void LoadESPTimeFromNTP(void)
{
    // TO DO
}

void GetBackupData(void)
{
  if (RTC_ENABLED)          // best case; RTC is available
  {
    LoadAppTimeFromRTC();   // than we will take the RTC data
  }
  else
  {
    LoadAppTimeFromESP();   // no RTC => take the time from ESP
  }
}

// ----------------------------------------------------------------------------
void MyTimeHandler_Function()   // to be called cyclic in background
{
  if (NTP_Online)               // possible only if WLAN connected
  {
    if (NTP_Updated)            // new time data from NTP available?
    {
      if (RTC_ENABLED)          // if RTC availabe, update the RTC now
      {
        MyRTC_Copy_NTP_To_RTC();
      }                        
      LoadAppTimeFromNTP();     // export the updated time information
      LoadESPTimeFromNTP();     // and update the ESP_Time
      NTP_Updated = false;      // clear flag because flag is handled
    }                           // of NTP updated          
    else                        // NTP is online but no new data 
    {                           // => fall back to another time source
      GetBackupData();          // get data from RTC or ESP
    }                           // of NTP updated
  }
  else                          // NTP is offline
  {     
    GetBackupData();            // get data from RTC or ESP
  }
}

// ----------------------------------------------------------------------------
void MyTimeHandler_Init()
{
}


// ----------------------------------------------------------------------------
