# web Graph library
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLib is a simple graphic library to make a web user interface for your projects using the  the **ESP32** . It uses web sockets and lets you create and update grapical elements on your GUI through multiple browser devices.

This Library runs on any kind  **ESP32** (genral ESP32 board)


## Functions to be provided
- Easily display sensor data in a color graph.
- Display JSON data from the MQTT Broker in a graph.
- The graph is constantly updated with newly incoming data.
- Auto-scaling feature
- Graphics Web server functions
- Customizing graph view.

  - [Table of contents](##table-of-contents)
  - [Installation]
    - [download zip  file]
  - [input and output](#important-things-to-remember)
    [input] json string
    [output] <html> <svg> graph 
  - [sample svg graph](###Set up Web graph)
  - [using 4 objects]()
    - [webGraph object]()
    - [graph object](#get-post-and-file-parameters)
    - [line  object](#file-upload-handling)
    - [point object](#body-data-handling)
    - [JSON import ArduinoJson](#json)
 
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
