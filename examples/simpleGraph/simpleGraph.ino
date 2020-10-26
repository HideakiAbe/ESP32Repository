#include <WiFi.h>
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer
#include <webGraphLib.h>

const char* ssid     = "****";
const char* password = "****";

AsyncWebServer Web(80);

void setup() {
  Serial.begin(115200);
  connectToWiFi();

// first make a start point
  float startX, startY = 50.0;
  point *myStartPoint = new point(startX, startY);

//2nd make  a line include the start point
  line *myLine = new line(myStartPoint);

//3rd make a new graph and add the line
  graph *simpleXYGraph = new graph();
  simpleXYGraph->addLine(myLine);

//final step  make a webGraph include web server object and graph object
  webGraph *mywebGraph = new webGraph(&Web, simpleXYGraph);

// then start server.
  mywebGraph->begin();

// optional etc setting. if you like.
  myLine->setLineColor("orange");
  simpleXYGraph->setBackgroundColor("darkgreen");//https://www.colordic.org/
  simpleXYGraph->setGrid(20, 10);
  mywebGraph->webRefreshRate(3);


  Serial.println("\nYou can now access graph to http://" + WiFi.localIP().toString());
  while (1) {
    myLine->addPoint(randomX(), randomY()); //max 255 points are retained.
    mywebGraph->setDirty(true); 
    delay(100);
    if (millis() / 10000 % 20 > 10) {
      simpleXYGraph->setBackgroundColor("darkgreen");
    } else {
      simpleXYGraph->setBackgroundColor("midnightblue");
	//https://www.colordic.org/
    }
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