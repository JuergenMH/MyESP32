// ----------------------------------------------------------------------------
// My ESP NTP module 
// functions for NTP functions (RTC server over WLAN)
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyNTP_PrintTime()
{
  #ifndef SilentMode
    MyNTP_ReadTime();
    SP("NTP Time: "); 
    SP(MyTime.NTPTime.Hour);   SP(":"); 
    SP(MyTime.NTPTime.Minute); SP(":");
    SP(MyTime.NTPTime.Second); SP(" ");
  #endif
}

// ----------------------------------------------------------------------------
void MyNTP_ReadTime()
{
  if ((NTP_ENABLED) && (WLAN_CONNECTED))
  { 
    timeClient.update();
    MyTime.NTPTime.Hour   = timeClient.getHours();
    MyTime.NTPTime.Minute = timeClient.getMinutes();
    MyTime.NTPTime.Second = timeClient.getSeconds();
  }
}

// ----------------------------------------------------------------------------
void MyNTP_Init()
{ 
  uint16_t MyOffset;
  if ((NTP_ENABLED) && (WLAN_CONNECTED))
  {
    #ifndef SilentMode
      SPLF("Establish NTP service");
    #endif  
    timeClient.begin();
    switch (STIME_MODE)
    {
      case 0: MyOffset = NTP_OFFSET;    break;  // 0 = standard time
      case 1: MyOffset = 2*NTP_OFFSET;  break;  // 1 = summer time
      case 2: break;                            // 2 = auto, not implemented yet
    }
    timeClient.setTimeOffset(MyOffset);         // timezone + offset summer/winter
    timeClient.setUpdateInterval(NTP_PERIOD);   // time update interval set
    MyNTP_PrintTime();                          // final echo NTP time if not silentmode 
  }
}

// ----------------------------------------------------------------------------
