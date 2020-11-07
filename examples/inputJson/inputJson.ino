#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TimeLib.h>
#include <webGraphLib.h>

const char* ssid     = "*******";
const char* password = "*******";




long  timezone = 0;

AsyncWebServer aWeb(80);

graph *xyGraph = new graph();
webGraph *jsonWebGraph = new webGraph(&aWeb, xyGraph);

String xKey = "time";
//xKey must be  one of the json's key ,no Multiple.

String yKeys[] = {"temperature", "humidity", "pressure"};

//all yKeys also must be json's keys , up to max 5 keys.
//for json exsample
//"{"time" : 8320,"temperature" : 30.6,"humidity" : 62.4,"pressure" : 996.5}"


uint8_t ykeyCount = sizeof(yKeys) / sizeof(String);
//plese specify number of ykeys

String graphName = "Radom environment";
//graphName will be disply as graph title.

time_t GtimeSave;
void setup() {
  Serial.begin(115200);

  connectToWiFi();
  setupNTP();
  xyGraph->setGraphName(graphName);
  GtimeSave = now();
  for (int i = 0; i < 5; i++) {
    String Json = randomJson();
    //Serial.println(Json);
    jsonWebGraph->importJson(graphName, Json, xKey, yKeys, ykeyCount);
  }
  //we can specify call back function
  //cali back function is type of    String func(float val);

  xyGraph->XYvalueString(xKey, timeToDisplayf, yKeys[0], stdDispTmp);
  xyGraph->XYvalueString(xKey, timeToDisplayf, yKeys[1], stdDispH);
  xyGraph->XYvalueString(xKey, timeToDisplayf, yKeys[2], stdDispP);


  xyGraph->setGrid(8, 3);

  jsonWebGraph->webRefreshRate(600);

  Serial.println("\nYou can now access to http://" + WiFi.localIP().toString());
  jsonWebGraph->begin();
}

void loop() {
  if (millis() % 1000 < 10) {
    String Json = randomJson();
    while(jsonWebGraph->busy());
    jsonWebGraph->importJson(graphName, Json, xKey, yKeys, ykeyCount);
  }
}

String randomJson() {
  static float oldT = 10, oldH = 25, oldP = 50;
  oldT = ((float)random(20) + oldT * 7.) / 8.0;
  oldH = ((float)random(50) + oldH * 7.) / 8.0;
  oldP = ((float)random(100) + oldP * 7.) / 8.0;
  delay(30);

  String Json = "{\"time\" : " + String(now() - GtimeSave) + ","\
                "\"temperature\" : " + String(20.0 + oldT, 1) + ","\
                "\"humidity\" : " + String(40. + oldH, 1) + ","\
                "\"pressure\" : " + String(950. + oldP, 1) + "}";
  return Json;
}

String myHostName() {
  String h = __FILE__;
  return h.substring(h.lastIndexOf("\\") + 1, h.lastIndexOf(".ino"));
}
void connectToWiFi() {
  Serial.println("Connecting to WiFi network");
  WiFi.disconnect(true);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
String timeToDisplayf(float fval) {
  time_t val = (time_t)fval + GtimeSave;
  TimeElements tm2;
  breakTime(val, tm2);
  String S = "";
  S += (tm2.Hour < 10 ? "0" : "") +  String(tm2.Hour) + ":";
  S += (tm2.Minute < 10 ? "0" : "") +  String(tm2.Minute) + ":";
  S += (tm2.Second < 10 ? "0" : "") +  String(tm2.Second);
  return S;
}
String stdDispX(float fval) {
  String S = String((unsigned int)fval / 1000) + "[S]";
  return S;
}

String stdDispTmp(float fval) {
  String S = String((int)fval) + "[C]";
  return S;
}
String stdDispH(float fval) {
  String S = String((unsigned int)fval) + "[%RH]";
  return S;
}
String stdDispP(float fval) {
  String S = String((unsigned int)fval) + "[hPa]";
  return S;
}
time_t setupNTP() {
  byte daysavetime = 0;
  configTime(3600 * timezone, daysavetime * 3600, "pool.ntp.org", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct ;
  tmstruct.tm_year = 0;
  while (tmstruct.tm_year + 1900 < 2020) {
    getLocalTime(&tmstruct, 5000);
    delay(1000);
    Serial.printf("\n%d/%02d/%02d %02d:%02d:%02d", (tmstruct.tm_year) + 1900, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  }
  Serial.printf("\n%d/%02d/%02d %02d:%02d:%02d", (tmstruct.tm_year) + 1900, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  setTime(tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec, tmstruct.tm_mday, tmstruct.tm_mon + 1, (tmstruct.tm_year) + 1900);
  return now();
}