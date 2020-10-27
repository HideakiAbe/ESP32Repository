# ESP32Repository
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/doc/sampleGraph.png)
## Table of contents
- [webGraphLib](#eESP32Repository)
  - [Table of contents](#table-of-contents)
  - [Installation](#installation)
    - [download zip  file](#)
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