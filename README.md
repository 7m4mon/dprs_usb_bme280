# dprs_usb_bme280
USB Host Sield と BME280 を組み合わせて ArduinoからIC-705にD-PRSの気象データを送るプログラム

D-STARのDVモードの音声パケットのフレームには音声セグメントの他にもデータセグメントが用意されており、  
音声と同時にユーザーが任意のデータを送ることができます。

D-PRS(D-STAR Packet Reporting System)はこのデータ部分を利用したもので、
送信元のGPSの位置情報等をAPRSのサーバーに引き渡すことができます。  

APRSには気象局というカテゴリがあり、位置情報に気温や気圧のデータを付加して送信すると  
インターネットの地図サイト上に無線局の送信位置の気象情報を登録することができます。  

このプログラムは IC-705 と 気象センサ、およびArduinoとUSB Host Shieldを組み合わせて  
D-PRSの気象局を構築したものです。  

![](https://github.com/7m4mon/prs_usb_bme280/blob/master/dprs_bme280_block_sch.png)

![](https://github.com/7m4mon/prs_usb_bme280/blob/master/dprs_bme280_inside.jpg)
