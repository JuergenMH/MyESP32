// ----------------------------------------------------------------------------
// My ESP NTP module 
// functions for NTP functions (RTC server over WLAN)
// ----------------------------------------------------------------------------

void MyNTP_PrintTime()
{
  #ifndef SilentMode
    SP("NTP Time: "); 
    SP(MyTime.NTPTime.Hour);   SP(":"); 
    SP(MyTime.NTPTime.Minute); SP(":");
    SP(MyTime.NTPTime.Second); SP(" ");
  #endif
}

// ----------------------------------------------------------------------------
void MyNTPHandler_Init()
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
void MyNTPHandler_Function()
{ 
  if ((NTP_ENABLED) && (WLAN_CONNECTED))
  { 
    if (timeClient.update())                    // check for NTP udpate available
    {
      NTP_Online     = true;                    // flag for timer handler: we are online
      NTP_Updated    = true;                    // dito: new time data from NTP available
      NTP_TimeOutCtr = 0;                       // error counter to be cleared
      MyTime.NTPTime.Hour   = timeClient.getHours();
      MyTime.NTPTime.Minute = timeClient.getMinutes();
      MyTime.NTPTime.Second = timeClient.getSeconds();
    }    
    else
    {                                           // no update, check for NTP timeout
      if ((NTP_PERIOD/1000) < NTP_TimeOutCtr)
      {
        NTP_Online = false;                     // timeout reached
      }
      else
      {                                         // no update but also no timeout
        if (NTP_Online) NTP_TimeOutCtr++;       // increment error counter
      }
    }
    // SP ("NTP Online: "); SPLF(NTP_Online);   // for debug purposes only
  }  
}

// ----------------------------------------------------------------------------
