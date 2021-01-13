# dprs_usb_bme280
### USB Host Sield と BME280 を組み合わせて ArduinoからIC-705にD-PRSの気象データを送るプログラム

D-STARのDVモードには、音声フレームの他にもデータフレームが用意されており、  
音声と同時にユーザーが任意のデータを送ることができます。

D-PRS(D-STAR Packet Reporting System)はこのデータ部分を利用したもので、  
APRSサーバーに接続されているI-GATE局を介して、送信元のGPSの位置情報等を  
APRSサーバーへ引き渡すことができます。

APRSには気象局というカテゴリがあり、位置情報に気温や気圧のデータを付加すると、  
無線局の送信位置の気象情報を登録することができます。

このプログラムは IC-705 と 気象センサ、およびArduinoとUSB Host Shieldを組み合わせて  
D-PRSの気象局を構築したものです。  

![](https://github.com/7m4mon/dprs_usb_bme280/blob/main/dprs_bme280_block_sch.png)

![](https://github.com/7m4mon/dprs_usb_bme280/blob/main/dprs_bme280_inside.jpg)
