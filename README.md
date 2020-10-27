# Web Graph library
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLib is a simple graphic library to make a web user interface for your Arduino projects using the  the **ESP32** . It uses web sockets and lets you create and update dynamic grapical elements on your GUI through multiple browser devices.

This Library runs on any kind  **generic ESP32 board** .
![ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 
##  Dependency
This Library depends on 2 famous ESP32 libraries.
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [ArdunoJson](https://github.com/bblanchon/ArduinoJson)

##  Provided Functions
- Displays such as sensor sequence data in a color graph.
- Displays JSON data comming from MQTT Broker in a color graph.
- The graph is constantly updated with newly incoming data.
- Auto-scaling feature(sensing maxmum and minimum floating point data).
- Graphics Web server functions by ESPAsyncWebServer.
- Easily Customizing graph view as you like.

##  Install 

download zip file.
![download zip file](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


import zip library.
![import zip library](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/EzipLibimport.png)



## Getting started with sample arduino schetch
- simpleGraph   :  adding random point to Graph continusly. 
- multiGraph    :  two graphs in one window sample
- inputJson     :  Importing Json String  
- MQTTGpaph     :  connect to MQTT broker "test.mosquitto.org" and getting Json data. depend on library [PubSubClient](https://github.com/knolleary/pubsubclient)
- windBlows     :  graph range set Fixed.  


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)


 ## using 4 objects
    - webGraph object
    - graph object
    - line  object
    - point object
    
     [JSON import ArduinoJson](#json)
 
###Set up Web graph
```cpp
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

// then start webGaph server.
  mywebGraph->begin();

```
