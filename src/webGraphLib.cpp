#include "webGraphLib.h"
#include <ArduinoJson.h>
#include <WiFi.h>


#include <ESPAsyncWebServer.h>
template<class T> inline Print &operator <<(Print & obj, T arg) {
  obj.print(arg);
  return obj;
}

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG

#define BREAK             {Serial << "\nBREAK at "<<__func__<<":" << __LINE__; while (!Serial.available());Serial.read();}
#define DEBUGSTOP         {Serial << "\nSTOP at "<<__func__<<":" << __LINE__; while (1);}
#define LINE              {Serial << "\n["<<__func__<<":" << __LINE__<<"]";}
#define DEBUGOUT(var)     {Serial << "\n[" << __func__ << ":" << __LINE__ << "]" << #var<<"=" << var;}
#define WATCH(var)        {Serial << "\n[" << __func__ << ":" << __LINE__ << "]"<< #var<<"=" << var;} while (!Serial.available());Serial.flush();
#define DEBUGOUTH(var)    {Serial << "\n[" << __func__ << ":" << __LINE__ << "]" << #var<<"=" << String((uint32_t)var,HEX);}
#define WATCHH(var)       {Serial << "\n[" << __func__ << ":" << __LINE__ << "]"<< #var<<"=" << String((uint32_t)var,HEX);} while (!Serial.available());Serial.flush();

#else

#define BREAK
#define DEBUGSTOP
#define LINE
#define DEBUGOUT(var)
#define WATCH(var)
#define DEBUGOUTH(var)
#define WATCHH(var)

#endif

String Stringf(String format, float x, float y) {
  char bufferX[512];
  if (format.length() > sizeof(bufferX) / 6) {
	DEBUGOUT(format)
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
  unsigned int c = 0;
  point *h = this;
  while (h) {
    h = h->_next;
    c++;
    //H(h)
  }
  return c;
}

void point::removeAllPoints() {
  ////(String((uint32_t)this, HEX))
  if (this) {
    point  *p = this->_next;
    point *temp;
    while ( p  ) {
      temp = p->_next; //次のデータの参照を取っておく
      ////(String((uint32_t)p, HEX))
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
  if (p) {
    sprintf(buff , "%.1f,%.1f L", p->_x, p->_y);
    _pointString += String(buff);
  }
  for (; p; p = p->_next) {
    sprintf(buff , "%.1f,%.1f ", p->_x, p->_y);
    _pointString += String(buff);
  }
  return _pointString;
}

String point::response(float midY, float offsetX, float offsetY, float ratioX, float  ratioY) {
  //(ESP.getFreeHeap())
  char buff[64];
  String toRet;
  point *p = this;
  if (p) {
    sprintf(buff , "%.1f,%.1f L", ratioX * (p->_x - offsetX), offsetY - ratioY * (p->_y - midY)  );
    toRet += String(buff);
  }
  for (; p; p = p->_next) {
    sprintf(buff , "%.1f,%.1f ", ratioX * (p->_x - offsetX), offsetY - ratioY * (p->_y - midY) );
    toRet += String(buff)  ;
    //(toRet)
  }
  //(ESP.getFreeHeap())
  return toRet;
}

String point::print() {
  point *p = this;
  //  //(String((uint32_t)p, HEX))
  String ret = "\n\t\t\t";
  while (p) {
    ret += "point(x,y)=( " + gfp(p->_x) + " , " + gfp(p->_y) + " )";
    p = p->_next;
    //  //(String((uint32_t)p, HEX))

  }
  return ret;
}
//////
//// Tpoint class
//////


Tpoint::Tpoint(float x, float y, String text): point(x, y) {
  _text = text;
}
Tpoint::~Tpoint() {
  _text = "";
}
int Tpoint::removeTpoint(Tpoint *removeTpoint) {

  Tpoint  *t = this;
  Tpoint *tmp;
  while (t && ((point *)t)->_next && (Tpoint *)((point*)t)->_next != removeTpoint ) {
    t = (Tpoint *)((point*)t)->_next;
  }
  //最後まで見つからなかった
  if ( ((point*)t)->_next == NULL ) {
    return 0;
  }
  //removePointのデータをリストから外す
  tmp = (Tpoint *)((point*)t)->_next;

  ((point *)t)->_next = (Tpoint *) ((point*)tmp)->_next;

  tmp->_text = "";
  delete( tmp );
  return 1;
}

String Tpoint::response( float midY, float offsetX, float offsetY, float ratioX, float  ratioY, String color) {
  String toRet;
  String tmp;
  Tpoint *t = this;
  for (; t; t = (Tpoint *)((point *)t->_next)) {
    tmp = textMatrix;
    if (color.length()) {
      tmp.replace("fill=\"#00FF7F\"", "fill=\"" + color + "\"");
    }
    tmp.replace("></", ">" + t->_text + "</");
    float tx = ratioX * (t->x() - offsetX);
    float ty = offsetY - ratioY * (t->y() - midY);
    tmp.replace("matrix()", "matrix(1 0 0 1 " + String(tx,0) + " " + String(ty,0) + ")");
    toRet += tmp;
  }
  //(toRet)
  return toRet;
}
String Tpoint::response(  String color) {
  String toRet;
  String tmp;
  Tpoint *t = this;
  for (; t; t = (Tpoint *)((point *)t->_next)) {
    tmp = textMatrix;
    if (color.length()) {
      tmp.replace("fill=\"#00FF7F\"", "fill=\"" + color + "\"");
    }
    tmp.replace("></", ">" + t->_text + "</");
    float tx = t->x() ;
    float ty = t->y();
    tmp.replace("matrix()", "matrix(1 0 0 1 " + String(tx,0) + " " + String(ty,0) + ")");
    toRet += tmp;
  }
  //(toRet)
  return toRet;
}
uint32_t Tpoint::memory() {
  Tpoint * t = this;
  uint32_t memo = 0;
  while (t) {
    memo += sizeof(Tpoint) + _text.length();
    t = (Tpoint*)((point *)t)->_next;
  }
  return memo;
}

//////
//// line class
//////

String standarDisplayfunc( float xy) {
  return String((int)xy);
}

line::line(point *toSet) {
  ////(toSet->print())
  _next = 0;
  if (toSet && this) {
    _head = toSet;
  }
  XvalueString(0);
  YvalueString(0);
}
line::line(float xPoint, float yPoint) {
  point *p = new point(xPoint, yPoint);
  if (!p) log_e("out of memory");
  if (p && this) {
    _head = p;
  }
  XvalueString(0);
  YvalueString(0);
}

line::~line() {
  if (_head) {
    _head->removeAllPoints();
    _Thead->removeAllPoints();
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
  if (_dirty) {
    _lineString = "";
  }
  return _dirty;
}

unsigned int line::scan() {
  if (_scaleFix == false) {
    _maxX = _maxY = - _MAXFLOTPOINT_;
    _minX = _minY = _MAXFLOTPOINT_;
    unsigned int count = 0;
    ////(String((uint32_t)this,HEX))
    ////(String((uint32_t)_head,HEX))
    point *p = _head;
    for (; p; p = p->_next) {
      //log_i("p=%x",p);
      if (p->x() > _maxX) _maxX = p->x() ;
      if (p->y() > _maxY) _maxY = p->y() ;
      if (p->x() < _minX) _minX = p->x() ;
      if (p->y() < _minY) _minY = p->y() ;
      count ++;
    }
    //Serial.printf("\n_minX=%g  _minY=%g", _minX, _minY);
    //(_maxX)
    //(_maxY)
    //(_minX)
    //(_minY)

    if (_maxX - _minX < _MINFLOATPOINT_ * 100.) {
      _maxX += fabs(_maxX) / 1000.;
      _minX -= fabs(_minX) / 1000.;
    }
    if (fabs(_maxX) < _MINFLOATPOINT_ * 100. && fabs(_minX) < _MINFLOATPOINT_ * 100.) {
      _maxX += _MINFLOATPOINT_ * 100.;
      _minX -= _MINFLOATPOINT_ * 100.;
    }
    if (_maxY - _minY < _MINFLOATPOINT_ * 100.) {
      _maxY += fabs(_maxY) / 1000.;
      _minY -= fabs(_minY) / 1000.;
    }
    if (fabs(_maxY) < _MINFLOATPOINT_ * 100. && fabs(_minY) < _MINFLOATPOINT_ * 100.) {
      _maxY += _MINFLOATPOINT_ * 100.;
      _minY -= _MINFLOATPOINT_ * 100.;
    }

    return count;
  }
}
Tpoint *line::outOfRange() {
  Tpoint *t = _Thead;
  while (t) {
    if ( t->x() < _minX || t->x() > _maxX || t->y() < _minY || t->y() > _maxY) {
      return t;
    }
    t = (Tpoint *)((point *)t)->_next;
  }
  return 0;
}
line *line::addPoint(float x, float y) {
  point *p = new point(x, y);
  if (!p) log_e("out of memory");
  if (p) {
    //
    addPoint(p);
  }
}
line *line::addPoint(point *pointList) {
  //
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
      ////(removelCount)
    }
    while ( p &&  p->_next) {
      p = p->_next;
    }
    p->_next = pointList;
    //
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
line *line::addPoint(Tpoint *TpointList) {
  if ( TpointList && TpointList != this->_Thead) {
    _dirty = true;
    Tpoint *t = _Thead;
    if (! _Thead) {
      _Thead = TpointList;
      ////(_Thead)
      return this;
    }

    unsigned int thisCount = ((point *)t)->_count();
    //(thisCount)
    unsigned int addCount = ((point *)TpointList)->_count();
    //(addCount)
    unsigned int removelCount = 0;
    if (thisCount + addCount > _MAX_POINTS_IN_A_LINE_) {
      removelCount = thisCount + addCount - _MAX_POINTS_IN_A_LINE_;
    }
    while ( t && ((point *)t)->_next) {
      t = (Tpoint *)((point *)t)->_next;
    }
    ((point *)t)->_next = TpointList;
    //
    for (; removelCount; removelCount--) {
      if (_Thead) {
        Tpoint *nextChainTop = (Tpoint *)((point * )_Thead)->_next;
        _Thead->removePoint(_Thead);
        delete(_Thead);
        _Thead = nextChainTop;
      }
    }
  }
  return this;
}
int line::addPointString(String XYs) {
  _dirty = true;
  if (XYs.indexOf(",") < 0) return 0;
  if (XYs.indexOf(" ") < 0) return 0;
  //
  XYs.replace(" ,", ",");
  XYs.replace(", ", ",");
  XYs.replace("   ", " ");
  XYs.replace("  ", " ");

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
    //(iC)
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
  ////(String((uint32_t)L, HEX))
  ////(String((uint32_t)L->_next, HEX))
  line *temp;
  while ( L  ) {
    temp = L->_next; //次のデータの参照を取っておく
    point *p = (point *)L;
    ////(String((uint32_t)p, HEX))
    p->removeAllPoints();
    if (L != this) delete( L );
    L = temp;       //tempを次の処理対象に
  }
  _dirty = true;
}
size_t line::processNextPointResponse(String *content,int lineIndex,int lineNumbers,graph *parent,int halfPoint){
	if(halfPoint ==0){
		//DEBUGOUT(lineNumbers)
		//DEBUGOUT(lineIndex)
		//DEBUGOUTH(this)
		//DEBUGOUT(_lineNameY)
		size_t increase =	responseFirst(content, lineIndex,lineNumbers,parent);
		return increase;
	}else if(halfPoint==1){
		//DEBUGOUT(lineNumbers)
		//DEBUGOUT(lineIndex)
		//DEBUGOUTH(this)
		//DEBUGOUT(_lineNameY)
		size_t increase =	responseSecond(content, lineIndex,lineNumbers,parent);
		return increase;
	}else{
		//DEBUGOUT(lineNumbers)
		//DEBUGOUT(lineIndex)
		//DEBUGOUTH(this)
		//DEBUGOUT(_lineNameY)
		return 0;
	}
}
size_t line::responseFirst(String *content,int lineIndex, int lineNumbers, graph *parent) {
  //DEBUGOUT(_lineNameY)
 _lineString = "";
  lineNumbers = constrain(lineNumbers, 1, _MAX_LINES_IN_A_GRAPH_);
  lineIndex = constrain(lineIndex, 0, lineNumbers - 1);
  this->scan();
  Tpoint *remove;
  while (remove = outOfRange()) {
    if (_Thead) {
      _Thead->removeTpoint(remove);
    }
  }
  float xscale;
  float yscale;
  float offSetX = _minX;
  float harfY = parent->getSizeY() / lineNumbers / 2.0;
  float offSetY =  harfY * (2 * lineIndex + 1);
  float topY = offSetY - harfY;
  float diffX = (_maxX - _minX);
  float diffY = (_maxY - _minY);
  diffX < _MINFLOATPOINT_ * 100. ? diffX = _MINFLOATPOINT_ * 100.0 : diffX;
  diffY < _MINFLOATPOINT_ * 100. ? diffY = _MINFLOATPOINT_ * 100.0 : diffY;
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
  if (L) {
    point *p = L->_head;
    _lineString += p->response((_maxY + _minY) / 2.0, offSetX, offSetY, xscale, yscale);

  }
  //(_lineString)
  _lineString += pathString2;
  size_t increase=_lineString.length();
  (*content)+=_lineString;
  _lineString="";
  //log_i("inclease = %d",increase);
  return increase;
}
size_t line::responseSecond(String *content,int lineIndex, int lineNumbers, graph *parent) {
  
  float offSetX = _minX;
  float harfY = parent->getSizeY() / lineNumbers / 2.0;
  float offSetY =  harfY * (2 * lineIndex + 1);
  float diffX = (_maxX - _minX);
  float diffY = (_maxY - _minY);
  diffX < _MINFLOATPOINT_ * 100. ? diffX = _MINFLOATPOINT_ * 100.0 : diffX;
  diffY < _MINFLOATPOINT_ * 100. ? diffY = _MINFLOATPOINT_ * 100.0 : diffY;
  float xscale = parent->getSizeX() / diffX;
  float yscale = parent->getSizeY() / lineNumbers / diffY;
  String chunkColor = "\"#00FF7F\"";
  String toReplaceColor = colorList[lineIndex % _MAX_LINES_IN_A_GRAPH_];
   float topY = offSetY - harfY;

  Tpoint *t = _Thead;
  if (t) {
    _lineString += t->response((_maxY + _minY) / 2.0, offSetX, offSetY, xscale, yscale, _lineColor);
  }
  String txs = textGeneral;
  if (_lineNameY.length()) {
    txs.replace(chunkColor, toReplaceColor);
    txs.replace("x=\"0\" y=\"0\"", Stringf("x=\"%.1f\" y=\"%.1f\" text-anchor=\"middle\" alignment-baseline=\"hanging\" ", parent->getSizeX() / 2, topY));
    if (userDefLineScaleY) {
      txs.replace("</text>", _lineNameY +  "</text>");
    } else {
      txs.replace("</text>", _lineNameY + "=" + String((_maxY + _minY) / 2.0, 1) + "</text>");
    }
    txs += "\n<path stroke=" + toReplaceColor + " stroke-width=\"0.5\" fill=\"none\" d=\"" + _generateGrid(lineIndex, lineNumbers, parent) + "\" />";
    //(txs)
    _lineString += txs;
  }
  txs = textGeneral;
  if (_lineNameX.length()) {
    txs.replace(chunkColor, toReplaceColor);
    txs.replace("x=\"0\" y=\"0\"", Stringf("x=\"%.1f\" y=\"%.1f\" text-anchor=\"end\"", parent->getSizeX(), offSetY));
    txs.replace("</text>", _lineNameX +  "</text>");
    _lineString += txs;
  }
  //
  if (userDefLineScaleY) {
    //
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
    //(_maxY)
    //(userDefLineScaleY(_maxY))
  }
  size_t increase=_lineString.length();
  (*content)+=_lineString;
  _lineString="";
   //log_i("inclease = %d",increase);
  return increase;
}

String line::response(int lineIndex, int lineNumbers, graph *parent) {
  //
  //if (_dirty) {
  //
  _lineString = "";
  lineNumbers = constrain(lineNumbers, 1, _MAX_LINES_IN_A_GRAPH_);
  lineIndex = constrain(lineIndex, 0, lineNumbers - 1);
  this->scan();
  Tpoint *remove;
  while (remove = outOfRange()) {
    if (_Thead) {
      _Thead->removeTpoint(remove);
    }
  }
  float xscale;
  float yscale;
  float offSetX = _minX;
  float harfY = parent->getSizeY() / lineNumbers / 2.0;
  float offSetY =  harfY * (2 * lineIndex + 1);
  float topY = offSetY - harfY;
  float diffX = (_maxX - _minX);
  float diffY = (_maxY - _minY);
  diffX < _MINFLOATPOINT_ * 100. ? diffX = _MINFLOATPOINT_ * 100.0 : diffX;
  diffY < _MINFLOATPOINT_ * 100. ? diffY = _MINFLOATPOINT_ * 100.0 : diffY;
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
  if (L) {
    point *p = L->_head;
    _lineString += p->response((_maxY + _minY) / 2.0, offSetX, offSetY, xscale, yscale);

  }
  //(_lineString)
  _lineString += pathString2;
  
  Tpoint *t = L->_Thead;
  if (t) {
    _lineString += t->response((_maxY + _minY) / 2.0, offSetX, offSetY, xscale, yscale, _lineColor);
  }
  String txs = textGeneral;
  if (_lineNameY.length()) {
    txs.replace(chunkColor, toReplaceColor);
    txs.replace("x=\"0\" y=\"0\"", Stringf("x=\"%g\" y=\"%g\" text-anchor=\"middle\" alignment-baseline=\"hanging\" ", parent->getSizeX() / 2, topY));
    if (userDefLineScaleY) {
      txs.replace("</text>", _lineNameY +  "</text>");
    } else {
      txs.replace("</text>", _lineNameY + "=" + String((_maxY + _minY) / 2.0, 1) + "</text>");
    }
    txs += "\n<path stroke=" + toReplaceColor + " stroke-width=\"0.5\" fill=\"none\" d=\"" + _generateGrid(lineIndex, lineNumbers, parent) + "\" />";
    //(txs)
    _lineString += txs;
  }
  txs = textGeneral;
  if (_lineNameX.length()) {
    txs.replace(chunkColor, toReplaceColor);
    txs.replace("x=\"0\" y=\"0\"", Stringf("x=\"%g\" y=\"%g\" text-anchor=\"end\"", parent->getSizeX(), offSetY));
    txs.replace("</text>", _lineNameX +  "</text>");
    _lineString += txs;
  }
  //
  if (userDefLineScaleY) {
    //
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
    //(_maxY)
    //(userDefLineScaleY(_maxY))
  }
  //(_lineString)
  //}
  return _lineString;
}
String line::print() {
  line *L = this;
  String ret = "";
  while (L) {
    L->scan();
    if (_lineNameY.length()) {
      ret += "\n\t\tline(" + L->_lineNameY + ")";
    } else {
      ret += "\n\t\tline(" + String((uint32_t)L, HEX) + ")";
    }
    ret += " (maxX, maxY) = (" + String(L->_maxX) + ", " + String(L->_maxY) + ")";
    ret += " (minX, minY) = (" + String(L->_minX) + ", " + String(L->_minY) + ")";
    point *q = L->_head;
    //ret += q->print();
    L = L->_next;
    ////((uint32_t)L)
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
      toGenerate += "M" + String(dx * x, 1) + "," + String(oy, 1);
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
      toGenerate += "M" + String(0.0, 1) + "," + String(cy + oy + dy * y, 1);
      toGenerate += "L" + String(dx, 1) + "," + String(cy + oy + dy * y, 1);
    }
  }
  ////(toGenerate)
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
void line::setXvalueStringAngle(float angle) {
  _tA = constrain(angle, 0.0, 180.0);
  _tA = _tA * PI / 180.;
}
uint32_t line::memory() {
  uint32_t memo = sizeof(line);
  memo += _head->_count() * sizeof(point);
  if (_Thead)  memo += _Thead->memory();
  memo += _lineString.length();
  memo += _lineNameY.length();
  memo += _lineNameX.length();
  memo += _LineType.length();
  memo += _lineColor.length();
  memo += _lineWidth.length();
  if (_Thead)  memo += _Thead->memory();
  return memo;
}
void line::addUserText(String userText, float x, float y) {
  if (userText.length()) {

    Tpoint *t = new Tpoint(x, y, userText);
    if (!t) log_e("out of memory");
    if (t) {
      //(userText)
      addPoint(t);
    }
  }
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
  //
  if (!root.success()) {
    Serial.println(F("Failed to read json string"));
    return 0;
  }
  _dirty = true;
  float x = root[xKey].as<float>();
  //////(x)
  float y[_MAX_LINES_IN_A_GRAPH_];
  for (int i = 0; i < actualKeys; i++) {
    y[i] = root[yKeys[i]].as<float>();
    //////(y[i])
    ////(yKeys[i])
    line *L = searchLineName(yKeys[i]);
    ////(String((uint32_t)L, HEX))
    point *p = new point(x, y[i]);
    if (!p) {
      log_e("out of memory");
      return 0;
    }
    if (!L) {
      L = new line(p);
      graph::addLine(L);
      L->setLineName(yKeys[i], xKey);
    }
    ////(String((uint32_t)L, HEX))
    if (!L) {
      log_e("out of memory");
      return 0;
    }
    //
    L->addPoint(p);
  }
  jsonBuffer.clear();
  ;
  //this->print();
  ;
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
  //(json)
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    return 0;
  }


  float x = doc[xKey].as<float>();
  //(x)
  float y[_MAX_LINES_IN_A_GRAPH_];
  for (int i = 0; i < actualKeys; i++) {
    y[i] = doc[yKeys[i]].as<float>();
    //(y[i])
    ////(yKeys[i])
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
      L->setLineName(yKeys[i], xKey);
    }
    //DEBUGOUT(yKeys[0])
    //DEBUGOUT(yKeys[1])
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
  if (_dirty) {
    _graphString = "";
  }
  line *L = _head;
  while (L) {
    L->setDirty(_dirty);
    L = L->_next;
  }
  return _dirty;
}

size_t graph::processNextLineResponse(String *content,int lineIndex,float ypos){
	static int lineNumber=0;
	static int halfPoint=0;
	if(lineIndex==0){
		 lineNumber=0;
	}
	unsigned int LCount =_head->_count();
	//DEBUGOUT(LCount)
	
	size_t increase;
	if(lineNumber ==0){
		//LINE
		increase=responsePrep(content,ypos);
		lineNumber=1;
		return increase;
	}
	line *L=_head;
	int tmp=lineNumber-1;
	if(tmp < LCount){
		//DEBUGOUT(tmp)
		while(tmp-- && L){
			//DEBUGOUT(tmp)
			//DEBUGOUT(L->getLineName())
			L=L->_next;
			//DEBUGOUT(L->getLineName())
		}
	}
	//DEBUGOUT(lineNumber-1)
	//DEBUGOUT(L->getLineName())
	while(lineNumber<=LCount){
		do{
			DEBUGOUT(L->getLineName())
			increase=L->processNextPointResponse(content,lineNumber-1,LCount,this,halfPoint);
			halfPoint++;
			if(increase) return increase;
		}while(increase);
		lineNumber++;
		DEBUGOUT(lineNumber)
		halfPoint=0;
		return increase;
	}
	if(lineNumber==LCount+1){
		lineNumber++;
		increase=responseFinish(content);
		return increase;
	}
	lineNumber++;
	return 0;
}

size_t graph::responsePrep(String *content,float topPosition){
  scan();
  _graphString = base1;
  if (_graphName == "noName") {
    _graphString.replace("<!Title>", "");
  } else {
    _graphString.replace("<!Title>", _graphName);
  }
  _graphString.replace("translate(30.00,30.00", "translate(30.00," + String(topPosition));
  _graphString.replace("#000000", _bgColor);
  _graphString.replace("width=\"400.0\" height=\"200.0\"></rect>" , Stringf("width=\"%.1f\" height=\"%.1f\"></rect>", _sizeX, _sizeY));
  Tpoint *t = _Thead;
  if (t) {
    _graphString += t->response(colorList[0]);
  }
  size_t inc=_graphString.length();
  (*content)+=_graphString;
  _graphString="";
  return inc;
}
size_t graph::responseLine(String *content, int linenumber) {
  line * L;
  int lineCount = 0;
  if (linenumber >= _MAX_LINES_IN_A_GRAPH_) return 0;
  _graphString = "";
  int colorCount = 0;
  static boolean xScalalnotYet = true;
  if(linenumber==0) xScalalnotYet=true;

  for (L = _head; L; L = L->_next) {
    if (lineCount == linenumber) {
      L->scan();
      _graphString += "\n <!line" + String((uint32_t)L, HEX) + ">";
      _graphString += L->response(colorCount, _head->_count(), this) ;
      _graphString += " <!line" + String((uint32_t)L, HEX) + ">";
      if ((L->userDefLineScaleX || L->userDefLineScaleY) && xScalalnotYet) {
        String gs = graphScal;
        int i = 0;
        String xgs = ""; int ypos = 6;
        for (float xpos = 0.0; xpos <= _sizeX; xpos += _sizeX / _numberOfSplitCellsX) {
          gs = "\n" + graphScal;
          if (_tA != 0.0) {
            gs.replace("<!replace me>", String(cos(_tA)) + " " + String(sin(_tA)) + " " + String(-sin(_tA)) + String(cos(_tA)) + " "  + String(xpos) + " " + String(_sizeY + ypos));
            gs.replace("text-anchor=\"middle\"", "text-anchor=\"start\"");
          } else {
            gs.replace("<!replace me>", String("1 0 0 1 ") + String(xpos) + " " + String(_sizeY + ypos));
          }
          L->scan();
          L->getMinX();
          gs.replace("</text>", L->userDefLineScaleX(_minX + (_maxX - _minX) / _numberOfSplitCellsX * i) + "</text>");
          xgs += gs;
          i++;
          ////(xgs)
        }
        _graphString += xgs;
        xScalalnotYet = false;
      }
    }
    colorCount = (colorCount + 1) % _MAX_LINES_IN_A_GRAPH_;
    lineCount++;
  }
  size_t gsl = _graphString.length();
  (*content) += _graphString;
  _graphString = "";
  return gsl;
}
size_t graph::responseFinish(String *content){
	
	_graphString="";
	boolean 	xScalalnotYet =true;
	line *L=_head;
    if ((L->userDefLineScaleX || L->userDefLineScaleY) && xScalalnotYet) {
        String gs = graphScal;
        int i = 0;
        String xgs = ""; int ypos = 6;
        for (float xpos = 0.0; xpos <= _sizeX; xpos += _sizeX / _numberOfSplitCellsX) {
          gs = "\n" + graphScal;
          if (_tA != 0.0) {
            gs.replace("<!replace me>", String(cos(_tA)) + " " + String(sin(_tA)) + " " + String(-sin(_tA)) + String(cos(_tA)) + " "  + String(xpos) + " " + String(_sizeY + ypos));
            gs.replace("text-anchor=\"middle\"", "text-anchor=\"start\"");
          } else {
            gs.replace("<!replace me>", String("1 0 0 1 ") + String(xpos) + " " + String(_sizeY + ypos));
          }
          L->scan();
          L->getMinX();
          gs.replace("</text>", L->userDefLineScaleX(_minX + (_maxX - _minX) / _numberOfSplitCellsX * i) + "</text>");
          xgs += gs;
          i++;
          ////(xgs)
        }
        _graphString += xgs;
        xScalalnotYet = false;
    }

  _graphString += "\n" + base2;
  _graphString.replace("<!REPLACE_ME>", _generateGrid());
   size_t inc=_graphString.length();
  (*content)+=_graphString;
  _graphString="";
  return inc;

}
String graph::response(int graphCount, float topPosition) {
  //if (_dirty) {
  scan();
  ////(_minX)
  line *L = _head;
  int lineNumbers = 0;
  for (; L; L = L->_next) {
    lineNumbers++;
  }

  _graphString = base1;
  if (_graphName == "noName") {
    _graphString.replace("<!Title>", "");
  } else {
    _graphString.replace("<!Title>", _graphName);
  }
  _graphString.replace("translate(30.00,30.00", "translate(30.00," + String(topPosition));
  _graphString.replace("#000000", _bgColor);
  _graphString.replace("width=\"400.0\" height=\"200.0\"></rect>" , Stringf("width=\"%.1f\" height=\"%.1f\"></rect>", _sizeX, _sizeY));
  Tpoint *t = _Thead;
  if (t) {
    _graphString += t->response(colorList[0]);
  }
  int colorCount = 0;
  boolean xScalalnotYet = true;
  for (L = _head; L; L = L->_next) {
    L->scan();
    _graphString += "\n <!line" + String((uint32_t)L, HEX) + ">";
    _graphString += L->response(colorCount, lineNumbers, this) ;
    _graphString += " <!line" + String((uint32_t)L, HEX) + ">";
    colorCount = (colorCount + 1) % _MAX_LINES_IN_A_GRAPH_;
    if ((L->userDefLineScaleX || L->userDefLineScaleY) && xScalalnotYet) {
      //LINE
      String gs = graphScal;
      int i = 0;
      String xgs = ""; int ypos = 6;
      for (float xpos = 0.0; xpos <= _sizeX; xpos += _sizeX / _numberOfSplitCellsX) {
        //LINE
        gs = "\n" + graphScal;
        //(_tA)
        if (_tA != 0.0) {
          gs.replace("<!replace me>", String(cos(_tA)) + " " + String(sin(_tA)) + " " + String(-sin(_tA)) + String(cos(_tA)) + " "  + String(xpos) + " " + String(_sizeY + ypos));
          gs.replace("text-anchor=\"middle\"", "text-anchor=\"start\"");
        } else {
          gs.replace("<!replace me>", String("1 0 0 1 ") + String(xpos) + " " + String(_sizeY + ypos));
        }
        L->scan();
        L->getMinX();
        //(L->getMinX())
        //(_minX)
        //LINE
        gs.replace("</text>", L->userDefLineScaleX(_minX + (_maxX - _minX) / _numberOfSplitCellsX * i) + "</text>");
        xgs += gs;
        i++;
        ////(xgs)
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
      ////(removelCount)
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
graph *graph::addPoint(Tpoint *TpointToAdd) {
  if ( TpointToAdd && TpointToAdd != this->_Thead) {
    _dirty = true;
    Tpoint *t = _Thead;
    if (! _Thead) {
      _Thead = TpointToAdd;
      return this;
    }
    unsigned int thisCount = ((point *)t)->_count();
    unsigned int addCount = ((point *)TpointToAdd)->_count();
    unsigned int removelCount = 0;

    if (thisCount + addCount > _MAXTPOINT_IN_A_GRAPH_) {
      removelCount = thisCount + addCount - _MAXTPOINT_IN_A_GRAPH_;
    }
    //(removelCount)
    while ( t && ((point *)t)->_next) {
      t = (Tpoint *)((point *)t)->_next;
    }
    ((point *)t)->_next = TpointToAdd;

    for (; removelCount; removelCount--) {
      if (_Thead) {
        Tpoint *nextChainTop = (Tpoint *)((point * )_Thead)->_next;
        _Thead->removePoint(_Thead);
        delete(_Thead);
        _Thead = nextChainTop;
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
unsigned int graph::_countLine() {
 line  *L =_head;
  unsigned int c = 0;
  while(L){
	L=L->_next;
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

    c += L->scan();

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

  scan();

  String ret = "\n\tgraph(" + _graphName + ")";
  ret += " (maxX, maxY) = (" + String(_maxX) + ", " + String(_maxY) + ")";
  ret += " (minX, minY) = (" + String(_minX) + ", " + String(_minY) + ")";
  while (p) {
    ret += "graph(" + String((uint32_t)p, HEX) + ")";
    line *q = p->_head;
    ret += q->print();
    p = p->_next;

  }
  return ret;
}
void graph::setGraphName(String nameOfGraph) {
  _graphName = nameOfGraph;
}
void graph::setBackgroundColor(String ColorNumber) {
  _bgColor = ColorNumber;
}
String graph::_generateGrid() {

  String toGenerate = "";
  float dx = _sizeX / _numberOfSplitCellsX;
  float dy = _sizeY / _numberOfSplitCellsY;
  for (uint8_t x = 0; x < _numberOfSplitCellsX; x++) {
    toGenerate += "M" + String(dx * x, 1) + ", 0L" + String(dx * x, 1) + "," + String(_sizeY, 1);
  }
  for (uint8_t y = 0; y < _numberOfSplitCellsY; y++) {
    toGenerate += "M0," + String(dy * y, 1) + ", L" + String(_sizeX, 1) + ","   + String(dy * y, 1);
  }
  ////(toGenerate)
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
    if(L->getLineNameX()== lineName){
      L->XvalueString(myXfunc);
      break;
    }
    L = L->_next;
  }
  if (L == 0) {
    line *NL = new line(new point(0, 0));
    if (!NL)  log_e("out of memory");
    if (NL) {
      addLine(NL);
      NL->XvalueString(myXfunc);
      NL->setLineNameX(lineName);
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
  if (L == 0) {
    line *NL = new line(new point(0, 0));
    if (!NL) log_e("out of memory");
    if (NL) {
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
  if (L == 0) {
    line *NL = new line(new point(0, 0));
    if (!NL) log_e("out of memory");
    if (NL) {
      addLine(NL);
      NL->XvalueString(stdDispX);
      NL->YvalueString(stdDispY);
      NL->setLineName(yKeys);
    }
  }

}
void graph::setXvalueStringAngle(float angle) {
  _tA = constrain(angle, 0.0, 180.0);
  _tA = _tA * PI / 180.;
}
uint32_t graph::memory() {
  uint32_t memo = sizeof(graph);
  line *L = _head;
  while (L) {
    memo += L->memory();
    L = L->_next;
  }
  if (_Thead)  memo += _Thead->memory();
  memo += _graphString.length() + _graphName.length() + _bgColor.length();
  return memo;
}
void graph::addUserText(String userText, float x, float y) {
  if (userText.length()) {
    Tpoint *t = new Tpoint(x, y, userText);
    if (!t)  log_e("out of memory");
    if (t) {
      addPoint(t);
    }
  }
}
//////
//// webGraph class
//////
/*
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
*/

/*
webGraph::webGraph(AsyncWebServer *myServer) {
  _graphWebserver = myServer;
  _head = 0;
  if (_graphWebserver) {
    _graphWebserver->on("/", HTTP_GET, [this](AsyncWebServerRequest * request) {
        this->response();
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", this->_webGraphString);
    });
  }
}
*/
webGraph::webGraph(AsyncWebServer *myServer) {
  _graphWebserver = myServer;
  _head = 0;
  if (_graphWebserver) {
	_graphWebserver->on("/", HTTP_GET, [this](AsyncWebServerRequest * request) {
		this->setDirty(true);
		AsyncWebServerResponse *response =
		  request->beginChunkedResponse("text/html",[this](uint8_t* content, size_t maxLen, size_t index){
		   size_t K=this->_webGraphString.length();
				size_t writeSize = manageWebString(content,maxLen, index);
				return writeSize;
			});
		  request->send(response);
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
		this->setDirty(true);
		AsyncWebServerResponse *response =
		  request->beginChunkedResponse("text/html",[this](uint8_t* content, size_t maxLen, size_t index){
		   size_t K=this->_webGraphString.length();
				size_t writeSize = manageWebString(content,maxLen, index);
				return writeSize;
			});
		  request->send(response);
    });
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
webGraph *webGraph::addPoint(Tpoint *TpointToAdd) {
  if ( TpointToAdd && TpointToAdd != _Thead) {
    _dirty = true;
    Tpoint *t = _Thead;
    if (! _Thead) {
      _Thead = TpointToAdd;
      //H(_Thead)
      return this;
    }
    unsigned int thisCount = ((point *)t)->_count();
    unsigned int addCount = ((point *)TpointToAdd)->_count();
    unsigned int removelCount = 0;
    if (thisCount + addCount > _MAXTPOINT_IN_A_GRAPH_) {
      removelCount = thisCount + addCount - _MAXTPOINT_IN_A_GRAPH_;
    }
    while ( t && ((point *)t)->_next) {
      t = (Tpoint *)((point *)t)->_next;
    }
    ((point *)t)->_next = TpointToAdd;
    //
    for (; removelCount; removelCount--) {
      if (_Thead) {
        Tpoint *nextChainTop = (Tpoint *)((point * )_Thead)->_next;
        _Thead->removePoint(_Thead);
        delete(_Thead);
        _Thead = nextChainTop;
      }
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
  //while(busy) ;
  ////(sizeof(*yKey1))
  //
  if (graphName.length() && graphName == "noName") {
    return 0;
  }
  _dirty = true;
  graph *G = searchGraphName(graphName);
  if (!G) {
    G = new graph();
    if (G) {
      addGraph(G);
      G->setName(graphName);
    }
  }
  if (G) {
    G->importJson(json, xKey, yKey1, actualKeys);
  }
  return G;
}

String webGraph::response(String graphName) {
  _busy = true;
  if (_dirty) {
    graph *G = searchGraphName(graphName);
    if (!G) {
      return String("");
    } else {
      _webGraphString = wbase1;
      _webGraphString.replace("URL = http: //111.222.333.444", "URL=http://" + WiFi.localIP().toString());
      _webGraphString.replace("content=\"600", "content=\"" + String(_refreshSecond));
      //(_sizeY())
      _webGraphString.replace("height=\"490.00", "height=\"" + String( _sizeY()));
      _webGraphString.replace("height=\"480.00", "height=\"" + String( _sizeY()));
      _webGraphString.replace("fill=\"#DDDDDD", "fill=\"" + _panelColor);
      int graphCount = 0;
      ////(String((uint32_t)G, HEX))
      _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
      _webGraphString += G->response(graphCount, 30) ;
      _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
      ////(_webGraphString)
      _webGraphString += wbase2;
      _dirty = false;
    }
  }
  _busy = false;
  return _webGraphString;
}
float webGraph::_sizeX() {
  float maxX = 100;
  for (graph *G = _head; G; G = G->_next) {
    maxX = max(maxX, G->getSizeX());
    ////(maxX)
  }
  return maxX;
}
float webGraph::_sizeY() {
  float maxY = 60;
  for (graph *G = _head; G; G = G->_next) {
    if (G->_next) {
      maxY += G->getSizeY() + 60;
    } else {
      maxY += G->getSizeY();
    }
  }
  return maxY;
}
float webGraph::_sizeY(int count) {
  float maxY = 30;
  for (graph *G = _head; G && count; G = G->_next) {
    maxY += G->getSizeY() + 60;
    count--;
  }
  return maxY;
}
size_t webGraph::processNextresponse(size_t index){
	static int graphNumber=0;
	static int lineIndex=0;
	unsigned int gCount =_head->_count();
	if(index==0) graphNumber=0;
	size_t increase;
	if(graphNumber ==0){
		increase=responsePrep();
		graphNumber=1;
		return increase;
	}
	if(gCount){
		while(graphNumber<=gCount){
			graph *g=_head;
			int tmp=graphNumber;
			while(--tmp){
				g=g->_next;
			}
			unsigned int lineCount=g->_countLine();
			do{
				static int pointHalf=0;
				pointHalf++;
				int add= pointHalf%3 ? 0:1;
				increase=g->processNextLineResponse(&_webGraphString,lineIndex,_sizeY(graphNumber-1));
				if(lineIndex < 1) lineIndex++;
				else lineIndex+=add;
				if(increase) return increase;
			}while(increase  || lineIndex<=lineCount+1);
			graphNumber++;
			lineIndex=0;
		}
	}
	LINE
	if(graphNumber==gCount+1){
		graphNumber++;
		increase=responseFinish();
		return increase;
	}
	graphNumber++;
	return 0;
}
size_t webGraph::manageWebString(uint8_t *content,size_t maxlen,size_t index){
	LINE
	if(_busy==false){
		_busy=true;
		log_i("busy=%d",_busy);
	}
	static size_t maxWebstringlength=0;
	while(_webGraphString.length() < maxlen){
		size_t add=processNextresponse(index);
		if(add==0) break;
		index+=add;
	}
	if(maxWebstringlength <_webGraphString.length()){
		maxWebstringlength=_webGraphString.length();
		log_i("\nmaxWebstringlength = %d",maxWebstringlength);
	}
	size_t sendSize=flushWebString(content, maxlen);
	if(sendSize==0){
		_busy=false;
		log_i("busy=%d",_busy);
	}
	return sendSize;
}
size_t webGraph::flushWebString(uint8_t *content,size_t maxlen){
	size_t K=_webGraphString.length();
	if(!content) return 0;
	if(!maxlen) return 0;
	if (K==0) return 0;
	if(maxlen < K){				
		memcpy(content,_webGraphString.substring(0,maxlen).c_str(),maxlen);
		_webGraphString=_webGraphString.substring(maxlen);
		return maxlen;
	}else{
		memcpy(content,_webGraphString.substring(0,K).c_str(),K);
		_webGraphString="";
		return K;
	}
}
size_t webGraph::responsePrep(){
 static size_t generate;
  _busy = true;
  //if (_dirty) {
  //
  _webGraphString = wbase1;
  float tx = _sizeX() + 60;
  float ty = _sizeY()  ;
  //(ty)
  _webGraphString.replace("version=\"1.1\"width = \"460.00\" height=\"490.00\">", Stringf("version=\"1.1\"width = \"%g\" height=\"%g\">", tx, ty));
  _webGraphString.replace("URL=http://111.222.333.444", "URL=http://" + WiFi.localIP().toString());
  _webGraphString.replace("content=\"600", "content=\"" + String(_refreshSecond));
  _webGraphString.replace("width=\"480.00\" height=\"480.00\"", Stringf("width = \"%.1f\" height=\"%.1f\"", tx, ty));
  _webGraphString.replace("fill=\"#DDDDDD", "fill=\"" + _panelColor);

  Tpoint *t = _Thead;
  if (t) {
    _webGraphString += t->response("black");
  }
 generate =_webGraphString.length();
 return generate;
}
size_t webGraph::responseFinish(){
	 _webGraphString += wbase2;
	return wbase2.length();
}
size_t webGraph::responseGraph(int graphNumber){
  graph * G;
  int graphCount = 0;
  if(graphNumber>= _MAXGRAPHS_IN_A_WEBGRAPH_) return 0;
  size_t initialSize=_webGraphString.length();
  for (G = _head; G; G = G->_next) {
	 if(graphCount==graphNumber){
	    _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
    	_webGraphString += G->response(graphCount, _sizeY(graphCount)) ;
    	_webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
    }
    graphCount++;
  }
  size_t lastSize=_webGraphString.length();
  size_t increased=lastSize-initialSize;
  return increased;
}
String webGraph::response() {
  _busy = true;
  //if (_dirty) {
  //
  _webGraphString = wbase1;
  float tx = _sizeX() + 60;
  float ty = _sizeY()  ;
  //(ty)
  _webGraphString.replace("version=\"1.1\"width = \"460.00\" height=\"490.00\">", Stringf("version=\"1.1\"width = \"%g\" height=\"%g\">", tx, ty));
  _webGraphString.replace("URL=http://111.222.333.444", "URL=http://" + WiFi.localIP().toString());
  _webGraphString.replace("content=\"600", "content=\"" + String(_refreshSecond));
  _webGraphString.replace("width=\"480.00\" height=\"480.00\"", Stringf("width = \"%g\" height=\"%g\"", tx, ty));
  _webGraphString.replace("fill=\"#DDDDDD", "fill=\"" + _panelColor);

  Tpoint *t = _Thead;
  if (t) {
    _webGraphString += t->response("black");
  }
  graph * G;
  int graphCount = 0;
  for (G = _head; G; G = G->_next) {
    _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
    _webGraphString += G->response(graphCount, _sizeY(graphCount)) ;

    _webGraphString += "\n <!graph " + String((uint32_t)G, HEX) + " > ";
    ////(_webGraphString)
    graphCount = (graphCount + 1) % _MAXGRAPHS_IN_A_WEBGRAPH_;
  }
  _webGraphString += wbase2;
  _dirty = false;
  //}
  _busy = false;
  return _webGraphString;
}
String webGraph::print() {
  String ret = "\nwebGraph(" + _name + ")";
  graph *g = _head;
  while (g) {
    ret +=  g->print();
    g = g->_next;
  }
  return ret;
}
void webGraph::webRefreshRate(time_t refreshSecond) {
  _refreshSecond = constrain(refreshSecond, 10, 9999999);
}
void webGraph::XvalueString(String graphName , String lineName, callback_with_arg_float myXfunc) {
  graph *G = _head;
  while (G) {
    if (G->getGraphName() == graphName) {
      break;
    }
    G = G->_next;
  }
  if (!G) {
    G = new graph();
    if (G) {
      G->setName(graphName);
    }
  }
  G->XvalueString(lineName, myXfunc);
}
void webGraph::YvalueString(String graphName , String lineName, callback_with_arg_float myYfunc) {
  graph *G = _head;
  while (G) {
    if (G->getGraphName() == graphName) {
      break;
    }
    G = G->_next;
  }
  if (!G) {
    G = new graph();
    if (G) {
      G->setName(graphName);
    }
  }
  G->YvalueString(lineName, myYfunc);
}
uint32_t webGraph::memory() {
  uint32_t memo = sizeof(webGraph);
  graph *g = _head;
  while (g) {
    memo += g->memory();
    g = g->_next;
  }
  if (_Thead)  memo += _Thead->memory();
  memo += _webGraphString.length() + _panelColor.length();
  return memo;
}
boolean webGraph::setDirty(boolean dirty) {
  _dirty = dirty;
  if (_dirty) {
    _webGraphString = "";
  }
  graph *g = _head;
  while (g) {
    g->setDirty(_dirty);
    g = g->_next;
  }
  return _dirty;
}
void webGraph::addUserText(String userText, float x, float y) {
  if (userText.length()) {
    Tpoint *t = new Tpoint(x, y, userText);
    if (!t) log_e("out of memory");
    if (t) {
      addPoint(t);
    }
  }
}
boolean webGraph::addUserText(String objectName, String userText, float x, float y, uint8_t fontsize, float angle ) {
  graph *g = _head;
  line *L = 0;
  boolean result = false;
  if (objectName == _name) {
    addUserText(userText, x, y);

    result = true;
  }
  while (g) {
    if (objectName == g->getGraphName()) {
      g->addUserText(userText, x, y);
      result = true;
    }
    g = g->_next;
  }
  g = _head;
  while (g) {
    L = g->searchLineName(objectName);
    //(L)
    while (L) {
      if (objectName == L->getLineName()) {

        L->addUserText(userText, x, y);
        result = true;
      }

      L = L->_next;
      //(L)
    }
    g = g->_next;
    //(g)
  }
  //
  if (!result) log_n("no such object name \"%s\"", objectName);
  return result;
}

