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
esp32ボードに接続し、シリアルモニターに「You can now access graph to http://<ipaddress>」と表示されるのを待ちます。Webブラウザから http://<ipaddress> にアクセスできるようになります。
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)

## 4つのオブジェクトを使用して
### webGraph オブジェクト座標系の説明
    
![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/webGraphOject.png)

webGraph のXsize, Ysize は子グラフのサイズとその数によって自動的に決定されます．デフォルトでは xsize=460, ysize=260 に設定されています．サイズは _sizeX() と _sizeY() のメンバ関数を使って確認することができます。ただし，WebGraph のサイズはユーザが直接設定することはできません．
変更は子供のオブジェクトgraphから行ってください。

![](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/objectLevel.png)

webGraphオブジェクトはこのライブラリのトップレベルのオブジェクトです．このwebGraphはウェブサーバ・ポインタを保持し，デフォルトではこのオブジェクトの中に1つまたは最大3つのまでの子グラフを保持します．最大グラフ数はヘッダファイル <webGraphLib.h> でいかのように定義されています．若干の変更は可能ですが、ヒープ領域の制限から３つの掛け算の値２４０ｘ４　X　3＝２８８０を超えないように設定をお願いします。


cpp
const unsigned int _MAX_POTS_IN_A_LINE_ = 240.
const unsigned int _MAX_LINES_IN_A_GRAPH_ = 4.
const unsigned int _MAXGRAPHS_IN_A_WEBGRAPH_ = 3.

```

### 以下の関数でwebGraphを利用することができます．
cpp
/// webGraph オブジェクトのコンストラクタは 必ずAsyncWebServer ポインタのパラメータを持たなければなりません．
    webGraph(AsyncWebServer *myServer).
    webGraph(AsyncWebServer *myServer, graph *g).
    webGraph(AsyncWebServer *myServer, graph *g); ~webGraph().
    すでに作成済みのgraphオブジェクトを含めたコンストラクタでもOKです。

//メンバー関数の利用方法
    
    void begin()。 
    ブラウザからのhttpリクエストであれば、 //ウェブレスポンスを開始します。begin()が実行されるまではWEBへの公開はされません。
    
    webGraph *addGraph(graph *g).      
    //グラフオブジェクトをwebGraphに追加するメンバー関数です。
    //webGrapに4つ以上のグラフを追加した場合。
    //webGraphは3つのグラフまでしか保持しません。
    
    boolean setDirty(boolean dirty).   
    // このwebGraphの下にある子オブジェクトを変更した場合は，真の値を設定してください。真の値を設定した場合は変更内容がグラフに反映されます。

    void webRefreshRate(time_t refreshSecond = 600). 
    // ウェブブラウザにリフレッシュ期間を通知します。
    
    void XvalueString(String graphName, String lineName, callback_with_arg_float myXfunc). 
    //X値表示関数を指定されたグラフに設定にします。
    //mywebGraph->XvalueString("myGraph", "time",myXfunc).
　設定する関数は浮動小数点を引数として関数の戻り値がString型です。
例えば以下のような関数です。
    //String myXfunc(float time){ return String(uint32_t(time);}.
    
    void YvalueString(String graphName, String lineName, callback_with_arg_float myYfunc).
    //名前のついたグラフにY軸の値の表示関数を設定する
    
    void setBackgroundColor(String color).
    // 背景色を設定します (デフォルトカラー ="#dddddd")
    
    graph *importJson(String graphName, String json, String xKey. 
    
    
    
    
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
//point の使いまわしは不可、必ず新しいpoint を追加してください。
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
webGraphの廃棄はある意味で本ライブラリの存在を否定するのでできません。
２、オブジェクトを一気に廃棄するタイプ
```cpp
_head->removeAllPoints();
_head->removeAllLines();
_head->removeAllGraphs();
```
