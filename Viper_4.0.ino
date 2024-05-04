#include <Alfredo_NoU2.h>
#include <AlfredoConnect.h>
#include <BluetoothSerial.h>


NoU_Motor LeftDrive (2);
NoU_Motor RightDrive (1);

NoU_Motor Intake(4);

NoU_Servo Arm (1);
BluetoothSerial bluetooth;

NoU_Drivetrain drivetrain(&LeftDrive, &RightDrive);

void setup() {
  // put your setup code here, to run once
  bluetooth.begin("Viper 4.0");
  AlfredoConnect.begin(bluetooth);
  RSL::initialize();
  RSL::setState(RSL_ENABLED);
}


bool three_cube = true;


void loop() {

  int servoAngle = 0;

  
  // put your main code here, to run repeatedly:
  if (AlfredoConnect.getGamepadCount() >= 1) {
    float throttle = AlfredoConnect.getAxis(0, 1);
    float rotation = -AlfredoConnect.getAxis(0, 2);

    drivetrain.curvatureDrive(rotation, throttle); //lets robot drive
    RSL::setState(RSL_ENABLED);
  } else {
    RSL::setState(RSL_DISABLED);
  }

  


  if (AlfredoConnect.buttonHeld(0, 0)) {
    servoAngle = 172;
    Arm.write(servoAngle);
  }

  if (AlfredoConnect.buttonHeld(0, 3)) {
    servoAngle = 90;
    Arm.write(servoAngle);
  }

  if (AlfredoConnect.buttonHeld(0, 2)) {
    servoAngle = 45;
    Arm.write(servoAngle);
  }

  if (AlfredoConnect.buttonHeld(0, 1)) {
    servoAngle = 130;
    Arm.write(servoAngle);
  }

  

  if (AlfredoConnect.buttonHeld(0, 6)) {
    Intake.set(1);
  } else if (AlfredoConnect.buttonHeld(0, 7)){
    Intake.set(-1);
  } else {
    Intake.set(0);
  }



  //autos

  if (three_cube && AlfredoConnect.keyHeld(Key::W)) {
    servoAngle = 45;
    Arm.write(servoAngle); //Set arm to back for outtake
    delay(500);
    Intake.set(1); //Starts outtake
    delay(500);
    drivetrain.curvatureDrive(0, -1);
    delay(500);
    drivetrain.curvatureDrive(0, -0.8);
    servoAngle = 172;
    Arm.write(servoAngle); // Sets arm to ground
    Intake.set(-1); //slows down for approach to cube 1
    delay(1800);
    drivetrain.curvatureDrive(0, .6);
    delay(500); 
    servoAngle = 45; //
    Arm.write(servoAngle);
    drivetrain.curvatureDrive(-.0142, .8); //starts to back up then swaps to a curved drive to get close to the low node
    delay(1500);
    drivetrain.curvatureDrive(.01, .8);
    delay(1300);
    drivetrain.curvatureDrive(0, .65);
    delay(500);
    Intake.set(1);
    delay(600);
    drivetrain.curvatureDrive(0, 0);
    servoAngle = 45;
    Arm.write(servoAngle);
    drivetrain.curvatureDrive(0, -.9);
    delay(1100);
    Intake.set(-1);
    servoAngle = 172;
    Arm.write(servoAngle);
    drivetrain.curvatureDrive(.0375, -.75);
    delay(1600);
    drivetrain.curvatureDrive(0, 0);
    delay(400);
    servoAngle = 45;
    Arm.write(servoAngle);
    drivetrain.curvatureDrive(-0.036, .8);
    delay(2000);
    drivetrain.curvatureDrive(0, .7);
    delay(750);
    drivetrain.curvatureDrive(0, 0);
    Intake.set(1);
    delay(500);
    Intake.set(0);
    

    three_cube = false;
  }

  if (AlfredoConnect.keyHeld(Key::E)) {
    servoAngle = 45;
    Arm.write(servoAngle);
    delay(500);
    Intake.set(1);
    delay(500);
    drivetrain.curvatureDrive(0, 1);
    delay(900);
    drivetrain.curvatureDrive(0, 0);
  }

  if (AlfredoConnect.keyHeld(Key::R)) {
    three_cube = true;
  }

  if (AlfredoConnect.keyHeld(Key::T)) {
    drivetrain.curvatureDrive(.01, 1);
  }
  

  





AlfredoConnect.update();
RSL::update();
}
