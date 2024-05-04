#include <AlfredoConnect.h>
#include <BluetoothSerial.h>
#include <Alfredo_NoU2.h>
#include <Adafruit_BNO08x.h>
#include <Math.h>
#define PI 3.14159265359
Adafruit_BNO08x gyro(-1);

//drivetrain motor ports set
NoU_Motor frontLeftMotor(2);
NoU_Motor frontRightMotor(5);
NoU_Motor rearLeftMotor(1);
NoU_Motor rearRightMotor(3);


int backRightAngle = 45;
int frontRightAngle = 135;
int frontLeftAngle = 225;
int backLeftAngle = 315;


//intake motor port set
NoU_Motor Intake(4);

//arm servos
//NoU_Servo Distal(1);
//NoU_Servo Shoulder_Left(2);
//NoU_Servo Shoulder_Right(3);

NoU_Servo m_shoulderLeft(2, 540, 2460);
NoU_Servo m_shoulderRight(3, 540, 2460);

NoU_Servo m_wristServo(1); 


//bools created for autos
bool Engage = true;
bool Clean_Side = true;
bool Just_Score_Cone = true;
bool Just_Score_Cube = true;
bool Bump_Side = true;

String gameState = "CUBE";

enum KnownArmPlacement {
  STOWED,
  CONE_HIGH,
  CUBE_HIGH,
  CONE_MID,
  CUBE_MID,
  CONE_LOW,
  SUB_DOUBLE_VERT_CONE,
  SUBSTATION_APPROACH,
  CONE_FLING,
  SUBSTATION_GRAB_SINGLE,
  CONE_FLOOR_GRAB,
  CUBE_FLOOR_GRAB
};

struct ArmPlacement {
  double m_shoulderAngle;
  double m_wristAngle;
};

const ArmPlacement knownArmPlacements[] = {
  {180.0, 0.0}, //Stowed
  {55.0, 130.0}, //Cone high
  {65.0, 120.0}, //Cube high
  {46.0, 169.0}, //Cone mid
  {90.0, 118.0}, //Cube mid
  {160.0, 100.0}, //Cone low 
  {46.0, 165.0}, //Double Sub Vert Cone 
  {112.0, 94.0}, //Substation approach 
  {70.0, 0.0}, //Cone fling
  {160.0, 40.0}, //Substation grab single
  {93.0, 143.0}, //Cone floor grab
  {160.0, 68.0} //Cube floor grab
};

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
    
    if (AlfredoConnect.buttonHeld(0, 7)) {
      Intake.set(1);
    } else if (AlfredoConnect.buttonHeld(0, 6)) {
      Intake.set(-1);
    } else {
      Intake.set(0.1);
    }
       
    
 


  
 // Game state code
    if (AlfredoConnect.buttonHeld(0, 8)) {
      gameState = "CUBE";
    } else if (AlfredoConnect.buttonHeld(0, 9)) {
      gameState = "CONE";
    }

    // Arm code
      
      if (AlfredoConnect.buttonHeld(0, 1)) {
        setKnownArmPlacement(knownArmPlacements[STOWED]);
      }


      if (AlfredoConnect.buttonHeld(0, 5)) {
         if (gameState.equals("CONE")) {
          setKnownArmPlacement(knownArmPlacements[CONE_HIGH]);
        } else if (gameState.equals("CUBE")) {
          setKnownArmPlacement(knownArmPlacements[CUBE_HIGH]);
        }
      }

      if (AlfredoConnect.buttonHeld(0, 4)) {
        if (gameState.equals("CONE")) {
          setKnownArmPlacement(knownArmPlacements[CONE_MID]);
        } else if (gameState.equals("CUBE")) {
          setKnownArmPlacement(knownArmPlacements[CUBE_MID]);
        }
      }

      if (AlfredoConnect.buttonHeld(0, 0)) {
        if (gameState.equals("CONE")) {
          setKnownArmPlacement(knownArmPlacements[CONE_FLOOR_GRAB]);
        } else if (gameState.equals("CUBE")) {
          setKnownArmPlacement(knownArmPlacements[CUBE_FLOOR_GRAB]);
        }
      }


      if (AlfredoConnect.buttonHeld(0, 2)) {
        if (gameState.equals("CONE")) {
          setKnownArmPlacement(knownArmPlacements[SUB_DOUBLE_VERT_CONE]);
        } else if (gameState.equals("CUBE")) {
          setKnownArmPlacement(knownArmPlacements[SUBSTATION_APPROACH]);
        }
      }

      if (AlfredoConnect.buttonHeld(0, 3)) {
        setKnownArmPlacement(knownArmPlacements[SUBSTATION_GRAB_SINGLE]);
      }

      if (AlfredoConnect.buttonHeld(0, 12)) {
        setKnownArmPlacement(knownArmPlacements[CONE_FLING]);
      }

     
      if (AlfredoConnect.buttonHeld(0, 14)) {
        setKnownArmPlacement(knownArmPlacements[CONE_LOW]);
      }

//autos  
  
  if (Engage && AlfredoConnect.keyHeld(Key::E)) {
    setKnownArmPlacement(knownArmPlacements[CONE_HIGH]);
    delay(1000); 
    Intake.set(-1);
    delay(1000);
    Intake.set(0);
    delay(200);
    setKnownArmPlacement(knownArmPlacements[STOWED]);
    drive2(-1, 0.3, 0.0, false);
    delay(2000);
    drive2(0.0, 0.0, 0.0, false);

    Engage = false;
  }

  if (Clean_Side && AlfredoConnect.keyHeld(Key::C)) {
    setKnownArmPlacement(knownArmPlacements[CONE_HIGH]);
    delay(10);
    drive2(1.0, 0.0, 0.2, false);
    delay(500);
    drive2(0, 0.0, 0.0, false);
    Intake.set(-1);
    delay(1000);
    Intake.set(0);
    Clean_Side = false;
  }

  if (Just_Score_Cone && AlfredoConnect.keyHeld(Key::Y)){

    setKnownArmPlacement(knownArmPlacements[CONE_HIGH]);
    delay(1000); 
    Intake.set(-1);
    delay(1000);
    Intake.set(0);
    Just_Score_Cone = false;
  }

  if (Just_Score_Cube && AlfredoConnect.keyHeld(Key::P)) {
    setKnownArmPlacement(knownArmPlacements[CONE_HIGH]);
    delay(1000); 
    Intake.set(-1);
    delay(1000);
    Intake.set(0);
    delay(200);
    Just_Score_Cube = false;
  }

  if (Bump_Side && AlfredoConnect.keyHeld(Key::B)) {

    Bump_Side = false;
  }


  if (AlfredoConnect.keyHeld(Key::R)) {
    Engage = true;
    Clean_Side = true;
    Just_Score_Cone = true;
    Just_Score_Cube = true;
    Bump_Side = true;
  }


  
AlfredoConnect.update();
RSL::setState(RSL_ENABLED);
    } 


void setKnownArmPlacement(ArmPlacement targetPlacement) {
   m_shoulderLeft.write(180 - targetPlacement.m_shoulderAngle);
   m_shoulderRight.write(targetPlacement.m_shoulderAngle);
   m_wristServo.write(180 - targetPlacement.m_wristAngle);
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
