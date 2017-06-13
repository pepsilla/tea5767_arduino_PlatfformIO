/// Arduino FM receiver with TEA5767 https://www.electronicsblog.net/
#include <Wire.h>
//#include <LiquidCrystal.h>
#include "radioFM.h"

unsigned char search_mode=0;

int b=0;
int c=0;

#define Button_next 30
#define Button_prev 31

#define Button_scannext 32
#define Button_scanprev 33

unsigned char frequencyH=0;
unsigned char frequencyL=0;

unsigned int frequencyB;
double frequency=0;

double freq_available=0;

radioFM Radio;


//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()   {

  Serial.begin(115200);
  delay(3000);
  Serial.print("Inicializando...");
  Wire.begin();
  //lcd.begin(16, 2);
  Radio.init();
  /// buttons

  pinMode(Button_next, INPUT);
  digitalWrite(Button_next, HIGH); //pull up resistor

  pinMode(Button_prev, INPUT);
  digitalWrite(Button_prev, HIGH); //pull up resistor

  pinMode(Button_scannext, INPUT);
  digitalWrite(Button_scannext, HIGH); //pull up resistor

  pinMode(Button_scanprev, INPUT);
  digitalWrite(Button_scanprev, HIGH); //pull up resistor

  frequency=96.6; //starting frequency

  frequencyB=4*(frequency*1000000+225000)/32768; //calculating PLL word

  frequencyH=frequencyB>>8;

  frequencyL=frequencyB&0XFF;

  delay(100);

  Wire.beginTransmission(0x60);   //writing TEA5767

  Wire.write(frequencyH);
  Wire.write(frequencyL);
  Wire.write(0xB0);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();

  delay(100);

}

void loop()
{

  unsigned char buffer[5];

  //lcd.setCursor(0, 0);

  Wire.requestFrom(0x60,5); //reading TEA5767

  if (Wire.available())

  {
    for (int i=0; i<5; i++) {

      buffer[i]= Wire.read();
    }

    freq_available=(((buffer[0]&0x3F)<<8)+buffer[1])*32768/4-225000;

    Serial.print("FM ");

    Serial.print((freq_available/1000000));

    frequencyH=((buffer[0]&0x3F));

    frequencyL=buffer[1];

    if (search_mode) {

      if(buffer[0]&0x80) search_mode=0;

    }

    if (search_mode==1) Serial.print(" SCAN");
    else {
      Serial.print("       ");
    }

    if(buffer[0]&0x80)Serial.print(" RF ");
    else Serial.print("    ");

    if(buffer[0]&0x40)Serial.print(" BLF ");
    else Serial.print("     ");

    Serial.print(" IF:");
    Serial.print((buffer[2]&0x7f));

    //lcd.setCursor(0, 1);
    Serial.println();

    Serial.print("Level: ");
    Serial.print((buffer[3]>>4));
    Serial.print("/16 ");

    if (buffer[2]&0x80) Serial.print("STEREO   ");
    else Serial.print("MONO   ");

    Serial.println();
    delay(1000);

  }

  ///// buttons read

  //////////// button_next//////////
  if (!digitalRead(Button_next)) {

    frequency=(freq_available/1000000)+0.1;

    frequencyB=4*(frequency*1000000+225000)/32768+1;

    frequencyH=frequencyB>>8;
    frequencyL=frequencyB&0XFF;

    Wire.beginTransmission(0x60);

    Wire.write(frequencyH);
    Wire.write(frequencyL);
    Wire.write(0xB0);
    Wire.write(0x1F);
    Wire.write(0x00);

    Wire.endTransmission();

    //////////////////////

    //b=100;
    //delay(500);

  };

  if (!digitalRead(Button_scannext)) {

    ///scannnn UP

    search_mode=1;

    Wire.beginTransmission(0x60);

    Wire.write(frequencyH+0x40);
    Wire.write(frequencyL);
    Wire.write(0xD0);
    Wire.write(0x1F);
    Wire.write(0x00);

    Wire.endTransmission();

    /////////////////

    b=0;
    delay(1000);

  };

  b=0;

  //////////// button_prev//////////
  if (!digitalRead(Button_prev)) {

    frequency=(freq_available/1000000)-0.1;

    frequencyB=4*(frequency*1000000+225000)/32768+1;

    frequencyH=frequencyB>>8;
    frequencyL=frequencyB&0XFF;

    Wire.beginTransmission(0x60);

    Wire.write(frequencyH);
    Wire.write(frequencyL);
    Wire.write(0xB0);
    Wire.write(0x1F);
    Wire.write(0x00);

    Wire.endTransmission();

    //c=100;

  };

  if (!digitalRead(Button_scanprev)) {

    ///scannnn DOWN

    search_mode=1;

    Wire.beginTransmission(0x60);

    Wire.write(frequencyH+0x40);
    Wire.write(frequencyL);

    Wire.write(0x50);
    Wire.write(0x1F);
    Wire.write(0x00);
    Wire.endTransmission();
    delay(1000);
    //c=100;

  };

  c=0;

  ////////////////////

}