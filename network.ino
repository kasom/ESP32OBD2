#ifdef UDP_LOG

#define NETWORK_PREF "OBD2WiFi"

WiFiUDP wifiUDP;
bool wifiOn=false;
bool wifiWasDisconnected=false;
String localIP="";
unsigned long previousMillisWifiRetry = 0;
char can2udpIP[128]="";
uint16_t can2udpPort=0;

void networkConfigUpdate() {
  Preferences preferences;

  preferences.begin(NETWORK_PREF,false);
  
  File f =  SD.open("/wifissid.txt");
  if (f) {
    String t=f.readString();
    t.trim();
    preferences.putString("wifissid",t);
    f.close();
    SD.remove("/wifissid.txt");
  }

  f=SD.open("/wifipass.txt");
  if (f) {
    String t=f.readString();
    t.trim();
    preferences.putString("wifipass",t);
    f.close();
    SD.remove("/wifipass.txt");
  }

  f=SD.open("/loghost.txt");
  if (f) {
    String t=f.readString();
    t.trim();
    preferences.putString("loghost",t);
    f.close();
    SD.remove("/loghost.txt");
  }

  f=SD.open("/logport.txt");
  if (f) {
    String t=f.readString();
    t.trim();
    preferences.putInt("logport",t.toInt());
    f.close();
    SD.remove("/logport.txt");
  }
  preferences.end();
}

void wiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START:
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        wifiWasDisconnected=false;
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        if (!wifiWasDisconnected) {
          wifiWasDisconnected=true;
          WiFi.disconnect();
        }
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        localIP=WiFi.localIP().toString();
        
        break;
    case ARDUINO_EVENT_WIFI_AP_START:
        break;
    default: break;
  }
}

void networkCheck() {
  if (WiFi.status() != WL_CONNECTED && (millis() - previousMillisWifiRetry >=WIFI_AUTO_RECONNECT_INTERVAL_MS)) {
    wifiWasDisconnected=false;
    WiFi.reconnect();
    previousMillisWifiRetry = millis();
  }
}

void networkStart() {
  Preferences preferences;
  preferences.begin(NETWORK_PREF, true); // open in read-only mode

  String wifissid=preferences.getString("wifissid","");
  String wifipass=preferences.getString("wifipass","");
  String loghost=preferences.getString("loghost","");
  int logport=preferences.getInt("logport",1234);

  preferences.end();

  bool ok=true;

  if (wifissid=="") { printf("No wifissid configured.\n"); ok=false; }
  if (wifipass=="") { printf("No wifipass configured.\n"); ok=false; }
  if (loghost=="") { printf("No loghost configured.\n"); ok=false; }

  if (ok) {
    char ap_ssid[128];
    char ap_pass[128];

    strcpy(ap_ssid,wifissid.c_str());
    strcpy(ap_pass,wifipass.c_str());
    strcpy(can2udpIP,loghost.c_str());
    can2udpPort=logport;

    printf("Connecting WiFi\n");
    
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname("ESP32OBD2");
    WiFi.onEvent(wiFiEvent);

    WiFi.begin(ap_ssid,ap_pass);

    if (!wifiUDP.begin(logport)) {
      printf("WiFiUDP: no sockets available\n");
    } else {
      printf("WiFiUDP: started.\n");
    }

    wifiOn=true;
  }
}

#endif
