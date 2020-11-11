# ウェブグラフライブラリ
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLibは、**ESP32**を使ったArduinoプロジェクト用のWebユーザ・インターフェースを作成するためのシンプルなグラフィックス・ライブラリです。Webソケットを使用しており、複数のブラウザデバイスを介してGUI上でダイナミックなグラフィック要素を作成したり、更新したりすることができます。

このライブラリは、一般的な**ESP32ボード**で動作します。
![ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 

このライブラリには「ESP32用Arduinoコア」も必要です(https://github.com/espressif/arduino-esp32#installation-instructions)
### 依存関係
このライブラリは、2つの有名なESP32ライブラリに依存しています。
- ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- ArduinoJson](https://github.com/bblanchon/ArduinoJson)
あわせてダウンロードを検討ください。

## 提供される機能
- センサのシーケンスデータなどをカラーグラフで表示します。
- MQTT Brokerから送られてくるJSONデータをカラーグラフに表示します。
- グラフは常に新しいデータが入ってくると更新されます。'
- オートスケーリング機能(浮動小数点データの最大値と最小値を検出します)
- ESPAsyncWebServerによるグラフィックスWebサーバー機能
- グラフ表示内容を簡単にカスタマイズ可能です、。

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/provideFunctionAbstruct.png)

## インストール 

### ZIPファイルをダウンロードしてください。
![click here](https://github.com/HideakiAbe/ESP32Repository/archive/main.zip)

![zipファイルをダウンロード](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### zip ライブラリをインポートします。

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipLibimport.png)

## Arduino IDEでサンプルを使ってみよう 
- simpleGraph.ino : アナログリードデータを連続的にグラフ線に変換する．
- multiGraph.ino : 1つのウィンドウに2つのグラフを表示する 
- inputJson.ino : JSONの文字列を複数行にインポートする。
- MQTTGpaph.ino : MQTTブローカー "test.mosquitto.org "に接続してJSONデータを取得する。
- windBlows : グラフの範囲を設定します。


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)

### ここではサンプルスケッチ "windBlows "を開いた後に。
Wi-Fi設定を自分環境あわせてに変更してください。

```cpp
const char* ssid = "****".
const char* password = "****".
```

### サンプルスケッチをESP32ボードにアップロードした後
esp32ボードに接続し、シリアルモニターに「You can now access graph to http://ipaddress」と表示されるのを待ちます。Webブラウザから http://ipaddressにアクセスできるようになります。
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)

## 4つのオブジェクトとは
- webGrap object
- graph object
- line object
- point object
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/allOject.png)


### webGraph オブジェクト座標系の説明
    
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/webGraphOject.png)

webGraph のXsize, Ysize は子グラフのサイズとその数によって自動的に決定されます．デフォルトでは xsize=460, ysize=260 に設定されています．サイズは _sizeX() と _sizeY() のメンバ関数を使って確認することができます。ただし，WebGraph のサイズはユーザが直接設定することはできません．
変更は子供のオブジェクトgraphから行ってください。

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/objectLevel.png)

webGraphオブジェクトはこのライブラリのトップレベルのオブジェクトです．このwebGraphはウェブサーバ・ポインタを保持し，デフォルトではこのオブジェクトの中に1つまたは最大3つのまでの子グラフを保持します．最大グラフ数はヘッダファイル <webGraphLib.h> でいかのように定義されています．若干の変更は可能ですが、ヒープ領域の制限から３つの掛け算の値
２４０ｘ４X3＝２８８０
を超えないように設定をお願いします。本ライブラリの全オブジェクトの利用してる実メモリ量はmemory()関数で確認できます。２８８０ポイントを最大に利用した場合でも本ライブラリの消費メモリ量が40Kバイト以下に抑えるように設計されています。

```cpp
const unsigned int _MAX_POTS_IN_A_LINE_ = 240;
const unsigned int _MAX_LINES_IN_A_GRAPH_ = 4;
const unsigned int _MAXGRAPHS_IN_A_WEBGRAPH_ = 3;
```

## グラフを簡単に作成する
このライブラリを利用して最も簡単なグラフ作成方法を説明します。
- webGraphを作成
- JSON テキストを　webGraphにインポートする

以下の10数行の簡単な手順で34番ピンのアナログ電圧や35番ピンのグラフを作成できます。
wiFi へ接続する手順は省いていますので追加ください。
```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h> //https://github.com/me-no-dev/ESPAsyncWebServer
#include <webGraphLib.h>
AsyncWebServer webServer(80);
webGraph *w = new  webGraph (&webServer);
void setup() {
  Serial.begin(115200);
  //connect To wifi 
  w->begin();
}
void loop() {
  String jsonString = "{\"time\":" + String(millis()) + ",\"volt34\":" + String(analogRead(34)) + ",\"volt35\":" + String(analogRead(35)) + "}";
  String xkey("time");
  String ykeys[2] = {"volt34", "volt35"};
  int ykeyElements = 2;
  while (w->busy()); //ブラウザアクセス中はインポートを待つ
  w->importJson("mygraph", jsonString, xkey, ykeys, ykeyElements);
  delay(100);
}
```
グラフ作成関数のimportJsonの機能について説明します。
- 第一引数はgraph名をString型で指定します。すでにその名前のgraphがwebGraphに存在している場合はそのgraphに第二引数のjsonデータを追加します。
- 存在していない場合には新しく指定した名前のgraphを作成し、第二引数のjsonデータを1つのpointデータとして追加します。
- 第二引数はgraphにしたいデータを含むjsonのStringです。この例ではtime,volt34,volt35をkeyとしてその値が設定されています。
- 第三引数はx軸として利用するjsonのkeyをString型で指定します。この例ではx軸の値として"time"指定しています。
- 第四引数はy軸として利用するjsonのkeyをString型の**配列**で指定します。この例では2つのy軸のとしてvolt34とvolt35を指定しています。graphは上下2つに分割され上にvolt34の折線lineが、下にvolt35の折線lineが表示されます。
配列には最大で4つまでkeyを指定できます。その場合は4つに分割されて表示されます。ykeysはそれぞれのline名として利用さまれす。
- 第五引数は第四引数の配列の要素数を設定して

       
    
## オブジェクトの基本操作
### オブジェクトの生成例
```cpp
float x=0.0;
float y=analogRead(34);
point *p=new point(x,y); 
      //pointには必ずx,y座標をfloat型で指定してください。
line *L =new line(p); 
      //lineは必ず１つ以上のpointを持たせてください。
graph *g=new graph();
AsyncWebServer　webServer;
webGraph *w=new  webGraph (&webServer);
     //webGraphには必ずAsyncWebServerのポインタを指定してください。
```
### 親オブジェクトへ子オブジェクトの追加例
lineにpointを追加する。
```cpp
line *new line(p);
point *q=new point(x,y);
//point の使いまわしは不可、都度必ず新しいpoint を追加してください。
L->addPoint(q);
```
graph *gにline *Lを追加する。
```cpp
g->addLine(L);
```
webGraph *wにgraph *gを追加する。
```cpp
ｗ->addGraph(g);
```
まとめますと孫から子供　そして　子供から親の順番に作成してから追加してゆく手順になります。

###オブジェクトの廃棄
規定された数以上の子オブジェクトを親に追加すると、古いオブジェクトは自動的に廃棄されます。本ライブラリでは最新で最大数の子オブジェクトが残る仕様となっています。そのような理由で常に最新のデータで更新されるので通常はオブジェクトを廃棄する必要性は低いといえます。
あえて特定のオブジェクトの廃棄は行うには以下の関数があります。
１、オブジェクトへのポインタで指定して廃棄するタイプ
```cpp
_head->removePoint(pRemove);　
```
pRemove: リンクから外して廃棄したいpoint
_headは通常先頭の（古い）pointのポインタで親lineオブジェクトの_head(protected:）要素で管理されています。
_head要素は公開されていないのでメンバー関数以外からは消しにくい設計となっています。_head以外でもかまわないですが、それよりも新しいオブジェクトだけが削除対象となります。　

以下同様です。
指定したlineの廃棄
```cpp
_head->removeLine(Lremove);
```
指定したgraphの廃棄
```cpp
_head->removeGraph(gRemove)
```

２、オブジェクトを一気に廃棄するタイプ
```cpp
_head->removeAllPoints();
_head->removeAllLines();
_head->removeAllGraphs();
```
webGraphの廃棄関数は~webGrap()となります。

## オブジェクトの名前づけ
graphオブジェクトとlineオブジェクトには名前をつけることができます。lineオブジェクト名前はlineオブジェクトのｙ軸の名前と兼用しています。lineオブジェクトのX軸にも名前も持たせることができます。名前は後述するオブジェクトの操作を行う上でとても重要です。　ユニークな名前を持たせることで特定のオブジェクトへのアクセスが可能になります。pointオブジェクトやwebGraphオブジェクトには名前をつけることはできません。
```cpp
//lineオブジェクトには名前づけと名前の取得
line *L=new line(0.0,0.0);
L->setLineName(String nameLineToSet("volt"));
String　lNameY= getLineName();

L->setLineNameX("time");
String  lNmaeX =getLineNameX();
//両方の名前を同時に設定するには
L->setLineName("volt","time");
//第一引数がライン名で第二引数がX軸名です。ご注意ください。


//graphオブジェクトの名前づけと名前の取得
graph *g=new graph();
g->setGraphName("the graph");
String g->getGraphName();
```
## オブジェクトの名前による検索
名前をつけられたオブジェクトが親オブジェクトの子供として設定されている場合は
親オブジェクトから子オブジェクトの名前による検索できます。
```cpp
//子オブジェクトに名前を持たせる処理例
line *L =new line(p); 
L->setLineName("myLine");
graph *g =new graph();
g->addLine(L);
AsyncWebServer　webServer;
webGraph *w=new  webGraph (&webServer);
g->setGraphName("myGraph");
w->addGraph(g);
......
//名前による検索処理
graph *lookFor=w->searchGraphName("myGraph");
line *look =lookFor->searchLineName("myLine");
```
オブジェクト階層の最上位のwebGraph　*w　だけを保持していれば名前で子供オブジェクトを芋づる式に把握できることになります。
ですからwebGraph　オブジェクトはグローバル変数領域で宣言するとすべてのローカル関数で利用できるので便利かもしれません。
そのような例を以下に示します。
```cpp
AsyncWebServer　webServer;
webGraph *w=new  webGraph (&webServer);
void setup(){
//wを利用
}
void loop(){
//wを利用
}
```
ください。この例では2となります。

## 名前の設定
```cpp
//line名前の設定関数
void line::setName(String nameLineToSet);     // line name = line y-axis name
void line::setLineName(String nameLineToSet); //same functin as setName()
void line::setLineName(String nameLineToSet, String nameX);
void line::setLineNameX(String nameX);

//line名前の取得関数
String line::getLineName();
String line::getLineNameX();
```


## X軸Y軸表示関数の設定
グラフの両軸にスケールを表示する要望は多いかと思いますが、
表示の仕方は好みが分かれるのでユーザが設定可能な表示関数を
設定できます。
```cpp
//lineを直接指定してそのXY軸に表示関数を設定します
void line::XvalueString(callback_with_arg_float myXFunction);
void line::YvalueString(callback_with_arg_float myYFunction);

//graphに含まれるline名をxName,yNameで探してその軸に表示関数を設定します。
//存在しない場合はその名前でlineを追加します。
void graph::XvalueString(String xName, callback_with_arg_float myXfunc);
void graph::YvalueString(String yName, callback_with_arg_float myYfunc);
void graph::XYvalueString(String xName, callback_with_arg_float myXfunc, String yName, callback_with_arg_float myYfunc);
void graph::setXvalueStringAngle(float angle);

//webGraphに含まれるline名をgraph名とline名で探してその軸に表示関数を設定します。
//存在しない場合はその名前でgraphを追加します。
void webGraph::XvalueString(String graphName, String lineName, callback_with_arg_float myXfunc);
void webGraph::YvalueString(String graphName, String lineName, callback_with_arg_float myYfunc);
```
- X軸はグラフオブジェクトのグリッドの線の真下にXの座標値が表示されます。テキストの左右幅の中央と座標軸が一致します。
- ｙ軸は各グリッドラインの最大値と最小値の２か所に表示されます。フォントサイズは６ポイントに固定されています。
表示位置はグラフの左側で文字列の終わりがグラフの左側になります。
- 最小値ラインが文字列のベースラインと一致します。
- 最大値ラインの左側の文字列の上下位置は"hanging"となりますが,ブラウザーの種類によっては表示位置がずれることがあります。
- 文字列の回転に対応しているのはgraphのX軸のみでsetXvalueStringAngleを利用してください。時計回り正方向の回転です。
回転角度がゼロでない正の値は文字列の先頭が座標軸に合わせられます。
- 座標軸やグリッドラインに表示できる文字列は８文字程度なので長い文字列にならないよう表示関数を工夫ください。

## カラーの指定
色指定があらかじめデフォルト色が決まっていますが、３つのオブジェクトは色を変更することが可能です。
変更したいオブジェクトのポインタから下記関数を呼んでください。
```cpp
void line::setLineColor(String color);
void graph::setBackgroundColor(String ColorNumber);
void webGraph::setBackgroundColor(String color);
```
## ユーザーテキストの追加
座標軸とは別に　任意の場所に　特定の文字列を表示したい場合は
文字列を追加したいオブジェクトのポインタから下記関数を呼んでください。
指定するｘｙ座標は各オブジェクト固有の座標系で指定してください。
```cpp
void line::addUserText(String userText,float x,float y);
void graph::addUserText(String userText,float x,float y);
void webGraph::addUserText(String userText,float x,float y);
boolean webGraph::addUserText(String objectName,String userText,float x,float y,uint8_t fontsize =6,float angle =0);
```
- userText文字列をx,y座標に表示します。座標の指定は各オブジェクトの座標系で指定してください。
- ４番目のobjectNameを引数に持つ関数は配下のオブジェクト名を探してそのオブジェクトに文字列を追加します。
- この４番目の関数はフォントサイズと文字列の回転角度（時計回りが正）を指定できます。
- 文字列の表示位置は回転角度０度の場合は文字列左右中央のベースラインが(x,y)と一致します。
- 回転角度が正の場合は文字先頭が(x,y)と一致します。
- line オブジェクトに表示する文字の位置はその座標系に従うので　表示しているpointの最大最小の範囲にあるものだけが表示されます。その範囲外になったユーザテキストは削除されます。
- 各オブジェクトは最大１０個のユーザテキストを保持できますが、それ以上のユーザテキストを追加した場合は古いものから順番に削除されます。

## グリッドラインの変更
```cpp
void line::setGrid(uint8_t cellsXSplit = 24, uint8_t cellsYSplit = 2);
void graph::setGrid(uint8_t cellsXSplit = 24, uint8_t cellsYSplit = 8);
```
- グリッドラインはgraphとlineオブジェクトの両方に個別に設定するすることができます。グリッド線の表示は両方とも行われます。
- X軸方向はデフォルトで24分割されるようグリッド線が引かれます。
- lineのY軸方向はフォルトで2個に分割されるようにグリッド線が引かれます。
- lineのグリッド線の色はラインで指定した色と同じになります。
- graphのY軸方向はフォルトで8個に分割されようにグリッド線が引かれます。
- graphのグリッド線の色はいまのところ#DDDDDDで変更できません。　どうしても色を変更したい場合はwebGraph.hの290行目のbase2文字列を直接編集するしか変更手段がありません。変更した場合にはどのような副作用があるかはわかりません。



### webGraphオブジェクトの生成例
```cpp
/// webGraph オブジェクトのコンストラクタは 必ずAsyncWebServer ポインタのパラメータを持たなければなりません．
webGraph *w = new webGraph(&myServer);

// すでに作成済みのgraphオブジェクトを含めたコンストラクタでもOKです。
graph *g=new graph();
webGraph *w = new  webGraph(&myServer,g);
 ```
## webGraphの利用方法
```cpp
//メンバー関数の利用方法で必須の事項
  w->begin();
// ブラウザからのhttpリクエストに応答を開始します。 
//begin()が実行されるまではWEBへの公開はされません。
```
## webGraph に後からグラフを追加したい場合は
ポインタでで操作します。
```cpp
w->addGraph(g);
//グラフオブジェクトをwebGraphに追加するメンバー関数です。
//webGrapに4つ以上のグラフを追加した場合。
//webGraphは3つのグラフまでしか保持しません。
```
## 動的なグラフの作成例
```cpp
loop(){
    String json=getJsonFromMQtt(); // json文字列を定期的に取得する処理　例えばMQTTからのサブスクリプション
    while(w->busy());　　　　　　　//web ブラウザへの応答中はオブジェクトの変更は待つ。
                                  //ブラウザへの応答中はw->busy()はtureを返す。応答が終わるとfaluseを返す。
    w->importJson("myGraph",json,xkey,ykeys,elements); //オブジェクトの追加や変更
}
```
importJsonについては[グラフを簡単に作成する]を参照

## 動的なグラフの表示
```cpp
void webRefreshRate(time_t refreshSecond = 600);
// ウェブブラウザにリフレッシュ周期を通知します。
```



    
    
    
