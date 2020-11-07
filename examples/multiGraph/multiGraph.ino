#include <WiFi.h>
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer
#include <webGraphLib.h>


const char* ssid     = "******";
const char* password = "******";


AsyncWebServer Web(80);

void setup() {
  Serial.begin(115200);
  connectToWiFi();

  // first step : make 2 start points
  float startX1, startY1 = 50.0;
  float startX2, startY2 = 60.0;
  point *myStartPoint1 = new point(startX1, startY1);
  point *myStartPoint2 = new point(startX2, startY2);

  //2nd: make  2 lines include the each start point.
  line *myLine1 = new line(myStartPoint1);
  line *myLine2 = new line(myStartPoint2);

  //3rd :make a new graph and add the line
  graph *simpleXYGraph1 = new graph();
  graph *simpleXYGraph2 = new graph();
  simpleXYGraph1->addLine(myLine1);
  simpleXYGraph2->addLine(myLine2);

  //final step  make a webGraph include web server object and 2 graph objects
  webGraph *mywebGraph = new webGraph(&Web);
  mywebGraph->addGraph(simpleXYGraph1);
  mywebGraph->addGraph(simpleXYGraph2);
  // then start server.
  mywebGraph->begin();

  // optional etc setting. if you like.
  myLine1->setLineColor("#00d9ff");
  simpleXYGraph1->setBackgroundColor("#002b33");//https://www.colordic.org/
  simpleXYGraph1->setGrid(16, 10);
  simpleXYGraph1->setSizeXY(300, 300./1.618);

  myLine2->setLineColor("#00ff9d");
  simpleXYGraph2->setBackgroundColor("#00331f");
  simpleXYGraph2->setGrid(16, 10);
  simpleXYGraph2->setSizeXY(300, 300./1.618);
  mywebGraph->webRefreshRate(3);


  Serial.println("\nYou can now access graph to http://" + WiFi.localIP().toString());
  while (1) {
    while(mywebGraph->busy());
    myLine1->addPoint(randomX(), randomY()); //max 255 points are retained.
    myLine2->addPoint(randomX(), randomY()); //max 255 points are retained.
    delay(200);
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
float randomY() {
  static float y = 50;
  float ry = ((float)random(100) + 7.0 * y) / 8.0;
  y = ry;
  return y;
}
float randomX() {
  static float x = 0;
  x += 1.0;
  return x;
}
void loop() {
  // put your main code here, to run repeatedly:

}
