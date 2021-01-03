/**
 * dprs_usb_bme280
 * 
 * USB Host Sield と BME280 を組み合わせて
 * ArduinoからIC-705にD-PRSの気象データを送るプログラム
 *
 * USB_Host_Shield_2.0 Library (GPLv2) 
 * https://github.com/felis/USB_Host_Shield_2.0
 * と
 * SparkFun_BME280_Arduino_Library (MIT) 
 * https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
 * を 使用しています。
 * 
 * @author     7M4MON <github.com/7m4mon>
 * @date       17, Dec, 2020
 * @license    GPL v2
**/

#include <cdcacm.h>
#include <usbhub.h>
#include <Adafruit_SleepyDog.h>

#include "pgmstrings.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

//for BME280
#include <Wire.h>
#include "SparkFunBME280.h"
BME280 mySensor;

//#define SER_RCV /* for SerialReceive */
char wxStr[] = ".../...g...t...r...p...P...h..b.....\r\n"; // +\0 Fixed length 40 bytes
class ACMAsyncOper : public CDCAsyncOper
{
public:
    uint8_t OnInit(ACM *pacm);
};

uint8_t ACMAsyncOper::OnInit(ACM *pacm)
{
    uint8_t rcode;
    // Set DTR = 1 RTS=1
    rcode = pacm->SetControlLineState(3);

    if (rcode)
    {
        ErrorMessage<uint8_t>(PSTR("SetControlLineState"), rcode);
        return rcode;
    }

    LINE_CODING	lc;
    lc.dwDTERate	= 115200;   
    lc.bCharFormat	= 0;
    lc.bParityType	= 0;
    lc.bDataBits	= 8;

    rcode = pacm->SetLineCoding(&lc);

    if (rcode)
        ErrorMessage<uint8_t>(PSTR("SetLineCoding"), rcode);

    return rcode;
}

USB           Usb;
USBHub        Hub(&Usb);       // IC-705は内部のUSBハブ経由でcdc_acmが接続されている。（もう一方はオーディオ)
ACMAsyncOper  AsyncOper;
ACM           Acm(&Usb, &AsyncOper);

void setup()
{
  Serial.begin( 115200 );
  Serial.println("Start");

  /* USB Setup */
  Usb.Init();
  if (Usb.Init() == -1){
      Serial.println("OSCOKIRQ failed to assert");
      while(1); //Freeze
  }

  /* BME280 Setup */
  mySensor.setI2CAddress(0x76);
  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while(1); //Freeze
  }else{
     make_wx_str();  // センサから読み取って気象データの文字列を作成する。
  }
}

// センサから読み取って気象データの文字列を作成する。
void make_wx_str(){
    /* .../...g...t077r...p...P...h50b09900 */
    char  strBuff[10];
    sprintf(strBuff, "%03d", (int)mySensor.readTempF());     // 77.77 -> 077
    memcpy(&wxStr[12], &strBuff[0], 3);     // 該当の部分だけを差替
    sprintf(strBuff, "%02d", (int)mySensor.readFloatHumidity());     //7.77 -> 07
    memcpy(&wxStr[28], &strBuff[0], 2);     // 該当の部分だけを差替
    sprintf(strBuff, "%05d", (int)(mySensor.readFloatPressure()/10));   //1010800 -> 10108
    memcpy(&wxStr[31], &strBuff[0], 5);     // 該当の部分だけを差替

    Serial.print("wxStr: ");
    Serial.println(wxStr);
}

void loop()
{
    Usb.Task();
    if( Acm.isReady()) {
        make_wx_str();  // センサから読み取って気象データの文字列を作成する。
        uint8_t rcode;
        // Acm.SndData は送った後にポインタの中身をクリアするので
        // 別途バッファを用意する。
        char usbSndBuff[40];
        strcpy(usbSndBuff, wxStr);
        
        //rcode = Acm.SndData(39, usbSndBuff);  
        // 一度にまとめて送っても反映されないので1文字ずつ送る。
        // サンプルプログラムと同じ。
        Serial.print("wxSend: ");
        for(uint8_t i = 0; i <38; i++){
             Serial.print(wxStr[i], HEX);
             Serial.print(' ');
             delay(10);
             rcode = Acm.SndData(1, &usbSndBuff[i]);
             if (rcode){
                ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
                Serial.println("SndData error");
             }
        } //for
        Serial.println();

        #ifdef SER_RCV
        /* reading from usb device */
        /* buffer size must be greater or equal to max.packet size */
        /* it it set to 64 (largest possible max.packet size) here, can be tuned down
        for particular endpoint */
        // 受け取り側はサンプルプログラムのまま。
        uint8_t  buf[64];
        uint16_t rcvd = 64;
        rcode = Acm.RcvData(&rcvd, buf);
         if (rcode && rcode != hrNAK)
            ErrorMessage<uint8_t>(PSTR("Ret"), rcode);

            if( rcvd ) { //more than zero bytes received
              for(uint16_t i=0; i < rcvd; i++ ) {
                Serial.print((char)buf[i]); //printing on the screen
              }
            }
        #endif
        Watchdog.sleep(500);       // delay:8.5mA → WDT.Sleep:7.1mA @12V
    }       //if( Usb.getUsbTaskState() == USB_STATE_RUNNING..
    else{   //Acm.isNotRady
        delay(10);
    }
}
