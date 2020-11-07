# ウェブグラフライブラリ
!webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLibは、**ESP32**を使ったArduinoプロジェクト用のWebユーザ・インターフェースを作成するためのシンプルなグラフィックス・ライブラリです。Webソケットを使用しており、複数のブラウザデバイスを介してGUI上でダイナミックなグラフィック要素を作成したり、更新したりすることができます。

このライブラリは、一般的な**ESP32ボード**でも動作します。
!ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 

このライブラリには「ESP32用Arduinoコア」も必要です(https://github.com/espressif/arduino-esp32#installation-instructions)
## ♪# 依存関係
このライブラリは、2つの有名なESP32ライブラリに依存しています。
- ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- ArduinoJson](https://github.com/bblanchon/ArduinoJson)
あわせてダウンロードを検討ください。

## 提供される関数
- センサのシーケンスデータなどをカラーグラフで表示します。
- MQTT Brokerから送られてくるJSONデータをカラーグラフに表示します。
- グラフは常に新しいデータが入ってくると更新されます。'
- オートスケーリング機能(浮動小数点データの最大値と最小値を検出します)
- ESPAsyncWebServerによるグラフィックスWebサーバー機能
- グラフ表示内容を簡単にカスタマイズ可能です、。

!

## インストール 

### ZIPファイルをダウンロードしてください。

![zipファイルをダウンロード](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### zip ライブラリをインポートします。

!

## Arduino IDEでサンプルを使ってみよう 
- simpleGraph.ino : アナログリードデータを連続的にグラフ線に変換する．
- multiGraph.ino : 1つのウィンドウに2つのグラフを表示する 
- inputJson.ino : JSONの文字列を複数行にインポートする。
- MQTTGpaph.ino : MQTTブローカー "test.mosquitto.org "に接続してJSONデータを取得する。
- windBlows : グラフの範囲を設定します。


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)

### ここではサンプルスケッチ "windBlows "を開いた後に##。
Wi-Fi設定を自分環境あわせてに変更してください。

cpp

const char* ssid = "****".
const char* password = "****".
```

### サンプルスケッチをESP32ボードにアップロードした後
をesp32ボードに接続し、シリアルモニターに「You can now access graph to http://<ipaddress>」と表示されるのを待ちます。Webブラウザから http://<ipaddress> にアクセスできるようになります。
![windBlows](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/tornadopng.png)

## 4つのオブジェクトを使用して
### webGraph オブジェクト座標系の説明
    
!
webGraph のXsize, Ysize は子グラフのサイズとその数によって自動的に決定されます．デフォルトでは xsize=460, ysize=260 に設定されています．サイズは _sizeX() と _sizeY() のメンバ関数を使って確認することができます。ただし，WebGraph のサイズはユーザが直接設定することはできません．
変更は子供のオブジェクトgraphから行ってください。

!

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

www.DeepL.com/Translator（無料版）で翻訳しました。
