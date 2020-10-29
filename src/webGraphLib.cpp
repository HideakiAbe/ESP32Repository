#include "webGraphLib.h"
#include <ArduinoJson.h>
#include <WiFi.h>


#include <ESPAsyncWebServer.h>
template<class T> inline Print &operator <<(Print & obj, T arg) {
  obj.print(arg);
  return obj;
}

#define BREAK             {Serial << "\nBREAK at ["<<__func__<<":" << __LINE__<<"]"; while (!Serial.available());Serial.read();}
#define DEBUGSTOP         {Serial << "\nSTOP at ["<<__func__<<":" << __LINE__<<"]"; while (1);}
#define LINE              {Serial << "\n["<<__func__<<":" << __LINE__<<"]";}
#define DEBUGOUT(var)     {Serial << "\n[" << __func__ << ":" << __LINE__ << "]" << #var<<"=" << var;}
#define WATCH(var)        {Serial << "\n[" << __func__ << ":" << __LINE__ << "]"<< #var<<"=" << var;} while (!Serial.available());Serial.flush();
/*
  #else
  #define BREAK
  #define DEBUGSTOP
  #define LINE
  #define DEBUGOUT(var)
  #define WATCH(var)
  #endif
*/

String Stringf(String format, float x, float y) {
  char bufferX[512];
  if (format.length() > sizeof(bufferX) / 8) {
	DEBUGOUT(" stop for safty reason")
    DEBUGSTOP
  }
  sprintf(bufferX, format.c_str(), x, y);
  return String(bufferX);
}
//////
//// point class
//////

point::point(float x, float y) {
  _next = 0;
  _x = x;
  _y = y;
}
String gfp(float x) {
  char buff[32];
  sprintf(buff , "%g", x);
  return String(buff);
}
unsigned int point::_count() {
  int c = 0;
  point *h = this;
  while (h) {
    h = h->_next;
    c++;
  }
  return c;
}

void point::removeAllPoints() {
  //WATCH(String((uint32_t)this, HEX))
  if (this) {
    point  *p = this->_next;
    point *temp;
    while ( p  ) {
      temp = p->_next; //次のデータの参照を取っておく
      //WATCH(String((uint32_t)p, HEX))
      delete( p );
      p = temp;       //tempを次の処理対象に
    }
  }
}
int point::removePoint( point *removePoint) {
  point  *p = this;
  point *tmp;
  while (p && p->_next && p->_next != removePoint ) {
    p = p->_next;
  }
  //最後まで見つからなかった
  if ( p->_next == NULL ) {
    return 0;
  }
  //removePointのデータをリストから外す
  tmp = p->_next;
  p->_next = tmp->_next;
  delete( tmp );
  return 1;
}

String point::response() {
  char buff[48];
  String _pointString = "";
  point *p = this;
  if(p){
	  sprintf(buff , "%g , %g L", p->_x, p->_y);
  	_pointString += String(buff);
 }
  for (; p; p = p->_next) {
    sprintf(buff , "%g , %g ", p->_x, p->_y);
    _pointString += String(buff);
  }
  return _pointString;
}

String point::response(float midY, float offsetX, float offsetY, float ratioX, float  ratioY) {
  //DEBUGOUT(ESP.getFreeHeap())
  char buff[48];
  String toRet;
  point *p = this;
  if(p){ 
  	sprintf(buff , "%g , %g L", ratioX * (p->_x - offsetX), offsetY - ratioY * (p->_y - midY)  );
  	toRet += String(buff);
  }
  for (; p; p = p->_next) {
    sprintf(buff , "%g , %g ", ratioX * (p->_x - offsetX), offsetY - ratioY * (p->_y - midY) );
    toRet += String(buff)  ;
    //DEBUGOUT(toRet)
  }
  //DEBUGOUT(ESP.getFreeHeap())
  return toRet;
}

String point::print() {
  point *p = this;
  //  WATCH(String((uint32_t)p, HEX))
  String ret = "\n        this";
  while (p) {
    ret += "->point(x,y)=( " + gfp(p->_x) + " , " + gfp(p->_y) + " )";
    p = p->_next;
    //  WATCH(String((uint32_t)p, HEX))

  }
  return ret;
}
//////
//// line class
//////

String standarDisplayfunc( float xy) {
  return String((int)xy);
}

line::line(point *toSet) {
  //WATCH(toSet->print())
  _next = 0;
  if (toSet && this) {
    _head = toSet;
  }
  XvalueString(0);
  YvalueString(0);
}
line::line(float xPoint,float yPoint){
  point *p = new point(xPoint,yPoint);
  if (p && this) {
    _head = p;
  }
  XvalueString(0);
  YvalueString(0);
}

line::~line() {
  if (_head) {
    _head->removeAllPoints();
    _lineString = "";
    _dirty = true;
  }
}
unsigned int line::_count() {
  line *L = this;
  unsigned int c = 0;
  while (L) {
    L = L->_next;
    c++;
  }
  return c;
}

boolean line::setDirty(boolean dirty) {
  _dirty = dirty;
  if(_dirty){
	  _lineString="";
  }
  return _dirty;
}

unsigned int line::scan() {
  if (_scaleFix == false) {
    _maxX = _maxY = - _MAXFLOTPOINT_;
    _minX = _minY = _MAXFLOTPOINT_;
    unsigned int count = 0;
    //WATCH(String((uint32_t)this,HEX))
    //WATCH(String((uint32_t)_head,HEX))
    point *p = _head;
    for (; p; p = p->_next) {
      //WATCH(String((uint32_t)p,HEX))
      if (p->x() > _maxX) _maxX = p->x() ;
      if (p->y() > _maxY) _maxY = p->y() ;
      if (p->x() < _minX) _minX = p->x() ;
      if (p->y() < _minY) _minY = p->y() ;
      count ++;
    }
    //Serial.printf("\n_minX=%g  _minY=%g", _minX, _minY);
    //DEBUGOUT(_maxX)
    //DEBUGOUT(_maxY)
    //DEBUGOUT(_minX)
    //DEBUGOUT(_minY)
    //BREAK
    if(_maxX - _minX < _MINFLOATPOINT_*100.){
		_maxX+=_MINFLOATPOINT_*100.;
		_minX -=_MINFLOATPOINT_*100.;
	}
   if(_maxY - _minY < _MINFLOATPOINT_*100.){
		_maxY += _MINFLOATPOINT_*100.;
		_minY -= _MINFLOATPOINT_*100.;
	}
	
    return count;
  }
}
line *line::addPoint(float x, float y) {
  point *p = new point(x, y);
  if (p) {
//	LINE
    addPoint(p);
  }
}
line *line::addPoint(point *pointList) {
	//LINE
  if ( pointList && pointList != this->_head) {
    _dirty = true;
    line *L = this;
    point *p = _head;
    if (! _head) {
      _head = pointList;
      return this;
    }
    unsigned int thisCount = p->_count();
    unsigned int addCount = pointList->_count();
    unsigned int removelCount = 0;
    if (thisCount + addCount > _MAX_POINTS_IN_A_LINE_) {
      removelCount = thisCount + addCount - _MAX_POINTS_IN_A_LINE_;
      //WATCH(removelCount)
    }
    while ( p &&  p->_next) {
      p = p->_next;
    }
    p->_next = pointList;
	//LINE
    for (; removelCount; removelCount--) {
      if (_head) {
        point *nextChainTop = _head->_next;
        _head->removePoint(_head);
        delete(_head);
        _head = nextChainTop;
      }
    }
  }
  return this;
}
int line::addPointString(String XYs) {
  _dirty = true;
  if (XYs.indexOf(",") < 0) return 0;
  if (XYs.indexOf(" ") < 0) return 0;
  //DEBUGOUT(XYs)
  XYs.replace(" ,", ",");
  XYs.replace(", ", ",");
  XYs.replace("   ", " ");
  XYs.replace("  ", " ");
  //DEBUGOUT(XYs)
  int len = XYs.length();
  int count = 0;
  int iC = 0;
  int iS = XYs.indexOf(",") + 1;
  for ( ; iC < len && iS < len && iC >= 0 && iS >= 0;) {
    //"128 , 0.609641  256 , 0.611129  512 , 0.585868 "
    float xx = XYs.substring(iC, iS).toFloat();
    iC = XYs.indexOf(" ", iS) + 1;
    float yy = XYs.substring(iS, iC).toFloat();
    iS = XYs.indexOf(",", iC) + 1;
    addPoint(xx, yy);
    //DEBUGOUT(iC)
    count++;
    if (count > _MAX_POINTS_IN_A_LINE_) {
      break;
    }
  }
  return count;
}
int line::removeLine(line *toRemove) {
  line  *L = this;
  line *tmp;
  while (L && L->_next && L->_next != toRemove ) {
    L = L->_next;
  }
  //最後まで見つからなかった
  if (L->_next == NULL ) {
    return 0;
  }
  //removePointのデータをリストから外す
  tmp = L->_next;
  L->_next = tmp->_next;
  point *p = (point *)L;
  p->removeAllPoints();
  delete( tmp );
  _dirty = true;
  return 1;
}
void line::removeAllLines() {
  line  *L = this;
  //WATCH(String((uint32_t)L, HEX))
  //WATCH(String((uint32_t)L->_next, HEX))
  line *temp;
  while ( L  ) {
    temp = L->_next; //次のデータの参照を取っておく
    point *p = (point *)L;
    //WATCH(String((uint32_t)p, HEX))
    p->removeAllPoints();
    if (L != this) delete( L );
    L = temp;       //tempを次の処理対象に
  }
  _dirty = true;
}


String line::response(int lineIndex, int lineNumbers, graph *parent) {
  //LINE
  //if (_dirty) {
    //LINE
    _lineString = "";
    lineNumbers = constrain(lineNumbers, 1, _MAX_LINES_IN_A_GRAPH_);
    lineIndex = constrain(lineIndex, 0, lineNumbers - 1);
    this->scan();
    float xscale;
    float yscale;
    float offSetX = _minX;
    float harfY = parent->getSizeY() / lineNumbers / 2.0;
    float offSetY =  harfY * (2 * lineIndex + 1);
    float topY = offSetY - harfY;
    float diffX = (_maxX - _minX);
    float diffY = (_maxY - _minY);
    diffX < _MINFLOATPOINT_*100. ? diffX = _MINFLOATPOINT_*100.0 : diffX;
    diffY < _MINFLOATPOINT_*100. ? diffY = _MINFLOATPOINT_*100.0 : diffY;
    xscale = parent->getSizeX() / diffX;
    yscale = parent->getSizeY() / lineNumbers / diffY;
    float lineScale = 2.0;
    String chunkStroke = "stroke-width=\"0.5\"";
    String toReplaceStroke = "stroke-width=\"" + String(1.0 / lineScale) + "\"";
    String chunkColor = "\"#00FF7F\"";
    String toReplaceColor = colorList[lineIndex % _MAX_LINES_IN_A_GRAPH_];
    if (_lineColor.length()) {
      toReplaceColor = "\"" + _lineColor + "\"";
    }
    String pathStr = pathString;
    pathStr.replace(chunkStroke, toReplaceStroke);
    pathStr.replace(chunkColor, toReplaceColor);
    if (_lineNameY.length()) {
      pathStr.replace("id=\"\"", "id=\"" + _lineNameY + "\"");
    }
    line * L = this;
    _lineString += pathStr;
    if(L){
		point *p = L->_head;
     _lineString += p->response((_maxY + _minY) / 2.0, offSetX, offSetY, xscale, yscale);
    }
    //DEBUGOUT(_lineString)
    _lineString += pathString2;
    //LINE
    String txs = textGeneral;
    if (_lineNameY.length()) {
      txs.replace(chunkColor, toReplaceColor);
      txs.replace("x=\"0\" y=\"0\"", Stringf("x=\"%g\" y=\"%g\" text-anchor=\"middle\" alignment-baseline=\"hanging\" ", parent->getSizeX() / 2, topY));
      if (userDefLineScaleY) {
        txs.replace("</text>", _lineNameY +  "</text>");
      } else {
        txs.replace("</text>", _lineNameY + " = " + String((_maxY + _minY) / 2.0, 1) + "</text>");
      }
      txs += "\n<path stroke= " + toReplaceColor + "  stroke-width=\"0.5\" fill=\"none\" d=\"" + _generateGrid(lineIndex, lineNumbers, parent) + "\" />";
      //DEBUGOUT(txs)
      _lineString += txs;
    }
    txs=textGeneral;
    if(_lineNameX.length()){
      txs.replace(chunkColor, toReplaceColor);
      txs.replace("x=\"0\" y=\"0\"", Stringf("x=\"%g\" y=\"%g\" text-anchor=\"end\"", parent->getSizeX(), offSetY));
      txs.replace("</text>", _lineNameX +  "</text>");
      _lineString += txs;
    }
    //LINE
    if (userDefLineScaleY) {
      //LINE
      String gs = graphScal;
      gs.replace("<!replace me>", "1 0 0 1 0 " + String(offSetY + parent->getSizeY() / 2 / lineNumbers));
      //gs.replace("text-anchor=\"middle\"", "text-anchor=\"end\" alignment-baseline=\"baseline\" fill=\"" + toReplaceColor + "\"");
      gs.replace("text-anchor=\"middle\"", "text-anchor=\"end\"  fill=\"" + toReplaceColor + "\"");
      gs.replace("</text>", userDefLineScaleY(_minY) + "</text>");
      _lineString += gs;
      gs = graphScal;
      gs.replace("<!replace me>", "1 0 0 1 -1 " + String(offSetY - parent->getSizeY() / 2 / lineNumbers));
      gs.replace("text-anchor=\"middle\"", "text-anchor=\"end\" alignment-baseline=\"hanging\" fill=\"" + toReplaceColor + "\"" );
      gs.replace("</text>", userDefLineScaleY(_maxY) + "</text>");
      _lineString += gs;
      //DEBUGOUT(_maxY)
      //DEBUGOUT(userDefLineScaleY(_maxY))
    }
    //DEBUGOUT(_lineString)
  //}
  return _lineString;
}
String line::print() {
  line *L = this;
  String ret = "\n";
  while (L) {
    //BREAK
    L->scan();
    //BREAK
    if (_lineNameY.length()) {
      ret += "\n    this->line(" + _lineNameY + ")";
    } else {
      ret += "\n    this->line(" + String((uint32_t)L, HEX) + ")";
    }
    ret += " (maxX, maxY) = (" + String(L->_maxX) + ", " + String(L->_maxY) + ")";
    ret += " (minX, minY) = (" + String(L->_minX) + ", " + String(L->_minY) + ")";
    point *q = L->_head;
    ret += q->print();
    L = L->_next;
    //WATCH((uint32_t)L)
  }
  return ret;
}
void line::XvalueString(callback_with_arg_float myXFunction) {
  userDefLineScaleX = myXFunction;
}
void line::YvalueString(callback_with_arg_float myYFunction) {
  userDefLineScaleY = myYFunction;
}
String line::_generateGrid(int lineIndex, int lineNumbers, graph *parent) {
  String toGenerate = "";
  if (_numberOfSplitCellsX && lineNumbers) {
    float dx = parent->getSizeX() / _numberOfSplitCellsX;
    float dy = parent->getSizeY() / lineNumbers;
    float oy = dy * lineIndex ;
    for (uint8_t x = 0; x <= _numberOfSplitCellsX; x++) {
      toGenerate += "\nM" + String(dx * x, 1) + "," + String(oy, 1);
      toGenerate += "L" + String(dx * x, 1) + "," + String(oy + dy, 1);
    }
  }
  if (_numberOfSplitCellsY && lineNumbers) {
    float dx = parent->getSizeX();
    float dy = parent->getSizeY() / lineNumbers;
    float oy = dy * lineIndex ;
    dy /= _numberOfSplitCellsY;
    float cy = 0.0;
    for (uint8_t y = 0; y <= _numberOfSplitCellsY; y++) {
      cy = 0.0;
      if (y == 0) cy = 0.6;
      if (y == _numberOfSplitCellsY) cy = -0.6;
      toGenerate += "\nM" + String(0.0, 1) + "," + String(cy + oy + dy * y, 1);
      toGenerate += "L" + String(dx, 1) + "," + String(cy + oy + dy * y, 1);
    }
  }
  //WATCH(toGenerate)
  return toGenerate;
}
float line::getMaxX() {
  return _maxX;
}
float line::getMaxY() {
  return _maxY;
}
float line::getMinX() {
  return _minX;
}
float line::getMinY() {
  return _minY;
}
void line::setLineRangeXY(float minX, float maxX, float minY, float maxY) {
  _scaleFix = true;
  _minX = minX, _maxX = maxX, _minY = minY, _maxY = maxY;
}
void line::resetLineRangeXY() {
  _scaleFix = false;
}
void line::setGrid(uint8_t cellsXSplit, uint8_t cellsYSplit) {
  _numberOfSplitCellsX = constrain(cellsXSplit, 0, 128);
  _numberOfSplitCellsY = constrain(cellsYSplit, 0, 128);
}
void line::setXvalueStringAngle(float angle){
	_tA=constrain(angle,0.0,180.0);
	_tA=_tA*PI/180.;
}
uint32_t line::memory(){
	uint32_t memo= sizeof(line);
	memo +=_head->_count() * sizeof(point);
	memo += _lineString.length();
    memo += _lineNameY.length();
    memo += _lineNameX.length();
    memo += _LineType.length();
    memo += _lineColor.length();
    memo += _lineWidth.length();
    return memo;
}
//////
//// graph class
//////
graph::graph() {
  _next = 0;
  _head = 0;
}
graph::graph(line *toSet) {
  _next = 0;
  if (toSet && this) {
    _head = toSet;
  }
}
graph::~graph() {
  line *L = _head;
  if (L) {
    L->removeAllLines();
  }
  _graphString = "";
}

line *graph::searchLineName(String nameTofind) {
  line *L = _head;
  while (L) {
    if (nameTofind == L->getLineName()) {
      return L;
    }
    L = L->_next;
  }
  return 0;
}
#if ARDUINOJSON_VERSION_C == 5
#if ARDUINOJSON_VERSION_MINOR >=11
graph *graph::importJson(String json, String xKey, String yKeys[_MAX_LINES_IN_A_GRAPH_], uint8_t actualKeys) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  //LINE
  if (!root.success()) {
    Serial.println(F("Failed to read json string"));
    return 0;
  }
  _dirty = true;
  float x = root[xKey].as<float>();
  ////WATCH(x)
  float y[_MAX_LINES_IN_A_GRAPH_];
  for (int i = 0; i < actualKeys; i++) {
    y[i] = root[yKeys[i]].as<float>();
    ////WATCH(y[i])
    //WATCH(yKeys[i])
    line *L = searchLineName(yKeys[i]);
    //WATCH(String((uint32_t)L, HEX))
    point *p = new point(x, y[i]);
    if (!p) {
      return 0;
    }
    if (!L) {
      L = new line(p);
      graph::addLine(L);
      L->setLineName(yKeys[i], xKey);
    }
    //WATCH(String((uint32_t)L, HEX))
    if (!L) {
      return 0;
    }
    //LINE
    L->addPoint(p);
  }
  jsonBuffer.clear();
  //BREAK;
  //this->print();
  //BREAK;
  return this;
}
#else
#pragma message("please use ARDUINOJSON 5.11-5.13 or 6.9 above")
#endif
#endif
#if ARDUINOJSON_VERSION_MAJOR >= 6
#if ARDUINOJSON_VERSION_MINOR >= 9
graph *graph::importJson(String json, String xKey, String yKeys[_MAX_LINES_IN_A_GRAPH_], uint8_t actualKeys) {
  DynamicJsonDocument doc(1024);
  DeserializationError  error = deserializeJson(doc, json);
  //DEBUGOUT(json)
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return 0;
  }


  float x = doc[xKey].as<float>();
  //DEBUGOUT(x)
  float y[_MAX_LINES_IN_A_GRAPH_];
  for (int i = 0; i < actualKeys; i++) {
    y[i] = doc[yKeys[i]].as<float>();
  //DEBUGOUT(y[i])
    //WATCH(yKeys[i])
    line *L = searchLineName(yKeys[i]);
    point *p = new point(x, y[i]);
    if (!p) {
      return 0;
    }
    if (L) {
      L->addPoint(p);
    } else {
      L = new line(p);
      if (!L) {
        return 0;
      }
      addLine(L);
      L->setLineName(yKeys[i],xKey);
    }
  }
  doc.clear();
  return this;
}
#else
#pragma message("please use ARDUINOJSON 5.11-5.13 or 6.9 above")
#endif
#endif
boolean graph::setDirty(boolean dirty) {
  _dirty = dirty;
  if(_dirty){
	  _graphString="";
  }
  line *L=_head;
  while(L){
    L->setDirty(_dirty);
	L=L->_next;
  }
  return _dirty;
}

String graph::response(int graphCount,float topPosition) {
  //if (_dirty) {
    scan();
    //WATCH(_minX)
    line *L = _head;
    int lineNumbers = 0;
    for (; L; L = L->_next) {
      lineNumbers++;
    }
    //BREAK
    _graphString = base1;
    if(_graphName=="noName"){
     _graphString.replace("<!Title>", "");
    }else{
      _graphString.replace("<!Title>", _graphName);
	}
    _graphString.replace("translate(30.00,30.00", "translate(30.00," + String(topPosition));
    _graphString.replace("#000000", _bgColor);
    _graphString.replace("width=\"400.0\" height=\"200.0\"></rect>" , Stringf("width=\"%g\" height=\"%g\"></rect>", _sizeX, _sizeY));
    int colorCount = 0;
    boolean xScalalnotYet = true;
    for (L = _head; L; L = L->_next) {
      L->scan();
      _graphString += "\n <!line" + String((uint32_t)L, HEX) + ">";
      _graphString += L->response(colorCount, lineNumbers, this) ;
      _graphString += " <!line" + String((uint32_t)L, HEX) + ">";
      colorCount = (colorCount + 1) % _MAX_LINES_IN_A_GRAPH_;
      //BREAK
      if (L->userDefLineScaleY && xScalalnotYet) {
        String gs = graphScal;
        int i = 0;
        String xgs = ""; int ypos = 6;
        for (float xpos = 0.0; xpos <= _sizeX; xpos += _sizeX / _numberOfSplitCellsX) {
          gs = "\n" + graphScal;
           //DEBUGOUT(_tA)
          if(_tA != 0.0){
            gs.replace("<!replace me>",String(cos(_tA))+" "+String(sin(_tA))+ " "+String(-sin(_tA))+String(cos(_tA))+" "  + String(xpos) + " " + String(_sizeY + ypos));
            gs.replace("text-anchor=\"middle\"","text-anchor=\"start\"");
          }else{
            gs.replace("<!replace me>", String("1 0 0 1 ") + String(xpos) + " " + String(_sizeY + ypos));			
		  }
          L->scan();
          L->getMinX();
          //DEBUGOUT(L->getMinX())
          //DEBUGOUT(_minX)

          gs.replace("</text>", L->userDefLineScaleX(_minX + (_maxX - _minX) / _numberOfSplitCellsX * i) + "</text>");
          xgs += gs;
          i++;
          //WATCH(xgs)
        }
        _graphString += xgs;
        xScalalnotYet = false;
      }
    }
    _graphString += "\n" + base2;
    _graphString.replace("<!REPLACE_ME>", _generateGrid());
    _dirty = false;
  //}
  return _graphString;
}
graph *graph::addLine(line *lineToAdd) {
  if ( lineToAdd && lineToAdd != this->_head) {
    _dirty = true;
    graph *G = this;
    line *L = _head;
    if (! _head) {
      _head = lineToAdd;
      return this;
    }
    unsigned int thisCount = G->_count();
    unsigned int addCount = lineToAdd->_count();
    unsigned int removelCount = 0;
    if (thisCount + addCount > _MAX_POINTS_IN_A_LINE_) {
      removelCount = thisCount + addCount - _MAX_POINTS_IN_A_LINE_;
      //WATCH(removelCount)
    }
    while ( L &&  L->_next) {
      L = L->_next;
    }
    L->_next = lineToAdd;

    for (; removelCount; removelCount--) {
      if (_head) {
        line *nextChainTop = _head->_next;
        _head->removeLine(_head);
        delete(_head);
        _head = nextChainTop;
      }
    }
  }
  return this;
}

int graph::removeGraph(graph *toRemove) {
  graph  *G = this;
  graph *tmp;
  while (G && G->_next && G->_next != toRemove ) {
    G = G->_next;
  }
  //最後まで見つからなかった
  if (G->_next == NULL ) {
    return 0;
  }
  //removePointのデータをリストから外す
  tmp = G->_next;
  G->_next = tmp->_next;
  line *l = _head;
  l->removeAllLines();
  delete( tmp );
  _dirty = true;
  return 1;
}
unsigned int graph::_count() {
  graph *G = this;
  unsigned int c = 0;
  while (G) {
    G = G->_next;
    c++;
  }
  return c;
}
unsigned int graph::scan() {
  _maxX = _maxY = - _MAXFLOTPOINT_;
  _minX = _minY = _MAXFLOTPOINT_;
  line *L = _head;
  unsigned int c = 0;

  while (L) {
    //BREAK
    c += L->scan();
    //BREAK
    _maxX = max(L->getMaxX(), _maxX);
    _maxY = max(L->getMaxY(), _maxY);
    _minX = min(L->getMinX(), _minX);
    _minY = min(L->getMinY(), _minY);
    L = L->_next;
  }
  return c;
}
String graph::print() {
  graph *p = this;
  //BREAK
  scan();
  //BREAK
  String ret = "\nthis(" + _graphName + ")";
  ret += " (maxX, maxY) = (" + String(_maxX) + ", " + String(_maxY) + ")";
  ret += " (minX, minY) = (" + String(_minX) + ", " + String(_minY) + ")";
  while (p) {
    ret += "->graph(" + String((uint32_t)p, HEX) + ")";
    line *q = p->_head;
    //BREAK
    ret += q->print();
    p = p->_next;
    //BREAK
  }
  return ret;
}
void graph::setGraphName(String nameOfGraph) {
  _graphName = nameOfGraph;
}
String  graph::getGraphName() {
  return _graphName;
}
void graph::setBackgroundColor(String ColorNumber){
	_bgColor=ColorNumber;
}
String graph::_generateGrid() {

  String toGenerate = "";
  float dx = _sizeX / _numberOfSplitCellsX;
  float dy = _sizeY / _numberOfSplitCellsY;
  for (uint8_t x = 0; x < _numberOfSplitCellsX; x++) {
    toGenerate += "\nM" + String(dx * x, 1) + ", 0.0L" + String(dx * x, 1) + "," + String(_sizeY, 1);
  }
  for (uint8_t y = 0; y < _numberOfSplitCellsY; y++) {
    toGenerate += "\nM0.0, " + String(dy * y, 1) + ", L" + String(_sizeX, 1) + ","   + String(dy * y, 1);
  }
  //WATCH(toGenerate)
  return toGenerate;
}
void graph::setGrid(uint8_t cellsXSplit, uint8_t cellsYSplit) {
  _numberOfSplitCellsX = constrain(cellsXSplit, 0, _sizeX / 2);
  _numberOfSplitCellsY = constrain(cellsYSplit, 0, _sizeY / 2);
}

void graph::XvalueString(String lineName, callback_with_arg_float myXfunc) {
  line *L = _head;
  while (L) {
    if (L->getLineName() == lineName) {
      L->XvalueString(myXfunc);
      break;
    }
    L = L->_next;
  }
  if(L==0){
	line *NL=new line(new point(0,0));
	if(NL){
	  addLine(NL);
	  NL->XvalueString(myXfunc);
	  NL->setLineName(lineName);
	}
  }

}
void graph::YvalueString(String lineName, callback_with_arg_float myYfunc) {
  line *L = _head;
  while (L) {
    if (L->getLineName() == lineName) {
      L->YvalueString(myYfunc);
      break;
    }
    L = L->_next;
  }
  if(L==0){
	line *NL=new line(new point(0,0));
	if(NL){
	  addLine(NL);
	  NL->YvalueString(myYfunc);
	  NL->setLineName(lineName);
	}
  }
}
void graph::XYvalueString(String xKey, callback_with_arg_float stdDispX, String yKeys, callback_with_arg_float stdDispY) {
  line *L = _head;
  while (L) {
    if (L->getLineName() == yKeys) {
      L->XvalueString(stdDispX);
      L->YvalueString(stdDispY);
      break;
    }
    L = L->_next;
  }
  if(L==0){
	line *NL=new line(new point(0,0));
	if(NL){
	  addLine(NL);
	  NL->XvalueString(stdDispX);
	  NL->YvalueString(stdDispY);
	  NL->setLineName(yKeys);
	}
  }

}
void graph::setXvalueStringAngle(float angle){
	_tA=constrain(angle,0.0,180.0);
	_tA=_tA*PI/180.;
}
uint32_t graph::memory(){
  uint32_t memo=sizeof(graph);
  line *L=_head;
  while(L){
	memo += L->memory();
	L=L->_next;
  }
  memo += _graphString.length()+_graphName.length()+_bgColor.length();
  return memo;
}
	
//////
//// webGraph class
//////

webGraph::webGraph(AsyncWebServer *myServer) {
  _graphWebserver = myServer;
  _head = 0;
  String _webGraphString;
  if (_graphWebserver) {
    _graphWebserver->on("/", HTTP_GET, [this](AsyncWebServerRequest * request) {
      String message = this->response();
      request->send(200, "text/html", message);
    });
  }
}
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}


webGraph::webGraph(AsyncWebServer *myServer, graph *g) {
  _graphWebserver = myServer;
  _head = g;
  String _webGraphString;
  if (_graphWebserver) {
    _graphWebserver->on("/", HTTP_GET, [this](AsyncWebServerRequest * request) {
      request->send(200, "text/html", this->response());
    });
    _graphWebserver->onNotFound(notFound);
  }
}
void webGraph::begin() {
  if (_graphWebserver) {
    _graphWebserver->begin();
  }
}
webGraph::~webGraph() {
  if (_head) {
    _head->removeAllGraphs();
  }
  if (_graphWebserver) {
    _graphWebserver->end();
  }
}
webGraph *webGraph::addGraph(graph *graphToAdd) {
  if (graphToAdd && graphToAdd != this->_head) {
    _dirty = true;
    unsigned int thisCount = 0;
    unsigned int addCount = 0;
    unsigned int removeCount = 0;

    graph *G = _head;
    thisCount = _head->_count();
    addCount = graphToAdd->_count();
    if (thisCount + addCount > _MAXGRAPHS_IN_A_WEBGRAPH_) {
      removeCount = thisCount + addCount - _MAXGRAPHS_IN_A_WEBGRAPH_;
    }
    while ( G && G->_next) {
      G = G->_next;
    }
    if (!G)_head = graphToAdd;
    else G->_next = graphToAdd;
    for (; removeCount; removeCount--) {
      graph *chainLink = _head->_next;
      _head->removeGraph(_head);
      delete(_head);
      _head = chainLink;
    }
  }
  return this;
}
graph *webGraph::searchGraphName(String nameTofind) {
  graph *G = _head;
  while (G) {
    if (nameTofind == G->getGraphName()) {
      return G;
    }
    G = G->_next;
  }
  return 0;
}
graph *webGraph::importJson(String graphName, \
                            String json, \
                            String xKey, \
                            String yKey1[_MAX_LINES_IN_A_GRAPH_], \
                            uint8_t actualKeys) {
  //while(_buisy) ;
  //WATCH(sizeof(*yKey1))
  //LINE
  if (graphName.length() && graphName == "noName") {
    return 0;
  }
  _dirty=true;
  graph *G = searchGraphName(graphName);
  if (G) {
//	LINE
    G->importJson(json, xKey, yKey1, actualKeys);
  }
  return G;
}

String webGraph::response(String graphName) {
	_buisy=true;
  if (_dirty) {
    graph *G = searchGraphName(graphName);
    if (!G) {
      return String("");
    } else {
      _webGraphString = wbase1;
      _webGraphString.replace("URL = http: //111.222.333.444", "URL=http://" + WiFi.localIP().toString());
      _webGraphString.replace("content=\"600", "content=\"" + String(_refreshSecond));
      //DEBUGOUT(_sizeY())
      _webGraphString.replace("height=\"490.00", "height=\"" + String( _sizeY()));
      _webGraphString.replace("height=\"480.00", "height=\"" + String( _sizeY()));
      _webGraphString.replace("fill=\"#DDDDDD", "fill=\"" + _panelColor);
      int graphCount = 0;
      //WATCH(String((uint32_t)G, HEX))
      _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
      _webGraphString += G->response(graphCount,30) ;
      _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
      //WATCH(_webGraphString)
      _webGraphString += wbase2;
      DEBUGOUT(ESP.getFreeHeap())
      _dirty = false;
    }
  }
  _buisy=false;
  return _webGraphString;
}
float webGraph::_sizeX() {
  float maxX = 100;
  for (graph *G = _head; G; G = G->_next) {
    maxX = max(maxX, G->getSizeX());
    //WATCH(maxX)
  }
  return maxX;
}
float webGraph::_sizeY() {
  float maxY = 0;
  for (graph *G = _head; G; G = G->_next) {
    maxY += G->getSizeY()+60;
  }
  return maxY;
}
float webGraph::_sizeY(int count) {
  float maxY = 0;
  if(count == 0) maxY=30;
  for (graph *G = _head; G && count; G = G->_next) {
    maxY += G->getSizeY()+60;
    count--;
  }
  return maxY;
}

String webGraph::response() {
	//LINE
    _buisy=true;
    //if (_dirty) {
	  //LINE
      _webGraphString = wbase1;
      float tx = _sizeX() + 60;
      float ty = _sizeY()  ;
      //DEBUGOUT(ty)
      _webGraphString.replace("version=\"1.1\"width = \"460.00\" height=\"490.00\">", Stringf("version=\"1.1\"width = \"%g\" height=\"%g\">", tx, ty));
      _webGraphString.replace("URL=http://111.222.333.444", "URL=http://" + WiFi.localIP().toString());
      _webGraphString.replace("content=\"600", "content=\"" + String(_refreshSecond));
      _webGraphString.replace("width=\"480.00\" height=\"480.00\"", Stringf("width = \"%g\" height=\"%g\"", tx, ty));
      _webGraphString.replace("fill=\"#DDDDDD", "fill=\"" + _panelColor);

      graph * G;
      int graphCount = 0;
      for (G = _head; G; G = G->_next) {
        _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
        _webGraphString += G->response(graphCount,_sizeY(graphCount)) ;
        _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
        //WATCH(_webGraphString)
        graphCount = (graphCount + 1) % _MAXGRAPHS_IN_A_WEBGRAPH_;
      }
      _webGraphString += wbase2;
      _dirty = false;
    //}
    _buisy=false;
  return _webGraphString;
}
String webGraph::print() {
}
void webGraph::webRefreshRate(time_t refreshSecond) {
  _refreshSecond = constrain(refreshSecond, 10, 9999999);
}
void webGraph::XvalueString(String graphName , String lineName, callback_with_arg_float myXfunc) {
  graph *G = _head;
  while (G) {
    if (G->getGraphName() == graphName) {
      G->XvalueString(lineName, myXfunc);
      break;
    }
    G = G->_next;
  }
}
void webGraph::YvalueString(String graphName , String lineName, callback_with_arg_float myYfunc) {
  graph *G = _head;
  while (G) {
    if (G->getGraphName() == graphName) {
      G->YvalueString(lineName, myYfunc);
      break;
    }
    G = G->_next;
  }
}
uint32_t webGraph::memory(){
  uint32_t memo=sizeof(webGraph);
  graph *g=_head;
  while(g){
	memo += g->memory();
	g=g->_next;
  }
  memo += _webGraphString.length()+ _panelColor.length();
  return memo;
}
boolean webGraph::setDirty(boolean dirty) {
  _dirty = dirty;
  if(_dirty){
	  _webGraphString="";
  }
  graph *g=_head;
  while(g){
    g->setDirty(_dirty);
	g=g->_next;
  }
  return _dirty;
}
