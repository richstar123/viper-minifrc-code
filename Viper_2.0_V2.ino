#include <AlfredoConnect.h>
#include <Keys.h>
#include <Alfredo_NoU2.h>
#include <BluetoothSerial.h>
#include <Adafruit_BNO08x.h>
#include <Math.h>
#define PI 3.14159265359

BluetoothSerial bluetooth;

#define BNO08X_RESET -1

float robotRotation = 0;
float joystickAngle = 0;
float fieldAngle = 0;
float skew = 0;

struct euler_t {
  float yaw;
  float pitch;
  float roll;
} ypr;

Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

long report_interval = 5000;

NoU_Motor frontLeft(2);
NoU_Motor frontRight(3);
NoU_Motor backLeft(1);
NoU_Motor backRight(5);

NoU_Drivetrain drivetrain(&frontLeft, &frontRight, &backLeft, &backRight);

NoU_Motor Intake(4);

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
  {48.0, 158.0}, //Cone mid
  {90.0, 118.0}, //Cube mid
  {160.0, 100.0}, //Cone low 
  {46.0, 165.0}, //Double Sub Vert Cone 
  {112.0, 94.0}, //Substation approach 
  {70.0, 0.0}, //Cone fling
  {160.0, 40.0}, //Substation grab single
  {93.0, 138.0}, //Cone floor grab
  {160.0, 68.0} //Cube floor grab
};
// ROBOT CODE BELOW

void quaternionToEuler(float qr, float qi, float qj, float qk, euler_t* ypr, bool degrees = false) {

  float sqr = sq(qr);
  float sqi = sq(qi);
  float sqj = sq(qj);
  float sqk = sq(qk);

  ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
  ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
  ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

  if (degrees) {
    ypr->yaw *= RAD_TO_DEG;
    ypr->pitch *= RAD_TO_DEG;
    ypr->roll *= RAD_TO_DEG;
  }
}

void setReports() {
  if (! bno08x.enableReport(SH2_GYRO_INTEGRATED_RV, report_interval)) {
    bluetooth.println("Could not enable stabilized remote vector");
  }
}

void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr, bool degrees = false) {
  quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

void setup() {
  // put your setup code here, to run once:

  backRight.setInverted(true);
  frontRight.setInverted(true);

  bluetooth.begin("Viper 3.0");
  AlfredoConnect.begin(bluetooth);

  while (!(AlfredoConnect.getGamepadCount() >= 1)) {
    delay(100);
    AlfredoConnect.update();
  }

  if (!bno08x.begin_I2C()) {
    bluetooth.println("Failed to find BNO08x chip. Restart to attempt reconnecting");
  }
  setReports();
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
  // put your main code here, to run repeatedly:
  float xVel = 0;
  float yVel = 0;
  float xVelPrime = 0;
  float yVelPrime = 0;
  float rotationSpeed = 0;

  // BNO085 code

  if (bno08x.wasReset()) {
    bluetooth.println("sensor was reset");
    setReports();
  }

  if (bno08x.getSensorEvent(&sensorValue)) {
      quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &ypr, true);

    // convert robot orientation into usable angle

    if (ypr.yaw < 0) {
      robotRotation = ypr.yaw + 360;
    }
    else {
      robotRotation = ypr.yaw;
    }

    robotRotation += fieldAngle;

    // wrap rotation

    if (robotRotation > 360) {
      robotRotation -= 360;
    }
    else if (robotRotation < 0) {
      robotRotation += 360;
    }

  }

  // Set the throttle of the robot based on what key is pressed
  if (AlfredoConnect.getGamepadCount() >= 1) {

      xVel = -AlfredoConnect.getAxis(0, 0);
      yVel = -AlfredoConnect.getAxis(0, 1);
      rotationSpeed = -AlfredoConnect.getAxis(0, 2);

      // convert joystick data into polar
      if (xVel != 0) {
        if (xVel >= 0 && yVel <= 0) { //Q1
          joystickAngle = atan(yVel / xVel) * -180 / PI;
        }
        else if (xVel <= 0 && yVel <= 0) { //Q2
          joystickAngle = atan(yVel / xVel) * -180 / PI + 180;
        }
        else if (xVel <= 0 && yVel >= 0) { //Q3
          joystickAngle = atan(yVel / xVel) * -180 / PI + 180;
        }
        else { //Q4
          joystickAngle = atan(yVel / xVel) * -180 / PI + 360;
        }
      }
      else {
        if (yVel >= 0) {
          joystickAngle = 270;
        }
        else {
          joystickAngle = 90;
        }
      }

      skew = joystickAngle - robotRotation; //determine difference between robot angle and joystick angle
      float joystickMag = sqrt(pow(xVel, 2) + pow(yVel, 2)); //find magnitude of joystick values

      /*
        bluetooth.print("Skew: ");
        bluetooth.println(skew);
        bluetooth.print("Joystick Direction: ");
        bluetooth.println(joystickAngle);
        bluetooth.print("Robot Orientation: ");
        bluetooth.println(robotRotation); */

      // set motor powers
      xVelPrime = joystickMag * cos(skew * PI / 180);
      yVelPrime = joystickMag * sin(skew * PI / 180);

      drivetrain.holonomicDrive(xVelPrime, yVelPrime, rotationSpeed);
    

    // field orientation reset
    if (AlfredoConnect.buttonHeld(0, 13)) {
      fieldAngle = -ypr.yaw;
    }


    RSL::setState(RSL_ENABLED);
  }
  else {
    drivetrain.curvatureDrive(0, 0);
    RSL::setState(RSL_DISABLED);
  }

    if (AlfredoConnect.buttonHeld(0, 6)) {
      Intake.set(-1);
    } else if (AlfredoConnect.buttonHeld(0, 7)) {
      Intake.set(1);
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
  
  
 if (Just_Score_Cube && AlfredoConnect.keyHeld(Key::P)) {
    setKnownArmPlacement(knownArmPlacements[CONE_HIGH]);
    delay(1000); 
    Intake.set(-1);
    delay(1000);
    Intake.set(0);
    delay(200);
    Just_Score_Cube = false;
  }

  if (Clean_Side && AlfredoConnect.keyHeld(Key::W)) {
    Engage = false;
  }

  if (Engage && AlfredoConnect.keyHeld(Key::E)) {
       setKnownArmPlacement(knownArmPlacements[CONE_HIGH]);
      delay(1000); 
      Intake.set(-1);
      delay(1000);
      Intake.set(0);
      delay(1000);
      
      drivetrain.holonomicDrive(0,1, 0);
      delay (1000);
      drivetrain.holonomicDrive(0,0, 0);
      setKnownArmPlacement(knownArmPlacements[STOWED]);
      

      Engage = false;
    
  }
 

  if (AlfredoConnect.keyHeld(Key::R)) {
    Engage = true;
    Clean_Side = true;
    Just_Score_Cone = true;
    Just_Score_Cube = true;
    Bump_Side = true;
  }  
  

  AlfredoConnect.update();
  RSL::update();
}

void setKnownArmPlacement(ArmPlacement targetPlacement) {
   m_shoulderLeft.write(180 - targetPlacement.m_shoulderAngle);
   m_shoulderRight.write(targetPlacement.m_shoulderAngle);
   m_wristServo.write(180 - targetPlacement.m_wristAngle);
   }
