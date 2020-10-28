# Web Graph library
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLib is a simple graphic library to make a web user interface for your Arduino projects using the **ESP32** . It uses web sockets and lets you create and update dynamic graphical elements on your GUI through multiple browser devices.

This Library runs on any kind  **generic ESP32 board** .
![ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 

This Library also needs [Arduino core for the ESP32](https://github.com/espressif/arduino-esp32#installation-instructions)
##  Dependency
This Library depends on 2 famous ESP32 libraries.
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

##  Provided Functions
- Displays such as sensor sequence data into a color graph.
- Displays JSON data coming from MQTT Broker into a color graph.
- The graph is constantly updated with newly incoming data.
- Auto-scaling feature(sensing maximum and minimum floating point data).
- Graphics Web server functions by ESPAsyncWebServer.
- Easily Customizing graph view as you like.

##  Install 

### download zip file.

![download zip file](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### import zip library.

![import zip library](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/EzipLibimport.png)



## Getting started with arduino IDE 
- simpleGraph.ino  :  converting analogRead data into Graph lines continuously. 
- multiGraph.ino   :  two graphs in one window 
- inputJson.ino    :  Importing Json String  into multi lines.
- MQTTGpaphino     :  connect to MQTT broker "test.mosquitto.org" and getting Json data. depend on library [PubSubClient](https://github.com/knolleary/pubsubclient)
- windBlows     :  setting graph range set Fixed.  default is auto-range.


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)

### after open a sample sketch "windBlows",
please change Wi-Fi setting to yours
```cpp

const char* ssid     = "****";
const char* password = "****";

```
### after upload sample sketch
to esp32 board, wait for "You can now access graph to http://<ipaddress>" on serial monitor. You will be able to access tohttp://<ipaddress> through web browser.
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)

## using 4 objects
### webGraph object coordinate system
    
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/webGraphOject.png)
webGraph object is the top level object of this library. This retains webServr pointer and default 1 and max 3 child graphs in this object. max graphs are defined in header file <webGraphLib.h>

const unsigned int _MAXGRAPHS_IN_A_WEBGRAPH_ = 3;

webGraph Xsize and Ysize are automatically determined by child graphs size and its count. Default setting xsize=460 and ysize=260. coordinate system is left upper is origin(0,0) right lower (460,260).

### You can use webGraph by using following member functions
```cpp
/// constructor and destructor
    webGraph(AsyncWebServer *myServer);
    webGraph(AsyncWebServer *myServer, graph *g);
    ~webGraph();
    
//Setting member functions    
    void begin();
    webGraph *addGraph(graph *g);
    boolean setDirty(boolean dirty);
    void webRefreshRate(time_t refreshSecond = 600);
    void XvalueString(String graphName, String lineName, callback_with_arg_float myXfunc);
    void YvalueString(String graphName, String lineName, callback_with_arg_float myYfunc);
    void setBackgroundColor(String color);
    graph *importJson(String graphName, String json, String xKey, String yKey1[_MAX_LINES_IN_A_GRAPH_], uint8_t actualYKeys);

//getting member functions
    float _sizeX();
    float _sizeY();
    graph *searchGraphName(String nameTofind);
    String response();
    String print();
    ArRequestHandlerFunction onRequestResponse();
```
    - graph object coordinate system

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/graphOject.png)

    - line  object coordinate system
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/lineOject.png)

    - point object coordinate system is same as line object coordinate system
    
     [JSON import ArduinoJson](#json)

