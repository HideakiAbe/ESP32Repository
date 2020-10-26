#include <TimeLib.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <webGraphLib.h>
const char* ssid     = "*******";
const char* password = "*******";



const char * mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient MQTTclient(espClient);

AsyncWebServer aWeb(80);
time_t GtimeSave;

graph *xyGraph = new graph();
webGraph *jsonWebGraph = new webGraph(&aWeb, xyGraph);
String graphName("sicurvia");
//this graph name also use as MQTT message topic

void setup() {
  Serial.begin(115200);
  Serial.println(F(__FILE__ ));  // Standard Predefined Macros
  Serial.print(F("Build Date  : "));
  Serial.println(F(__DATE__ ));  // Standard Predefined Macros
  Serial.print(F("Build  Time : "));
  Serial.println(F(__TIME__));  // Standard Predefined Macros

  connectToWiFi();
  GtimeSave = setupNTP();
  Serial.printf("\nTime now %ul", GtimeSave);
  MQTTclient.setServer(mqtt_server, 1883);
  MQTTclient.setCallback(MQTTcallback);
  reconnect();
  xyGraph->setGraphName(graphName);
  while (xyGraph->searchLineName("temperature") == 0) {
    waitForFirstSubscription();
  }
  xyGraph->XYvalueString("time", timeToDisplay, "temperature",  tDisp);
  xyGraph->XYvalueString("time", timeToDisplay, "humidity",  hDisp);
  xyGraph->setXvalueStringAngle(60);

  Serial.println("\nYou can now access to http://" + WiFi.localIP().toString());
  jsonWebGraph->begin();
}

void loop() {

  static uint32_t t = 0;
  if (!MQTTclient.connected()) {
    reconnect();
  }
  MQTTclient.loop();
  if (t < millis()) {
    t = millis() + 1000 * 60;
  }
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
time_t setupNTP() {
  long timezone = 9;
  byte daysavetime = 0;
  configTime(3600 * timezone, daysavetime * 3600, "pool.ntp.org", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct ;
  tmstruct.tm_year = 0;
  while (tmstruct.tm_year + 1900 < 2020) {
    getLocalTime(&tmstruct, 5000);
    delay(1000);
    //DEBUGOUT("waiting time server");
    Serial.printf("\n%d/%02d/%02d/%02d:%02d:%02d", (tmstruct.tm_year) + 1900, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  }
  Serial.printf("\n%d/%02d/%02d/%02d:%02d:%02d", (tmstruct.tm_year) + 1900, ( tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);

  setTime(tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec, tmstruct.tm_mday, tmstruct.tm_mon + 1, (tmstruct.tm_year) + 1900);
  //DEBUGOUT(timeToDisplay3(now()))
  return now();
}
String tDisp(float t) {
  return String(t, 1) + "C";
}
String hDisp(float h) {
  return String(h, 0) + "%RH";
}
String timeToDisplay(float residu) {
  //genarate japanes time style "yyyy/mm/dd hh:mm:sec"
  time_t val = GtimeSave;
  val +=  (time_t)residu;
  TimeElements tm2;
  breakTime(val, tm2);
  String S;
  S = String(tm2.Year + 1970) + "/";
  S += (tm2.Month < 10 ? "0" : "") + String(tm2.Month) + "/";
  S += (tm2.Day < 10 ? "0" : "") +  String(tm2.Day) + " ";
  S += (tm2.Hour < 10 ? "0" : "") +  String(tm2.Hour) + ":";
  S += (tm2.Minute < 10 ? "0" : "") +  String(tm2.Minute) + ":";
  S += (tm2.Second < 10 ? "0" : "") +  String(tm2.Second);
  return S;
}
volatile static time_t lastMessageTime = 0;
void MQTTcallback(char* topic, byte* payload, unsigned int plength) {
  //Serial.println("Message arrived []" );
  char *json;
  json = (char *)payload;
  json[plength] = 0;
  String JSON(json);
  //Serial.println(JSON);
  if (lastMessageTime + 5 < now()) {
    importMQTTTopic(JSON);
    lastMessageTime = now();
  }
}
void waitForFirstSubscription() {
  while (lastMessageTime == 0) {
    loop();
  }
}
const char *myHostName() {
  String h = __FILE__;
  return h.substring(h.lastIndexOf("\\") + 1, h.lastIndexOf(".ino")).c_str();
}
void reconnect() {
  if (!MQTTclient.connected()) {
    Serial.print("\nAttempting MQTT connection...");
    const char * clientId = myHostName();
    if (MQTTclient.connect(clientId)) {
      Serial.print("\nMQTT connected to broker");
      MQTTclient.subscribe(graphName.c_str());
    } else {
      Serial.print("\nfailed, rc=");
      Serial.print(MQTTclient.state());
      Serial.print(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
#if ARDUINOJSON_VERSION_MAJOR == 5
String importMQTTTopic(String input) {
  DynamicJsonBuffer jsonInputBuffer;
  JsonObject& root = jsonInputBuffer.parseObject(input);
  if (!root.success()) {
    Serial.println(F("Failed to read json string"));
    return String("");
  }
  Serial.printf("\nroot.size=%u", root.size());
  String ttt("time");
  String env[2] = {"temperature", "humidity"};

  root[ttt] = now() % SECS_PER_DAY;
  //Because of now()(from 1970 in sec)  is too long as float accuracy
  Serial.printf("\nroot.size=%u", root.size());

  String outJson;
  root.printTo(outJson);
  jsonInputBuffer.clear();
  Serial.println(outJson);
  jsonWebGraph->importJson(graphName, outJson, ttt , env, 2);
  return outJson;
}
#endif
#if ARDUINOJSON_VERSION_MAJOR >= 6
String importMQTTTopic(String input) {
  DynamicJsonDocument doc(1024);
  DeserializationError  error = deserializeJson(doc, input);
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return String("");
  }
  String ttt("time");
  String env[2] = {"temperature", "humidity"};
  doc[ttt] = now() -GtimeSave;
  
  //SECS_PER_DAY is defined in <TimeLib.h>
  //Because of now()(seconds from 1970/jan/1 ) is too long as float accuracy,
  //we should record the time from this today's begining.
  
  String outJson;
  serializeJson(doc, outJson); 
  Serial.println(outJson);
  doc.clear();
  jsonWebGraph->importJson(graphName, outJson, ttt , env, 2);
  return outJson;
}
#endif