# Webgraph Library
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLib is a simple graphics library for creating a web user interface for Arduino projects using **ESP32**, using web sockets to create a dynamic It allows you to create and update graphic elements.

This library works with common **ESP32 boards**.
![ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 

This library also requires the "Arduino Core for ESP32" (https://github.com/espressif/arduino-esp32#installation-instructions)
### Dependency.
This library relies on two well-known ESP32 libraries.
- ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- ArduinoJson](https://github.com/bblanchon/ArduinoJson)
Please consider downloading it along with.

## Features Provided.
- Displays sensor sequence data and other data in a color graph.
- Displays JSON data sent from the MQTT Broker in a color graph.
- The graph is always updated with new data as it comes in.
- Auto-scaling function (detects maximum and minimum values of floating point data).
- ESPAsyncWebServer for Graphics Web Server Functionality.
- The contents of the graph can be easily customized.

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/provideFunctionAbstruct.png)

## Installation. 
![click here](https://github.com/HideakiAbe/ESP32Repository/archive/main.zip)
### Download the zip file.

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### Import the zip library.


## Let's use examples in the Arduino IDE! 
- simpleGraph.ino : Convert analog lead data into continuous graph lines.
- multiGraph.ino : Display two graphs in one window. 
- inputJson.ino : Import a JSON string into multiple lines.
- MQTTGpaph.ino : Connect to MQTT broker "test.mosquitto.org" to get JSON data.
- windBlows : Set the range of the graph.


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)

### Here is a sample sketch "windBlows" after opening it.
Change your Wi-Fi settings to suit your environment.

```cpp
const char* ssid = "****".
const char* password = "****".
```

### After uploading the sample sketches to the ESP32 board.
to the esp32 board and wait for the serial monitor to say "You can now access the graph to http://<ipaddress>". You will be able to access http://<ipaddress> from your web browser.
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)


## Using the four objects
Object Hierarchy

The four objects have a hierarchical structure, as shown in the figure.
A parent object can have more than one child object, but the maximum number of objects it can have is fixed for each.　If you add more than the maximum number of objects, the oldest objects are deleted in the order in which they are added.
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/objectLevel.png)

## Easily create a graph
Let's take a look at the easiest way to create a graph using this library.
- Creating a webGraph
- Importing JSON text into webGraph

The following ten or so lines of easy steps will allow you to create a graph of the analog voltage on pin 34 and pin 35.
Please add the WiFi connection as it is omitted.
```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer
#include <webGraphLib.h>
AsyncWebServer webServer(80);
webGraph *w = new webGraph (&webServer);
void setup() {
  Serial.begin(115200);
  //connect To wifi 
  w->begin();
}
void loop() {
  String jsonString = "{\"time\":" + String(millis()) + ",\"volt34\":" + 
  String(analogRead(34)) + ",\"volt35\":" + String(analogRead(35)) + "}";
  String xkey("time");
  String ykeys[2] = {"volt34", "volt35"};
  int ykeyElements = 2;
  while (w->busy()); //Wait for import during browser access
  w->importJson("mygraph", jsonString, xkey, ykeys, ykeyElements);
  delay(100);
}
```
This section describes the function importJson, which is used to create graphs.
- The first argument "mygraph" is a string name for the graph.If a "mygraph" exists in WebGraph, the second argument, json data, is added to the graph as a single point of data.
- If it does not exist, we create a new graph with the name "mygraph" and add the json data of the second argument as a single point of data.
- The second argument is a String in json containing the data we want to make into a graph.In our example, time, volt34, and volt35 are set to their values as keys.
- The third argument is a json key of string type to be used on the x-axis. In this example, "time" is specified as the value for x-axis.
- The fourth argument specifies the json key to be used on the y-axis as a String type ** array**. In this example, the graph is divided into two parts, with the line of volt34 on top and the line of volt35 on the bottom.
Up to four keys can be specified in the array, in which case the array is divided into four parts. The array can have up to four keys, in which case it will be split into four separate lines, where ykeys is used as the name of each line.
- The fifth argument should be the number of elements of the fourth array. In our example, the fifth argument is 2.


### webGraph object coordinate system description
![webGraphObject](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/webGraphOject.png)
Xsize and Ysize are automatically determined by the size of the child graphs and their number.
The Xsize and Ysize of webGraph are automatically determined by the size and number of child graphs. The default value is xsize=460, ysize=260. The size can be checked using the member functions.
```cpp
_sizeX(); _sizeY(); 
```
However, the size of WebGraph cannot be set directly by the user.
Changes should be made from the child's object graph.



The webGraph object is a top-level object in this library. The webGraph holds a web server pointer, and by default the object holds one or up to three child graphs. The maximum number of graphs is defined in the header file <webGraphLib.h>, like this You can make some changes, but please make sure that the value of the three multipliers does not exceed 240x4 X 3 = 2880 due to the limitation of the heap area.


```cpp
const unsigned int _MAX_POTS_IN_A_LINE_ = 240.
const unsigned int _MAX_LINES_IN_A_GRAPH_ = 4.
const unsigned int _MAXGRAPHS_IN_A_WEBGRAPH_ = 3.
```

### You can use webGraph with the following function.

The constructor of the webGraph object must have a parameter for the AsyncWebServer pointer.
```cpp
    webGraph(AsyncWebServer *myServer).
    webGraph(AsyncWebServer *myServer, graph *g).
    webGraph(AsyncWebServer *myServer, graph *g); ~webGraph().
    You can use a constructor that includes a graph object that you have already created.
```
## How to use the member function
### Begin webGraph sever
If it is a http request from a browser, returns a http response, which will not be published to the web until begin() is executed.
 ```cpp 
 AsyncWebServer myServer(80);
 webGraph *w=new webGraph(&myServer);
 ....
 w->begin();
```
### Add a graph object to webGraph.
If you add four or more graphs to webGrap,webGraph will only hold up to three graphs. most oldest one will be removed.
 ```cpp 
 AsyncWebServer myServer(80);
 webGraph *w=new webGraph(&myServer);
 graph *g=new graph();
 w->addGraph(graph *g);
```
### When modified the child object
 If you modified the child object under the webGraph, set it to a true value of this function. If you set it to a true value the changes will be reflected in the graph response. 
 
 ```cpp
  webGraph *w=new webGraph(&myServer);
  ... add or change objects to w.
  w->setDirty(true).   
  ```
### Notify the web browser of the refresh period
It is recommended minum value would be 5.
```cpp
int seconds=10;
w->webRefreshRate(seconds). 
```
### Set the axis display function to the specified graph and Line. 
Set the X-axis display function to the specified graph.
Set the Y-axis display function to the specified graph.
```cpp
AsyncWebServer myServer(80);
webGraph *w=new webGraph(&myServer);
line *L=line(1.0, 1.0);
L->setName("volt");
L->setLineNameX("time");
graph *g=new graph();
g->addLine(L);
g->setGraphName("myGraph");
w->addGraph(g);
w->XvalueString("myGraph","time",myXfunc);
w->YvalueString("myGraph","volt",myYfunc);
 ...
 String myXfunc(float time){ return String(uint32_t(time);}
 String myYfunc(float volt){ return String(volt*1000.)+"[mV]";}
```
### changing background Color
Set the background color (default color ="#dddddd")
```cpp
w->setBackgroundColor(String color);
```
 ### Import JSON String   
  
  graph *importJson(String graphName, String json, String xKey. 

```
