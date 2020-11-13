# Webgraph Library
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLib is a simple graphics library for creating a web user interface for Arduino projects using **ESP32**, using web sockets to create a dynamic It allows you to create and update graphic elements.

This library works with common **ESP32 boards**.
![ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 

This library also requires the "Arduino Core for ESP32" (https://github.com/espressif/arduino-esp32#installation-instructions)
### Dependencies.
This library relies on two well-known ESP32 libraries.
- ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- ArduinoJson](https://github.com/bblanchon/ArduinoJson)
Please consider downloading it along with.

## Features Provided.
- Displays sensor sequence data and other data in a color graph.
- Displays JSON data sent from the MQTT Broker in a color graph.
- The graph is always updated with new data as it comes in.'
- Auto-scaling function (detects maximum and minimum values of floating point data).
- ESPAsyncWebServer for Graphics Web Server Functionality
- The contents of the graph can be easily customized, !

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/provideFunctionAbstruct.png)

## Installation. 

### Download the zip file.
![click here](https://github.com/HideakiAbe/ESP32Repository/archive/main.zip)

![download the zip file](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### Import the zip library.

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipLibimport.png)

## Let's use an example in the Arduino IDE 
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
Connect to the esp32 board and wait for the serial monitor to say "You can now access the graph to http://ipaddress" from your web browser. http://ipaddressにアクセスできるようになります.
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)

## What are the four objects?
- WebGrap object
- Graph object
- line object
- point object
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/allOject.png)


### webGraph object coordinate system description
    
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/webGraphOject.png)

The Xsize and Ysize of webGraph are automatically determined by the size of the child graphs and their number. The default value is xsize=460, ysize=260. The size can be checked using the _sizeX() and _sizeY() member functions. However, the size of WebGraph cannot be set directly by the user.
Changes should be made from the child's object graph.

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/objectLevel.png)

The webGraph object is a top-level object in this library. The webGraph holds a web server pointer, and by default the object holds one or up to three child graphs. The maximum number of graphs is defined in the header file <webGraphLib.h>, like this A few changes are possible, but the value of the three multipliers from the heap area limit
240x4X3 = 2880
Please set up the library so that the amount of memory used does not exceed The memory() function is designed to keep the amount of memory used by all objects in the library below 40K bytes, even at a maximum of 2880 points.
function is designed to use less than 40K bytes of memory, even at the maximum usage.
```cpp
const unsigned int _MAX_POTS_IN_A_LINE_ = 240;
const unsigned int _MAX_LINES_IN_A_GRAPH_ = 4;
const unsigned int _MAXGRAPHS_IN_A_WEBGRAPH_ = 3;
```

## Easily create a graph
Let's take a look at the easiest way to create a graph using this library.
- Creating a webGraph
- Importing JSON text into webGraph

The following ten or so lines of simple instructions will allow you to create a graph of the analog voltage on pin 34 and pin 35.
Please add the steps to connect to wiFi, as they are omitted.
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
  

  String xkey("time");
  String ykeys[2] = {"volt34", "volt35"};
  int ykeyElements = 2;
  while (w->busy()); //Wait for import during browser access
  w->importJson("mygraph", jsonString, xkey, ykeys, ykeyElements);
  delay(100);
}
```
This is a description of the function importJson, which is used to create graphs.
- The first argument is a graph name as a string. If a graph with that name already exists in webGraph, the second argument, json data, is added to the graph.
- If it does not exist, we create a new graph with the specified name and add the second argument, json data, as a single point of data.
- The second argument is a String in json containing the data we want to make into a graph. In this example, time, volt34, and volt35 are set to their values as keys.
- The third argument is a json key of string type to be used on the x-axis. In this example, "time" is specified as the value for x-axis.
- The fourth argument specifies the json key to be used on the y-axis as a String type ** array**. In this example, the graph is divided into two parts, with the line of volt34 on top and the line of volt35 on the bottom.
Up to four keys can be specified in the array, in which case the array is divided into four parts. The array can have up to four keys, in which case it will be split into four separate lines, where ykeys is used as the name of each line.
- The fifth argument is the number of elements of the fourth array.


## Basic operations on objects
### Example of object creation
```cpp.
float x=0.0;
float y=analogRead(34);
point *p=new point(x,y); 
      //Make sure to specify the x,y coordinates of the point as a float.
line *L =new line(p); 
      //A line must have at least one point.
graph *g=new graph();
AsyncWebServer webServer;
webGraph *w=new webGraph (&webServer);
     //webGraph must be a pointer to AsyncWebServer.
```
### Example of adding a child object to a parent object
Add a point to line.
```cpp
line *new line(p);
point *q=new point(x,y);
//Make sure to add a new point each time.
L->addPoint(q);
L->addPoint(q); }
Add "line *L" to graph *g.
```cpp
g->addLine(L);

```
Add "graph *g" to webGraph *w.
```cpp
w->addGraph(g);
``(g)
To sum up, you can create them in the order from grandchildren to children, and from children to parents, and then add them to the list.

### object destruction
If you add more than the required number of child objects to the parent, the old objects are automatically discarded. This library is designed to keep the latest and maximum number of child objects. As such, there is usually little need to discard objects as they are always updated with the latest data.
If you dare to discard a specific object, you can do so with the following functions.
1, the type of disposal specified by the pointer to the object
```cpp.
_head->removePoint(pRemove);　
``(pRemove)
pRemove: Points that you want to remove from the link and discard
The _head is usually a pointer to the first (old) point and is managed by the _head(protected:) element of the parent line object.
The _head element is not public and is not easily erased by anyone other than the member functions. Other than the _head element is acceptable, but only newer objects will be deleted.　

The same applies to the following.
Deleting the specified line
```cpp.
_head->removeLine(Lremove);
```
Disposal of the specified graph
```cpp.
_head->removeGraph(gRemove)
```

2, the type that discards objects at once
```cpp.
_head->removeAllPoints(); _head->removeAllLines();
_head->removeAllLines();
_head->removeAllGraphs();
```
The disposal function for webGraph is ~webGrap().

## object naming.
The graph and line objects can be named. The name is very important for the operations on the object described below.　It is not possible to name a point object or a webGraph object.
```cpp
//Naming and naming the line object
line *L=new line(0.0,0.0);
L->setLineName(String nameLineToSet("volt"));
String lNameY= getLineName();

L->setLineNameX("time");
String lNmaeX =getLineNameX();
//To set both names at the same time, you can use
L->setLineName("volt", "time");
//The first argument is the line name and the second is the name of the X-axis. Note that the first argument is the line name and the second argument is the name of the x-axis.


//Naming and getting the name of the graph object
graph *g=new graph();
g->setGraphName("the graph");
String g->getGraphName();
```
## Search by object name
If the named object is set up as a child of the parent object, then
You can search the parent object by its child's name.
```cpp
//An example of the process of giving a child object a name
line *L =new line(p); 
L->setLineName("myLine");
graph *g =new graph();
g->addLine(L);
AsyncWebServer webServer;
webGraph *w=new webGraph (&webServer);
g->setGraphName("myGraph");
w->addGraph(g);
......
//Search by name
graph *lookFor=w->searchGraphName("myGraph");
line *look =lookFor->searchLineName("myLine");
```
If you only have the topmost webGraph *w in the object hierarchy, you can keep track of your children by name.
So it may be convenient to declare a webGraph object in the global variable area so that it can be used by all local functions.
Here is an example of such a thing.
```cpp
AsyncWebServer webServer;
webGraph *w=new webGraph (&webServer);
void setup(){
Using //w
}
void loop(){
Using //w
}
```
please. In this example, it is 2.

## Name setting
```cpp.
//Line name setting function
void line::setName(String nameLineToSet); // line name = line y-axis name
void line::setLineName(String nameLineToSet); //same functin as setName()
void line::setLineName(String nameLineToSet, String nameX);
void line::setLineNameX(String nameX);

//The function to get the line name.
String line::getLineName();
String line::getLineNameX();
```


## Setting the X and Y axis display function
I know there are a lot of requests to display scales on both axes of the graph, but
As it is a matter of preference how it is displayed, user-configurable display functions can be added to
It can be configured.
```cpp
//Directly specify the line and set the display function to its XY axis
void line::XvalueString(callback_with_arg_float myXFunction);
void line::YvalueString(callback_with_arg_float myYFunction);

//We find the line name contained in the graph by xName,yName and set the display function to that axis.
//If it doesn't exist, we add the line to the graph by its name.
void graph::XvalueString(String xName, callback_with_arg_float myXfunc);
void graph::YvalueString(String yName, callback_with_arg_float myYfunc);
void graph::XYvalueString(String xName, callback_with_arg_float myXfunc, String yName, callback_with_arg_float myYfunc);
void graph::setXvalueStringAngle(float angle);

Set the display function to the axis of the graph, //which is the line name in the webGraph, using the graph name and line name.
//If it doesn't exist, add a graph to the axis using its name.
void webGraph::XvalueString(String graphName, String lineName, callback_with_arg_float myXfunc);
void webGraph::YvalueString(String graphName, String lineName, callback_with_arg_float myYfunc);
```

- The X axis displays the coordinate value of X directly below the line of the graph object's grid. The coordinate axis coincides with the center of the left and right widths of the text.
- The y-axis is displayed in two places: the maximum and minimum value of each grid line. The font size is fixed at 6 points.
The display position is on the left side of the graph and the end of the string is on the left side of the graph.
- The minimum value line coincides with the baseline of the string.
- The vertical position of the string to the left of the maximum line is "hanging", but the position may be shifted depending on the browser.
- Use setXvalueStringAngle to rotate the string on the X axis of graph only. This is a positive clockwise rotation.
A positive value with a non-zero rotation angle will cause the string to be aligned with the coordinate system.
- You can only display a string of about 8 characters on a coordinate axis or gridline, so devise a display function to avoid a long string.

## Specify the color.
Although the color specification has a predetermined default color, the three objects can be changed to a different color.
Call the following function from the pointer of the object you want to change.
```cpp
void line::setLineColor(String color);
void graph::setBackgroundColor(String ColorNumber);
void webGraph::setBackgroundColor(String color);
```
## Add user text.
If you want to display a specific string anywhere apart from the coordinate axes, you can use
Call the following function from the pointer of the object to which you want to add the string.
The xy-coordinates should be specified in the object's own coordinate system.
```cpp
void line::addUserText(String userText,float x,float y);
void graph::addUserText(String userText,float x,float y);
void webGraph::addUserText(String userText,float x,float y);
boolean webGraph::addUserText(String objectName,String userText,float x,float y,uint8_t fontsize =6,float angle =0);
```
- Display a userText string in x,y coordinate. The coordinates must be specified in the coordinate system of each object.
- The fourth function that takes objectName as an argument looks for the name of a subordinate object and adds a string to it.
- This fourth function allows you to specify the font size and the rotation angle (clockwise is positive) of the string.
- If the rotation angle is zero, the baseline of the left and right center of the string will be the same as (x,y).
- When the rotation angle is positive, the first character will be the same as (x,y).
- The position of characters in the line object follows its coordinate system, so only characters that fall within the maximum and minimum range of the displayed point are displayed. Any user text that falls outside of this range will be deleted.
- Each object can hold up to 10 user texts, but if you add more than 10 user texts, they will be removed in the order of oldest to oldest.

## Changing grid lines.
```cpp
void line::setGrid(uint8_t cellsXSplit = 24, uint8_t cellsYSplit = 2);
void graph::setGrid(uint8_t cellsXSplit = 2); void line::setGrid(uint8_t cellsXSplit = 24, uint8_t cellsYSplit = 2);
```
- Grid lines can be set separately for both graph and line objects. The grid lines are displayed in both directions.
- On the x-axis, grid lines are drawn in 24 segments by default.
- On the y-axis of the line, the grid lines are split into two by default.
- The color of the line grid lines is the same as the line's color.
- The y-axis of the graph is split by a fault into eight grid lines, and the grid lines are drawn in the direction of the y-axis.
- You can't change the color of the grid lines in the graph with #DDDDDDDD at this time.　The only way to change the color is to edit the base2 string directly in line 290 of webGraph.h. If you really want to change the color, you can do so by editing the base2 string in line 290 of webGraph.h. I don't know what the side-effects of this are.



### Example of creating a webGraph object.
```cpp.
The constructor of the /// webGraph object must have a parameter for the AsyncWebServer pointer.
webGraph *w = new webGraph(&myServer);

// The constructor can be a constructor with a graph object already created.
graph *g=new graph();
webGraph *w = new webGraph(&myServer,g);
 ```
## How to use webGraph.
```cpp.
//Required in the usage of the member function
  w->begin();
// Start responding to the http request from the browser. 
w->begin(); // The code will not be published to the web until the //begin() is executed.
The ``begin()''
## If you want to add a graph to webGraph later, use the
pointer.
It is operated with the
```cpp
w->addGraph(g);
//The member function to add a graph object to webGraph.
//If you add four or more graphs to webGrap.
w->addGraph(g); //If you add more than four graphs to webGrap, webGraph will only hold up to three graphs.
```

## Example of creating a dynamic graph
```cpp.
loop(){
    String json=getJsonFromMQtt(); // Process to get the json string periodically, e.g., a subscription from MQTT
    while(w->busy()); //While responding to the web browser, wait for the object to change, //while responding to the browser, w->busy() returns ture.
                                  //w->busy() returns ture while responding to the browser, and returns faluse after the response. After the response, it returns a faluse.
    w->importJson("myGraph",json,xkey,ykeys,elements); //Add or change the object
}
```
For more information on importJson, see [Easily create a graph].

## Display dynamic graphs.
```cpp.
void webRefreshRate(time_t refreshSecond = 600);
// Inform the web browser of the refresh period.
```


