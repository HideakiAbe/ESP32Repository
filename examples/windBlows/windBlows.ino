#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <webGraphLib.h>

const char* ssid     = "*******";
const char* password = "*******";


AsyncWebServer aWeb(80);



void setup() {
  Serial.begin(115200);
  graph *xyGraph = new graph();
  String graphName("Tornado");
  xyGraph->setGraphName(graphName);

 // creat webGraph object and append web serve and graph object
  webGraph *jsonWebGraph = new webGraph(&aWeb, xyGraph);


  String xKey = "east";
  String yKeys[] = {"north",};
  uint8_t ykeyCount = 1;
  for (int i = 0; i < 5; i++) {
    String Json = randomWindJson();
    while(jsonWebGraph->busy());
    jsonWebGraph->importJson(graphName, Json, xKey, yKeys, ykeyCount);
  }
  optionSettingIfYouLike(jsonWebGraph);
  
  connectToWiFi();
  Serial.println("\nYou can now access to http://" + WiFi.localIP().toString());
  jsonWebGraph->begin();
  while (1) {
    if (millis() % 200 < 10) {
      String Json = randomWindJson();
      jsonWebGraph->importJson(graphName, Json, xKey, yKeys, ykeyCount);
      //Serial.println(Json);
    }
  }
}

void loop() {

}

String randomWindJson() {
  static float oR = 50.0, oTH = PI;
  if ( oTH > (2.0 * PI)) oTH -= (2.0 * PI);
  static float ratio = 1.02;
  float r =  oR * ratio;
  float th = oTH + 0.2;
  oR = r;
  oTH = th;
  String Json = "{\"east\" : " + String(r * cos(th), 4) + ","\
                "\"north\" : " + String(r * sin(th), 4) + "}";
  if (r >= 80) ratio = 0.98;
  if (r <= 1) ratio = 1.02;
  return Json;
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
void optionSettingIfYouLike(webGraph *jsonWebGraph){

// search objects by name
  graph *xyGraph=jsonWebGraph->searchGraphName("Tornado");
  line *L=xyGraph->searchLineName("north"); 
 //y-axis name is line name

//setting to line object
  L->setLineRangeXY(-80.0, 80.0, -80.0, 80.0);
  L->setLineColor("white");
  L->setGrid(5, 5);

//setting to graph
  xyGraph->setGrid(2, 2);
  xyGraph->setSizeXY(200, 200);
  xyGraph->XYvalueString("east", eastStr, "north", northStr);
  
//setting to graph
  jsonWebGraph->webRefreshRate(5);
 
}
String eastStr(float Vx){
  return String(Vx,0)+"m/s";
}
String northStr(float Vy){
  return String(Vy,0)+"m/s";
}