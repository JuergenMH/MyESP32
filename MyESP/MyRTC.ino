// ----------------------------------------------------------------------------
// My ESP RTC module 
// functions for Dallas RTC DS3231
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyRTC_PrintTime()
{
  if (RTC_ENABLED)
  {
    MyRTC_ReadTime();                         // RTC data to data struct
    #ifndef SilentMode
      SP("RTC Time: ");                       // and print the data struct
      SP(MyTime.RTCTime.Hour);   SP(":"); 
      SP(MyTime.RTCTime.Minute); SP(":");
      SP(MyTime.RTCTime.Second); SP(" ");
    #endif  
  }
}

// ----------------------------------------------------------------------------
void MyRTC_ReadTime()                       // copy RTC data to data struct
{
  bool h12Flag;
  bool pmFlag;
  if (RTC_ENABLED)
  { 
    MyTime.RTCTime.Hour   = MyRTC.getHour(h12Flag, pmFlag);
    MyTime.RTCTime.Minute = MyRTC.getMinute();
    MyTime.RTCTime.Second = MyRTC.getSecond();
  }
}

// ----------------------------------------------------------------------------
void MyRTC_Init()                           // copy NTC time to RTC if possible
{ 
  if ((RTC_ENABLED) && (NTP_ENABLED ) && (WLAN_CONNECTED))
  {
    #ifndef SilentMode
      SPLF(" ");
      SPLF("Init DS3231 RTC with NTP time");
    #endif  
    MyRTC.setClockMode(false);              // set to 24h
    MyRTC.setHour(MyTime.NTPTime.Hour);     // h,m,s from NTP
    MyRTC.setMinute(MyTime.NTPTime.Minute);
    MyRTC.setSecond(MyTime.NTPTime.Second);
    MyRTC_PrintTime();                      // final: echo RTC time if not silentmode 
  }
}

// ----------------------------------------------------------------------------
