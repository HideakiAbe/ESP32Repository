# Web Graph library
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLib is a simple graphics library to make a web user interface for your Arduino projects using the **ESP32** . It uses web sockets and lets you create and update dynamic graphical elements on your GUI through multiple browser devices.

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
- The graph is constantly updated with new incoming data.
- Auto-scaling feature(sensing maximum and minimum floating-point data).
- Graphics Web server functions by ESPAsyncWebServer.
- Easily Customizing graph view as you like.

![provideFunctionAbstruct](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/provideFunctionAbstruct.png)

##  Install 

### download zip file.

![download zip file](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### import zip library.

![import zip library](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/EzipLibimport.png)



## Getting started with Arduino IDE 
- simpleGraph.ino  :  converting analogRead data into Graph lines continuously. 
- multiGraph.ino   :  two graphs in one window 
- inputJson.ino    :  Importing JSON String into multi-lines.
- MQTTGpaph.ino     :  connect to MQTT broker "test.mosquitto.org" and getting JSON data. depend on library [PubSubClient](https://github.com/knolleary/pubsubclient)
- windBlows     :  setting graph range set Fixed.  default is auto-range.


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)

### after open a sample sketch "windBlows",
please change the Wi-Fi setting to yours

```cpp

const char* ssid     = "****";
const char* password = "****";
```

### after upload sample sketch
to esp32 board, wait for "You can now access graph to http://<ipaddress>" on the serial monitor. You will be able to access http://<ipaddress> through a web browser.
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)

## using 4 objects
### webGraph object coordinate system
    
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/webGraphOject.png)
webGraph Xsize and Ysize are automatically determined by child graphs size and its count. Default setting xsize=460 and ysize=260. at the coordinate system , left upper is origin(0,0) right lower is (460,260). You can observe size by use of _sizeX() and _sizeY() member functions. But webGraph size cannot be set by the user directly.


![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/objectLevel.png)

The webGraph object is the top-level object of this library. This webGraph retains a web server pointer and the default 1 or max 3 child graphs in this object. Max graphs number are defined in header file <webGraphLib.h>


```cpp
const unsigned int _MAX_POINTS_IN_A_LINE_ = 255;
const unsigned int _MAX_LINES_IN_A_GRAPH_ = 5;
const unsigned int _MAXGRAPHS_IN_A_WEBGRAPH_ = 3;

```

### You can use webGraph by following  functions
```cpp
/// webGraph object constructor must have a parameter of AsyncWebServer pointer.
    webGraph(AsyncWebServer *myServer);
    webGraph(AsyncWebServer *myServer, graph *g);
    ~webGraph();
    
//Setting member functions    
    
    void begin();  
    //start web response, if http request is coming from browser.(tested chrome  86.0.4240.111)
    
    webGraph *addGraph(graph *g);      
    //add graph object to webGraph
    //If you add 4 or more graphs to webGrap, 
    //old graphs are removed. webGraph does retain only  upto 3 graphs. 
    
    boolean setDirty(boolean dirty);   
    // if you changed any child object under this webGraph ,you should set true value.

    void webRefreshRate(time_t refreshSecond = 600); 
    // inform web beowser to refresh period.
    
    void XvalueString(String graphName, String lineName, callback_with_arg_float myXfunc); 
    //resist X value display function to named graph
    
    void YvalueString(String graphName, String lineName, callback_with_arg_float myYfunc);
    //resist X value display function to named graph
    
    void setBackgroundColor(String color);
    // setting background color (default color ="#dddddd")
    
    graph *importJson(String graphName, String json, String xKey, String yKey1[_MAX_LINES_IN_A_GRAPH_], uint8_t actualYKeys);
    // import json string data into specified graph.
    // xKey is one of the JSON keys, it will be used as the x-value of each point.
    // ykeys[] is an array of JSON key. it will be used as y-value of each point.
    // actualYKeys is number of elements of ykeys[]. 

//getting member functions
    float _sizeX();
    float _sizeY();
    // return webGraph x,y size
    
    graph *searchGraphName(String nameTofind);
    // if named graph exsists in this webGraph, it will return graph pointer
    // if "nameTofind" graph is not exists, return null pointer.
    
    String response();
    // "<html><svg>" String will be return, when AsyncWebServer requests the response.
    
    String print();
    // print out the webGraph and its child object retaind data.
    // It will be useful to debug your project.

```
### graph object coordinate system

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/graphOject.png)

graph object has its own coordinate system, origin(0,0) is left upper corner. the right lower default value is (400,200).
graph coordinate system is different from webGraph coordinate system.
if webGraph retains three child graphs, three graphs have each different coordinate systems.
you can change the size by using three functions setSizeX(float x), setSizeY(float y) ,setSizeXY(float x, float y).
minimum x and y size are both 100, maximum size is 1000.
graph's origin(0,0) and  webGrahp(30,30) is same position.
if two graphs exsits in a webGraph, 2nd graph's origin(0,0) is  same postion webGraph (30, 60+ 1st ysize) .
Each graph is aligned vertically with an interval of 30.

### You can use webGraph by following functions

```cpp
    //constructor and remover from link
    
    graph(line *toSet);
    // genarate graph include specified line.
    
    graph();
    // genarate graph include no line.
    
    void removeAllGraphs(); 
    //remove all graphs links to this graph ,allocated memory are returned to free area.
    
    int removeGraph(graph *toRemove);
    // remove specified graph. allocated memory are returned to free area.
    

    
    graph *addLine(line *lineToAdd);
    // add line to this object
    //If you add 6 or more lines to this graph, old lines are removed. graph does retain only  upto 5 lines. 
    
    void setName(String nameOfGraph);
    void setGraphName(String nameOfGraph);
    //setting the name to this graph.
    //This name is displayed at top of this graph as title
    //you can search this graph  by this name from upper object.  see webGraph::searchGraphName(String nameTofind);
    
    float setSizeX(float x);
    float setSizeY(float y) ;
    void setSizeXY(float x, float y);
    //setting graph size
    
    void setBackgroundColor(String ColorNumber);
    //setting background color "#000000"~"#FFFFFF" also you can use "black","white",... 
    
    graph *importJson(String json, String xKey, String yKeys[_MAX_LINES_IN_A_GRAPH_], uint8_t actualKeys);
   /*
    import json string , for exsample  { "time" : 110 ,"volt" : 3.4  ,"current" : 1.2}
    xKey : specify  one of json' key, which  used as x value. for exsample "time"
    yKeys : specify one or max 5 json key arrey which used as y values ,for exsample {"volt","current"}
    actualKeys : please set number of ykeys arrey elements. in this case 2.
    if the graph has no line which named ykeys, new lines are automatically adding, which lines are named yKeys . 
    after adding new line, then add new point . 
    in this case  point( 110 , 3.4 ) are added to line name of "volt", point( 100 , 1.2) are added to line name "current"
    if the graph already has the line name of ykeys, no new lines are added. only x value and y values are added as new points.
    Each yKey is used as each line name.  you can get line pointer by using searchLineName(ykeys[0]) .
    xKey is also retained in eache line. 
    In this case line name "volt" has "time" as x key, line name "current" has also "time" as x key
   
    */

    void setGrid(uint8_t cellsXSplit = 24, uint8_t cellsYSplit = 8);
    // setting graph's grid line, 
    //"cellsXSplit" specify number of cells in the x coordinate.
    //"cellsYSplit" specify number of cells in the y coordinate.
    // default value of grid is (24,8)
    
    void XvalueString(String xKey, callback_with_arg_float myXfunc);
    void YvalueString(String yKey, callback_with_arg_float myYfunc);
    void XYvalueString(String xKey, callback_with_arg_float stdDispX, String yKeys, callback_with_arg_float stdDispTmp);
    // xKey speciy the name of line, myfunc is generic fnunction which argument is float type, return String type
    // for example        
    //   String myXfunc(float x){ return String(x)+"volt";}
    
    void setXvalueStringAngle(float angle);

    
    float getSizeX();
    float getSizeY();

    String response(int graphCount,float topPosition);
    String print();
    unsigned int _count();
    line *searchLineName(String nameTofind);
    String getGraphName();
    unsigned int scan();
    uint32_t memory();
    ```



graph object retains max 5 lines. 
    - line  object abd point object coordinate system
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/lineOject.png)

    - point object coordinate system is the same as the line object coordinate system
    
     [JSON import ArduinoJson](#json)

