#include <Alfredo_NoU2.h>
#include <AlfredoConnect.h>
#include <BluetoothSerial.h>


NoU_Motor Shooter_Right (1);
NoU_Motor Shooter_Right2 (2);
NoU_Motor Shooter_Left (3);
NoU_Motor Shooter_Left2 (4);
//change motor ports

BluetoothSerial bluetooth;



void setup() {
  
  bluetooth.begin("Shooter Prototype");
  AlfredoConnect.begin(bluetooth);

}

void loop() {
  
  if (AlfredoConnect.keyHeld(Key::W)) {
    Shooter_Right.set(.8);
    Shooter_Right2.set(.8);
    Shooter_Left.set(-.8);
    Shooter_Left2.set(-.8);

  } else {
    
     Shooter_Right.set(0);
    Shooter_Right2.set(0);
    Shooter_Left.set(0);
    Shooter_Left2.set(0);

  }

AlfredoConnect.update();
}
