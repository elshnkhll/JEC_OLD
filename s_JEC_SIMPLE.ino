#include <stdint.h>
#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <U8g2lib.h>
#include <RTC_DS1307.h>  // or DS3231 // http://www.instructables.com/id/ESP8266-Wiht-DS1307-RTC-NV-SRAM-and-Square-Wave-Ou/
#include <FS.h>

U8G2_PCD8544_84X48_F_4W_SW_SPI u8g2(U8G2_R2, D8, D7, D5, D6, D0);
RTC_DS1307 rtc;
WiFiClient client;

int tm_cntr;
// initializing the values for our Wi-Fi network
String hstnm   = "                            HSTNM"; // 32 bytes
char* ssid     = "                             SSID";
char* password = "                              PWD";
char* server   = "                             SRVR";
String script  = "                            SCRPT";
String hash    = "                              HSH";
String mac     = "                  MAC";               // 17 bytes
String tz      = "                               TZ"; // 32 bytes
int port       = 80;


byte second = 10;
byte minute = 10;
byte hour = 10;
byte dayOfWeek = 10;
byte dayOfMonth = 10;
byte month = 10;
byte year = 10;
byte dummy = 10;

bool show_cln = true;
bool light_on = false;
int light_cntr = 0;



#include "post.h"
#include "pulsate.h"
#include "Wi_Fi.h"

void setup(void) {

  delay(100);
  Serial.begin(9600);
  Serial.println();
  Serial.println( ESP.getResetReason() );
  Serial.println();
  Wire.begin();
  rtc.SQW( f1hz );
  u8g2.begin();

  if ( ESP.getResetReason() == "Software/System restart" ) {

    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(10);
    pinMode(D4, OUTPUT);
    digitalWrite( D4, LOW );
    delay(10);
    digitalWrite( D4, HIGH );
    delay(100);

    // u8g2.setDisplayRotation(U8G2_R2);

    delay(100);
    tm_cntr = 0;

  }  else {


    delay(1);
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
    delay(1);

    delay(3000);
    if ( !Wi_Fi_Read_Config() ) {
      Serial.println(F("config error"));
      return;
    }

    delay(300);
    if ( !Wi_Fi_Scan() ) {
      Serial.println(F("no Wi-Fi found"));
      return;
    }

    delay(300);
    if ( !Wi_Fi_Connect() ) {
      Serial.println(F("Wi-Fi connect failed"));
      delay(2500);
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.forceSleepBegin();
      ESP.eraseConfig();
      ESP.wdtFeed();
      ESP.deepSleep(32000, WAKE_RF_DEFAULT);
      return;
    }

    mac = Wi_Fi_MAC_Address();
    String post_data = "&mac=" + mac + "&tz=" + tz;
    String res = POST_to_Server( post_data );

    Serial.println();
    Serial.println( res );

    if ( !Wi_Fi_Disonnect() ) {
      Serial.println("Wi-Fi disconnect failed");
    }


    // Convert from String Object to String.
    char buf[20];
    res.toCharArray(buf, 20);
    char *p = buf;
    String str;
    int cntr = 0;
    byte bdt[10];

    while ((str = strtok_r(p, ";", &p)) != NULL) {
      bdt[cntr] = str.toInt() + 0;
      // Serial.println( bdt[cntr] );
      ++cntr;
    }
        
    rtc.SetTime( bdt[6], bdt[5], bdt[4], bdt[3], bdt[2], bdt[1], bdt[0] );

    delay(300);

    ESP.reset();

  }

}

void loop(void) {

  if (tm_cntr < 50) {
    ++tm_cntr;
    int ar = analogRead( A0 );
    if ( ar > 512 ) {
      light_on = true;
      light_cntr = 0;
      delay(8);
    } else {
      delay(8);
    }
    if ( light_on ) {
      if (light_cntr == 0 ) {
        digitalWrite( D4, LOW );
      }
      if (light_cntr < 500 ) {
        ++light_cntr;
      } else {
        digitalWrite( D4, HIGH );
        light_on = false;
      }
    }
  } else {

    Serial.print("|");

    tm_cntr = 0;
    u8g2.clearBuffer();

    char res_char[ 10 ];
    String s;
    String dt_line;

    delay(10);
    rtc.GetTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    delay(10);
    if( hour > 12 ) hour = hour -12;

    // Serial.println(rtc.GetTimeToString() );

    // BIG FONT
    u8g2.setFont(u8g2_font_logisoso32_tn); //u8g2_font_inb24_mn);

    // COLON
    if ( show_cln ) {
      s = ":";
      s.toCharArray(res_char, 2 );
      u8g2.drawStr(35, 44, res_char);
      // u8g2.sendBuffer();
    }
    show_cln = !show_cln;

    // HOUR
    snprintf(res_char, 3, "%02u", hour);
    u8g2.drawStr(-2, 48, res_char);
    // u8g2.sendBuffer();

    // MINUTE
    snprintf(res_char, 3, "%02u", minute);
    u8g2.drawStr(45, 48, res_char);
    // u8g2.sendBuffer();

    // small font
    u8g2.setFont(u8g2_font_crox1h_tf);   //u8g2_font_profont12_tf);

    snprintf(res_char, 3, "%02u", year);
    s = res_char;
    dt_line = "20" + s + "-";

    snprintf(res_char, 3, "%02u", month);
    s = res_char;
    dt_line = dt_line + s + "-";

    snprintf(res_char, 3, "%02u", dayOfMonth);
    s = strcat(res_char, " ");
    dt_line = dt_line + s;
    
    dt_line.toCharArray(res_char, dt_line.length() + 1 );
    u8g2.drawStr(-1, 11, res_char);

    String days_of_Week[7] = { "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN" };
    dt_line = days_of_Week[dayOfWeek-1];
    dt_line.toCharArray(res_char, dt_line.length() + 1 );
    u8g2.drawStr(84-u8g2.getStrWidth(res_char), 11, res_char);


    u8g2.sendBuffer();

  }
}

