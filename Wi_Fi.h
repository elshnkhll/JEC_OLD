boolean Wi_Fi_Scan() {
  WiFi.forceSleepWake();
  WiFi.mode( WIFI_STA );
  // look for any available Wi-Fi networks
  Serial.println("Scanning for to Wi-Fi");
  // int n = WiFi.scanNetworks();
  return true;     // return (n > 0);
}

boolean Wi_Fi_Connect() {
  // If networks are found, attempt to connect to our Wi-Fi network
  WiFi.hostname(hstnm);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi .");
  int attempts = 0;
  while ( WiFi.status() != WL_CONNECTED )  {
    delay(500);
    Serial.print(".");
    pulsate();

    if ( attempts++ > 25 ) {
      break;
    }
  }
  if ( WiFi.status() != WL_CONNECTED ) {
    return false;
  }
  Serial.println("");
  Serial.println("Wi-Fi connected");
  return true;
}

boolean Wi_Fi_Disonnect() {
  WiFi.disconnect();
  Serial.print("Disconnecting Wi-Fi .");
  int attempts = 0;
  while (WiFi.status() == WL_CONNECTED)  { // WL_DISCONNECTED
    delay(500);
    Serial.print(".");
    if ( attempts++ > 25 ) {
      break;
    }
  }
  if ( WiFi.status() == WL_CONNECTED ) {
    return false;
  }
  Serial.println("");
  Serial.println("Wi-Fi disconnected");
  return true;
}

String Wi_Fi_MAC_Address() {
  byte mc[6];  // MAC address
  WiFi.macAddress(mc);
  return String(mc[0], HEX) + ":" + String(mc[1], HEX) + ":" + String(mc[2], HEX) + ":" + String(mc[3], HEX) + ":" + String(mc[4], HEX) + ":" + String(mc[5], HEX);
}

bool Wi_Fi_Read_Config() {
  String key;
  String val;
  key = "";
  val = "";
  // strcpy(server, const_cast<char*>(val.c_str()) );

  SPIFFS.begin();
  File f = SPIFFS.open("/CONFIG.INF", "r");
  if (!f) {
    Serial.println("file open failed");
    return false;
  } else {
    Serial.println("Reading config");
    while ( f.available() ) {
      key = f.readStringUntil('=');
      val = f.readStringUntil('\n');
      Serial.println( val );
      if ( key == "ssid" ) {
        for ( int i = 0; i < val.length() - 1; i++) {
          ssid[i] = val[i];
        }
        ssid[val.length() - 1] = '\0';
      }

      if ( key == "password" ) {
        for ( int i = 0; i < val.length() - 1; i++) {
          password[i] = val[i];
        }
        password[val.length() - 1] = '\0';
      }

      if ( key == "server" ) {
        for ( int i = 0; i < val.length() - 1; i++) {
          server[i] = val[i];
        }
        server[val.length() - 1] = '\0';
      }
      if ( key == "hstnm" ) hstnm = val;
      if ( key == "script" ) script = val;
      if ( key == "hash" ) hash  = val;
      if ( key == "port" ) port  = val.toInt();
      if ( key == "tz" ) tz  = val;
    }


    f.close();
  }
  return true;
}
