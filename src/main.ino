/// Arduino FM receiver with TEA5767 https://www.electronicsblog.net/
#include <Wire.h>
//#include <LiquidCrystal.h>
#include "radioFM.h"
#include "SerialCommand.h"

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
SerialCommand sCmd;


//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()   {

  Serial.begin(115200);
  delay(3000);
  Serial.println("Inicializando...");
  Wire.begin();
  //lcd.begin(16, 2);
  Radio.init();

  sCmd.addCommand("fq", setFq_sCmd);
  sCmd.addCommand("at", autoTune);
  sCmd.addCommand("scu", autoScanUp);
  sCmd.addCommand("scd", autoScanDown);
  sCmd.addDefaultHandler(whatDyS);
  /// buttons

  pinMode(Button_next, INPUT);
  digitalWrite(Button_next, HIGH); //pull up resistor

  pinMode(Button_prev, INPUT);
  digitalWrite(Button_prev, HIGH); //pull up resistor

  pinMode(Button_scannext, INPUT);
  digitalWrite(Button_scannext, HIGH); //pull up resistor

  pinMode(Button_scanprev, INPUT);
  digitalWrite(Button_scanprev, HIGH); //pull up resistor

  Radio.init();
  delay(30);
  Serial.println("... starting");
  Serial.print(">");
}

void loop()
{

  sCmd.readSerial();

  ///// buttons read

  //////////// button_next//////////
  if (!digitalRead(Button_next)) {
    frequency = Radio.getFrecuency()+100000;
    Radio.setFrequency(frequency);
    Radio.updateStatus();
    updateStatus();
  };

  if (!digitalRead(Button_scannext)) {

    autoScanUp();

  };

  //////////// button_prev//////////
  if (!digitalRead(Button_prev)) {

    frequency = Radio.getFrecuency()-100000;
    Radio.setFrequency(frequency);
    Radio.updateStatus();
    updateStatus();

  };

  if (!digitalRead(Button_scanprev)) {

    ///scannnn DOWN
    autoScanDown();

  };

  ////////////////////
  delay(500);
}

void setFq_sCmd(){
  double aNumber;
  char *arg;


  arg = sCmd.next();
  if (arg != NULL)
  {
    Serial.print("Set frequency to: ");
    aNumber=atol(arg);
    Serial.print(aNumber/1000000);
    Serial.println(" MHz.");
    Radio.setFrequency(aNumber);
    Radio.updateStatus();
    delay(30);
    Radio.readStatus();
    updateStatus();
  }
  else{
    updateStatus();
  }
}

void autoTune(){
  double aNumber;
  char *arg;
  uint8_t initIfCounter = 0;
  uint8_t lastIfCounter = 0;

  bool lowInjection = Radio.isLowSideInjection();

  arg = sCmd.next();
  if (arg != NULL)
  {
    Serial.print("Tunning at ");
    aNumber=atol(arg);
    Serial.print(aNumber);
    Serial.print(" Hz. in ");
    if(Radio.isLowSideInjection())Serial.println("Low Side Injection");
    Serial.println("Hig Side Injection");
    Radio.setFrequency(aNumber);
    Radio.updateStatus();
    delay(30);
    Radio.readStatus();
    initIfCounter = Radio.getIfCounter();
    lowInjection = !lowInjection;
    (lowInjection)?Radio.setLowSideInjection():Radio.setHigSideInjection();
    Radio.setFrequency(aNumber);
    Radio.updateStatus();
    delay(30);
    Radio.readStatus();
    lastIfCounter = Radio.getIfCounter();
    if(!lowInjection) Serial.print("{LO:");
    else Serial.print("{HI:");
    Serial.print(",IFCOUNTER:");
    Serial.print(lastIfCounter);
    if(lowInjection) Serial.print("{LO:");
    else Serial.print("{HI:");
    Serial.print(",IFCOUNTER:");
    Serial.print(initIfCounter);
    Serial.println("}");
    if(initIfCounter  > 53 || initIfCounter <57){
      lowInjection = !lowInjection;
      (lowInjection)?Radio.setLowSideInjection():Radio.setHigSideInjection();
      Radio.setFrequency(aNumber);
      Radio.updateStatus();
      delay(30);
      Radio.readStatus();
      updateStatus();
    }
  }
  else{
    Serial.print("Need frequency in Hz...");
  }
}

void autoScanUp(){
  uint8_t loops = 0;

  Radio.setFrequency(Radio.getFrecuency()+100000);
  Radio.setSense(radioFM_senseHigh);
  Radio.scanUp();
  Radio.setScanMode();
  Radio.updateStatus();
  while(loops<10 && !Radio.isRf()){
    delay(30);
    Radio.readStatus();
    loops ++;
  }
  delay(30);
  Radio.readStatus();
  updateStatus();
}

void autoScanDown(){
  uint8_t loops = 0;
  
  Radio.setFrequency(Radio.getFrecuency()-100000);
  Radio.setSense(radioFM_senseHigh);
  Radio.scanDown();
  Radio.setScanMode();
  Radio.updateStatus();
  while(loops<10 && !Radio.isRf()){
    delay(30);
    Radio.readStatus();
    loops ++;
  }
  delay(30);
  Radio.readStatus();
  updateStatus();
}

void whatDyS(){
  Serial.println("available Commands:?");
  Serial.println("fq xxxxxxxxx //Set frequency x in Hz.");
  Serial.println("at xxxxxxxxx //Auto tune frecuency x in Hz.");
  Serial.println("scu //Auto scan up.");
  Serial.println("scd //Auto scan down.");
  Serial.println("fq //Prints Radio status information");
  Serial.println("PIN 30 to LOW //+100000 Hz.");
  Serial.println("PIN 31 to LOW //-100000 Hz.");
  Serial.println("PIN 32 to LOW //Auto scan up.");
  Serial.println("PIN 33 to LOW //Auto scan down.");
  Serial.println("Have a nice day!");
  Serial.print(">");
}

void updateStatus(){
  Radio.readStatus();
  Serial.println("*****************************************************");
  Serial.print("FRECUENCY: ");
  Serial.print(Radio.getFrecuency());
  Serial.println(" Hz.");
  if(Radio.isRf())Serial.print("RF ");
  else Serial.print("-- ");
  if(Radio.isStereo())Serial.print("STEREO ");
  else Serial.print("  MONO  ");
  Serial.print("Signal: ");
  Serial.print(Radio.getSignal());
  Serial.print("/16 IF:");
  Serial.print(Radio.getIfCounter());
  if(Radio.isHighSideInjection()) Serial.println(" HI");
  else Serial.println(" LO");
  Serial.println("*****************************************************");
}
