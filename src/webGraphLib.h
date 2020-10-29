#include <ESPAsyncWebServer.h>

#ifndef _webgraphlibl_h
#define _webgraphlibl_h

const unsigned int _MAX_POINTS_IN_A_LINE_ = 255;
const unsigned int _MAX_LINES_IN_A_GRAPH_ = 5;
const unsigned int _MAXGRAPHS_IN_A_WEBGRAPH_ = 3;
#define FLT_MAX 3.402823e+38

#define WEBGRAPHLIB_VERSION_MAJOR 0
#define WEBGRAPHLIB_VERSION_MINOR 83


class point;
class line;
class graph;
class webGraph;

class point {
  protected:
    float _x;
    float _y;
  public:
    point *_next = 0;
    point(float x, float y);

    float x() {
      return _x;
    }
    float y() {
      return _y;
    }
    unsigned int _count();
    int removePoint(point *p);
    void removeAllPoints();
    String response();
    String response( float midY, float offsetX, float offsetY, float ratioX, float  ratioY);
    String print();
};
typedef String (*callback_with_arg_float)(float);

class line   {
  protected:
    point *_head = 0;
    boolean _scaleFix = false;
    float _maxX = -FLT_MAX;
    float _maxY = -FLT_MAX;
    float _minX = FLT_MAX;
    float _minY = FLT_MAX;
    uint8_t _numberOfSplitCellsX = 24;
    uint8_t _numberOfSplitCellsY = 2;
    String _lineString;
    String _lineNameY;
    String _lineNameX;
    String _LineType = "L";
    String _lineColor = "";
    String _lineWidth = "0.5";
    float  _tA=0.0;
  public:

    line(point *toSet);
    line(float xPoint,float yPoint);
    ~line();
    
    line *_next = 0;
    boolean _dirty = true;
    boolean setDirty(boolean dirty);
    unsigned int _count();
    line *addPoint(point *pointList);
    line *addPoint(float x, float y);
    int addPointString(String XcommaYspace);
    int removeLine(line *toRemove);
    void removeAllLines();
    callback_with_arg_float userDefLineScaleX = 0;
    callback_with_arg_float userDefLineScaleY = 0;
    void setLineRangeXY(float minX, float maxX, float minY, float maxY);
    void resetLineRangeXY();
    void setGrid(uint8_t cellsXSplit = 24, uint8_t cellsYSplit = 8);
    void XvalueString(callback_with_arg_float myXFunction);
    void YvalueString(callback_with_arg_float myYFunction);
    String response(int lineIndex, int lineNumbers, graph* parent);
    unsigned int scan();
    String print();
    String getLineName() {
      return _lineNameY;
    }
    String getLineNameX(){
      return _lineNameX;
    }
    void   setLineName(String nameLineToSet) {
      _lineNameY = nameLineToSet;
    }
    void   setLineName(String nameLineToSet,String nameX) {
      _lineNameY = nameLineToSet;
      _lineNameX = nameX;
    }
    void setLineColor(String color) {
      _lineColor = color;
    }
    void setLineType(String lineType = "polyline");
    float getMaxX();
    float getMaxY();
    float getMinX();
    float getMinY();
    String _generateGrid(int lineIndex, int lineNumbers, graph *parent);
    void setXvalueStringAngle(float angle);
    uint32_t memory();
};

class graph  {
  protected:
    line *_head;
    String _graphString;
    String _graphName = "noName";
    String _bgColor = "#000000";
    float _maxX = -FLT_MAX;
    float _maxY = -FLT_MAX;
    float _minX = FLT_MAX;
    float _minY = FLT_MAX;
    uint8_t _numberOfSplitCellsX = 24;
    uint8_t _numberOfSplitCellsY = 8;
    String _generateGrid();
    float _sizeX = 400.0;
    float _sizeY = 200.0;
    float  _tA=0.0;

  public:
    graph *_next;
    boolean _dirty = true;
    boolean setDirty(boolean dirty);
    graph(line *toSet);
    graph();
    ~graph();
    float getSizeX() {
      return _sizeX;
    }
    float getSizeY() {
      return _sizeY;
    }
    float setSizeX(float x) {
      return _sizeX = constrain(x, 100, 1000);
    }
    float setSizeY(float y) {
      return _sizeY = constrain(y, 100, 1000);
    }
    void setSizeXY(float x, float y) {
      _sizeX = constrain(x, 100, 1000);
      _sizeY = constrain(y, 100, 1000);
    }
    unsigned int _count();
    graph *addLine(line *lineToAdd);
    void setName(String graphTitle);
    graph *importJson(String json, String xKey, String yKey1[_MAX_LINES_IN_A_GRAPH_], uint8_t actualKeys);
    line *searchLineName(String nameTofind);
    void removeAllGraphs();
    int removeGraph(graph *toRemove);
    String response(int graphCount,float topPosition);
    String print();
    void setGraphName(String nameOfGraph);
    void setGrid(uint8_t cellsXSplit = 24, uint8_t cellsYSplit = 8);
    String getGraphName();
    void setBackgroundColor(String ColorNumber);
    unsigned int scan();
    void XvalueString(String xKey, callback_with_arg_float myXfunc);
    void YvalueString(String yKey, callback_with_arg_float myYfunc);
    void XYvalueString(String xKey, callback_with_arg_float stdDispX, String yKeys, callback_with_arg_float stdDispTmp);
    void setXvalueStringAngle(float angle);
    uint32_t memory();
};

class webGraph {
  protected:
    AsyncWebServer *_graphWebserver;
    graph *_head;
    volatile boolean _dirty = true;
    String _webGraphString;
    time_t _refreshSecond = 600;
    String _panelColor = "#DDDDDD";
    volatile boolean _buisy = false;

  public:
    float _sizeX();
    float _sizeY();
    float _sizeY(int count);
    webGraph(AsyncWebServer *myServer);
    webGraph(AsyncWebServer *myServer, graph *g);
    ~webGraph();
    webGraph *addGraph(graph *g);
    void begin();
    void setBackgroundColor(String color){
		_panelColor=color;
	}
    graph *searchGraphName(String nameTofind);
    graph *importJson(String graphName, String json, \
                      String xKey, String yKey1[_MAX_LINES_IN_A_GRAPH_], uint8_t actualKeys);
    String response();
    String response(String graphName);
    String print();
    ArRequestHandlerFunction onRequestResponse();
    void webRefreshRate(time_t refreshSecond = 600);
    boolean setDirty(boolean dirty);
    void XvalueString(String graphName, String lineName, callback_with_arg_float myXfunc);
    void YvalueString(String graphName, String lineName, callback_with_arg_float myYfunc);
    uint32_t memory();
};


const float _MAXFLOTPOINT_  = 3.402823e+38;
const float _MINFLOATPOINT_ = 1.175494e-38;

const String colorList[_MAX_LINES_IN_A_GRAPH_ ] = {"\"#00FF7F\"", "\"#48D1CC\"", "\"#9ACD32\"", "\"#6A5ACD\"", "\"#556B2F\""};

const String pathString = "\n<path id=\"\" fill=\"none\" stroke=\"#00FF7F\" stroke-width=\"0.5\" d=\"M  ";
const String pathString2 = "\" />";
const String textGeneral = "<text fill=\"#00FF7F\" font-family=\"sans-serif\" x=\"0\" y=\"0\" font-size=\"6\">"\
                           "</text>";

const String wbase1 = "\n<html><head><meta charset=\"UTF-8\">\n <meta name=\"viewport\" content=\"width=device-width\">"\
                      "<meta http-equiv=\"refresh\" content=\"600; URL=http://111.222.333.444/\"></head>"\
                      "<body><svg xmlns=\"http://www.w3.org/2000/svg\"  xmlns:xlink=\"http://www.w3.org/1999/xlink\"version=\"1.1\""\
                      "width = \"460.00\" height=\"490.00\">"\
                      "<rect  x=\"0.00\" y=\"0.00\" fill=\"#DDDDDD\"\" stroke=\"#F8F8F8\" stroke-width=\"0.4\" "\
                      "width=\"480.00\" height=\"480.00\"></rect>";

const String wbase2 = "\n</svg></body></html>";

const String base1 = "\n<g transform=\"translate(30.00,30.00)\">"\
                     "<g transform=\"scale(1.00000000,1.00000000 )\">"\
                     "<g transform=\"translate(0 , 0 )\">"\
                     "\n<text transform=\"matrix(1 0 0 1 38.00 -2.00)\" font-size=\"12\" ><!Title></text>"\
                     "\n<rect  x=\"00.00\" y=\"00.00\" fill=\"#000000\"\" stroke=\"#000000\" stroke-width=\"0.4\" "\
                     "width=\"400.0\" height=\"200.0\"></rect>";

const String base2 = "<path  fill=\"none\" stroke=\"#DDDDDD\" stroke-width=\"0.2\" d=\"<!REPLACE_ME>\"></path>"\
                     "</g></g></g>";

const String lineString = "<polyline fill=\"none\" stroke=\"#00FF7F\" stroke-width=\"0.5\" points=\" \"></polyline>";
const String graphScal = "<text font-family=\"sans-serif\" text-anchor=\"middle\" transform=\"matrix(<!replace me>)\" font-size=\"6\" ></text>";
#endif
