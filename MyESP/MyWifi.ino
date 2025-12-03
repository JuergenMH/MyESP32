// ----------------------------------------------------------------------------
// My ESP WiFi module 
// functions for all basic WiFi functions e.g. connect..
// ----------------------------------------------------------------------------
const uint16_t DelayBetweenConnects = 1000;   // ms
const uint16_t ConnectTimeout       = 10000;  // = 10s

void MyWifi_Init()
{
  unsigned int ConnectTime = 0;
  if (WLAN_ENABLED)
  {
    WiFi.begin((char*)WLAN_SSID, (char*)WLAN_PWD);  
    SP("\nConnect to WiFi ");
    // loop until connected or timeout
    while ((WiFi.status() != WL_CONNECTED) && (ConnectTime < ConnectTimeout))
    {
      delay(DelayBetweenConnects);
      ConnectTime = ConnectTime + DelayBetweenConnects;
      #ifndef SilentMode
        SP(".");
      #endif
    }
    // check connection result and timeout occured
    if (WiFi.status() == WL_CONNECTED)
    {
      #ifndef SilentMode          
        SPLF(" connected!");
      #endif
      WLANState = WLAN_Connected;
    }
    else
    {
      #ifndef SilentMode
        SPLF(" connection timeout!");
      #endif
      WLANState = WLAN_ConnectionTimeout;
    }
  }
}