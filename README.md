# Web Graph library
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLib is a simple graphic library to make a web user interface for your Arduino projects using the  the **ESP32** . It uses web sockets and lets you create and update dynamic grapical elements on your GUI through multiple browser devices.

This Library runs on any kind  **generic ESP32 board** .
![ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 

This Library also needs [Arduino core for the ESP32](https://github.com/espressif/arduino-esp32#installation-instructions)
##  Dependency
This Library depends on 2 famous ESP32 libraries.
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [ArdunoJson](https://github.com/bblanchon/ArduinoJson)

##  Provided Functions
- Displays such as sensor sequence data into a color graph.
- Displays JSON data comming from MQTT Broker into a color graph.
- The graph is constantly updated with newly incoming data.
- Auto-scaling feature(sensing maxmum and minimum floating point data).
- Graphics Web server functions by ESPAsyncWebServer.
- Easily Customizing graph view as you like.

##  Install 

### download zip file.

![download zip file](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### import zip library.

![import zip library](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/EzipLibimport.png)



## Getting started with arduino IDE 
- simpleGraph.ino  :  converting analogRead data into Graph lines continously. 
- multiGraph.ino   :  two graphs in one window 
- inputJson.ino    :  Importing Json String  into multi lines.
- MQTTGpaphino     :  connect to MQTT broker "test.mosquitto.org" and getting Json data. depend on library [PubSubClient](https://github.com/knolleary/pubsubclient)
- windBlows     :  setting graph range set Fixed.  default is auto-range.


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)

### after open a sample schetch,
please change wifi setting to yours
```cpp

const char* ssid     = "****";
const char* password = "****";

```
### after upload sample schetch
to esp32 board, wait for "You can now access graph to http://<ipaddress>" on serial monitor. You will be able to access tohttp://<ipaddress> through web browser.
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)


 ## using 4 objects
    - webGraph object
    - graph object
    - line  object
    - point object
    
     [JSON import ArduinoJson](#json)
 
