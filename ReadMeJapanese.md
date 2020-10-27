# ウェブグラフライブラリ
![webGraphLib](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/sampleGraph.png)
webGraphLibは、**ESP32**を使ったArduinoプロジェクトのためのウェブ・ユーザー・インターフェースを作成するためのシンプルなグラフィック・ライブラリです。Webソケットを使用しており、ブラウザデバイスを介してGUI上でダイナミックなグラフィック要素を作成したり、更新したりすることができます。

このライブラリは一般的な**ESP32ボード**で動作します。
![ESP32](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/ESP32.jpg) 

このライブラリには[ESP32用Arduinoコア]も必要です(https://github.com/espressif/arduino-esp32#installation-instructions)
## 依存関係
このライブラリは、2つの有名なESP32ライブラリに依存しています。
- ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- ArdunoJson](https://github.com/bblanchon/ArduinoJson)

## 提供される関数
- センサから得た連続したデータをカラーグラフで表示します。
- MQTT Brokerから送られてくるJSONデータをカラーグラフで表示します。
- グラフは、新たに入力されたデータに応じて常に更新されます。
- オートスケーリング機能(浮動小数点データの最大値と最小値を検出)
- ESPAsyncWebServerによるグラフィックスWebサーバー機能
- グラフ表示属性を簡単にカスタマイズできます。

## インストール 

### ZIPファイルをダウンロードしてください。

![zipファイルをダウンロード](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/zipDwonload.png)


### zip ライブラリをインポートします。

!



## arduino IDEで始める 
- simpleGraph.ino : アナログリードデータを連続的にグラフ線に変換する．
- multiGraph.ino : 1つのウィンドウに2つのグラフを表示する 
- inputJson.ino : Jsonの文字列を複数行にインポートする。
- MQTTGpaphino : MQTTブローカー "test.mosquitto.org "に接続してJsonデータを取得する。
- windBlows : グラフの範囲を設定します。


![start](https://github.com/HideakiAbe/ESP32Repository/blob/main/doc/Startsample.png)


 ## 4つのオブジェクトを使用して
    - ウェブグラフオブジェクト
    - グラフオブジェクト
    - かいせんたいしょう
    - 点対象
    
     JSON import ArduinoJson](#json)
 
###Webグラフの設定



