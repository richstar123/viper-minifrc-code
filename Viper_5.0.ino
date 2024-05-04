#include <Alfredo_NoU2.h>
#include <AlfredoConnect.h>
#include <BluetoothSerial.h>






BluetoothSerial bluetooth;

void setup() {
  // put your setup code here, to run once:
  bluetooth.begin("Viper 5.0");
  AlfredoConnect.begin(bluetooth);
  RSL::initialize();
  RSL::setState(RSL_ENABLED);
}

void loop() {
// put your main code here, to run repeatedly:


  
}
