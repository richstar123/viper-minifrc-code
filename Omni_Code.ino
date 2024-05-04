#include <AlfredoConnect.h>
#include <BluetoothSerial.h>
#include <Alfredo_NoU2.h>
#include <Math.h>
#define PI 3.14159265359

//drivetrain motor ports set
NoU_Motor frontLeftMotor(2);
NoU_Motor frontRightMotor(5);
NoU_Motor rearLeftMotor(1);
NoU_Motor rearRightMotor(3);


int backRightAngle = 45;
int frontRightAngle = 135;
int frontLeftAngle = 225;
int backLeftAngle = 315;


BluetoothSerial bluetooth;

void setup() {
  bluetooth.begin("Viper 3.0");
  AlfredoConnect.begin(bluetooth);

  frontLeftMotor.setInverted(false);
  frontRightMotor.setInverted(false);
  rearRightMotor.setInverted(false);
  rearLeftMotor.setInverted(false);
 
  RSL::initialize();
  RSL::setState(RSL_ENABLED);
}

double signumOf(double n){
  if(n > 0.0){
    return 1.0;
  }
  else{
    return -1.0;
  }
}

void loop() {
  
   if (AlfredoConnect.getGamepadCount() >= 1) {
      double xAxis = AlfredoConnect.getAxis(0, 0);
      double yAxis = -AlfredoConnect.getAxis(0, 1);
      double rotation = AlfredoConnect.getAxis(0, 2);

      drive2(xAxis, yAxis, rotation, false);
   }

AlfredoConnect.update();
RSL::setState(RSL_ENABLED);
    }


void drive2(double xSpeed, double ySpeed, double rotation, bool fieldOrient) {
  ySpeed *= 1;
  xSpeed *= -1;
  double driveAngle = 0.0;

  double frontLeftSpeed = 0;
  double frontRightSpeed = 0;
  double backLeftSpeed = 0;
  double backRightSpeed = 0;

  double yawAngle = 0;

  double throttle = sqrt((xSpeed*xSpeed) + (ySpeed*ySpeed));
  if (throttle > 1) {
    throttle = 1;
  }

  driveAngle = atan2(xSpeed, ySpeed) * (180/PI) - 90;  
  
  if(!fieldOrient){
    frontLeftSpeed = cos((frontLeftAngle - driveAngle) * 0.01745329251)*throttle;
    //bluetooth.println("front left: " + frontLeftSpeed);
    frontRightSpeed = cos((frontRightAngle - driveAngle) * 0.01745329251)*throttle;
    //bluetooth.println("front right: " + frontRightSpeed);
    backLeftSpeed = cos((backLeftAngle - driveAngle) * 0.01745329251)*throttle;
    //bluetooth.println("back Left: " + backLeftSpeed);
    backRightSpeed = cos((backRightAngle - driveAngle) * 0.01745329251)*throttle;
    //bluetooth.println("back right: " + backRightSpeed);
  }
  if(fieldOrient){
    frontLeftSpeed = cos(frontLeftAngle - driveAngle + yawAngle);
    frontRightSpeed = cos(frontRightAngle - driveAngle + yawAngle);
    backLeftSpeed = cos(backLeftAngle - driveAngle + yawAngle);
    backRightSpeed = cos(backRightAngle - driveAngle + yawAngle); 
  }

  frontLeftSpeed += rotation;
  frontRightSpeed += rotation;
  backLeftSpeed += rotation;
  backRightSpeed += rotation;

 double highestValue = 0.0;
 int highestMotor = 0;
 if(abs(frontLeftSpeed) > highestValue){
   highestValue = abs(frontLeftSpeed);
   highestMotor = 1;
 }
 if(abs(frontRightSpeed) > highestValue){
   highestValue = abs(frontRightSpeed);
   highestMotor = 2;
 }
 if(abs(backLeftSpeed) > highestValue){
   highestValue = abs(backLeftSpeed);
   highestMotor = 3;
 }
 if(abs(backRightSpeed) > highestValue){
   highestValue = abs(backRightSpeed);
   highestMotor = 4;
 }

 if(highestValue > 1){
   double amountOver = 0.0;
   switch(highestMotor){
      case 1:
        amountOver = abs(frontLeftSpeed) - 1;
        frontLeftSpeed = signumOf(frontLeftSpeed);

        frontRightSpeed -= amountOver*signumOf(frontRightSpeed);
        backLeftSpeed -= amountOver*signumOf(backLeftSpeed);
        backRightSpeed -= amountOver*signumOf(backRightSpeed);
        break;
     
     case 2:
        amountOver = abs(frontRightSpeed) - 1;
        frontRightSpeed = signumOf(frontRightSpeed);

        frontLeftSpeed -= amountOver*signumOf(frontLeftSpeed);
        backLeftSpeed -= amountOver*signumOf(backLeftSpeed);
        backRightSpeed -= amountOver*signumOf(backRightSpeed);
        break;
     
     case 3:
        amountOver = abs(backLeftSpeed) - 1;
        backLeftSpeed = signumOf(backLeftSpeed);

        frontRightSpeed -= amountOver*signumOf(frontRightSpeed);
        frontLeftSpeed -= amountOver*signumOf(frontLeftSpeed);
        backRightSpeed -= amountOver*signumOf(backRightSpeed);
        break;
    
     case 4:
        amountOver = abs(backRightSpeed) - 1;
        backRightSpeed = signumOf(backRightSpeed);

        frontRightSpeed -= amountOver*signumOf(frontRightSpeed);
        backLeftSpeed -= amountOver*signumOf(backLeftSpeed);
        frontLeftSpeed -= amountOver*signumOf(frontLeftSpeed);
        break;
   }
 }
  
  double fls = -sqrt(abs(frontLeftSpeed))*signumOf(frontLeftSpeed);
  double frs = -sqrt(abs(frontRightSpeed))*signumOf(frontRightSpeed);
  double bls = -sqrt(abs(backLeftSpeed))*signumOf(backLeftSpeed);
  double brs = -sqrt(abs(backRightSpeed))*signumOf(backRightSpeed);
  
  frontLeftMotor.set(fls);
  frontRightMotor.set(frs);
  rearLeftMotor.set(bls);
  rearRightMotor.set(brs);
}
