#include <Alfredo_NoU2.h>
#include <AlfredoConnect.h>
#include <BluetoothSerial.h>

NoU_Motor LeftDrive(6);
NoU_Motor RightDrive(1);
NoU_Motor Intake_Roller(3);
NoU_Motor Shooter_Flywheel(2);
NoU_Motor Climber(4);
NoU_Servo Intake(1);



NoU_Servo Arm_Test(2);

NoU_Drivetrain drivetrain(&LeftDrive, &RightDrive);
BluetoothSerial bluetooth;


void setup() {
  //Setup Code
  bluetooth.begin("Viper 2.0");
  AlfredoConnect.begin(bluetooth);
  bluetooth.println("Starting motor and servo test program.");
  RSL::initialize();
  RSL::setState(RSL_ENABLED);

  //Shooter Invert
  Shooter_Flywheel.setInverted(true);
}

//bool creation for all autos
bool auto_2 = true;
bool auto_3 = true;
bool auto_4 = true;
bool auto_5 = true;
bool test = true;
bool drive = true;
bool aim = true;


void loop() {
  //Loop
  int servoAngle = 0; //intake servo variable
 
  //climber
  int n20_up = 0; //n20 speed variable (O command)
  int n20_down = 0; //n20 speed variable (P command)


  //Joystick Drive
  if (AlfredoConnect.getGamepadCount() >= 1) {
    float throttle = -AlfredoConnect.getAxis(0, 1);
    float rotation = -AlfredoConnect.getAxis(0, 2);

    drivetrain.arcadeDrive(rotation, throttle); //lets robot drive

    RSL::setState(RSL_ENABLED);
  } else {
    RSL::setState(RSL_DISABLED);
  }


  if (AlfredoConnect.buttonHeld(0,2)) {
    servoAngle = 90;
    Intake.write(servoAngle);
  }

  if (AlfredoConnect.buttonHeld(0, 1)) {
    servoAngle = -10;
    Intake.write(servoAngle);
  }

  if (AlfredoConnect.buttonHeld(0, 3)) {
    servoAngle = -20;
    Intake.write(servoAngle);
  }

  if (AlfredoConnect.buttonHeld(0, 0)) {
    servoAngle = 20;
    Intake.write(servoAngle);
  }



  //intake roller control
  if (AlfredoConnect.buttonHeld(0, 5)) {
    Intake_Roller.setInverted(true);
    Intake_Roller.set(-1);
  }
  else if (AlfredoConnect.buttonHeld (0, 4)) {
    Intake_Roller.set(1);
  }
  else {
    Intake_Roller.set(0);
  }



  //shooter control
  if (AlfredoConnect.buttonHeld(0, 7)) {
    Shooter_Flywheel.set(.88);
  }
  else if (AlfredoConnect.buttonHeld(0, 6)) {
    Shooter_Flywheel.set(-1);
  }
  else {
    Shooter_Flywheel.set(0);
  }



  //climber control > If statement below
  if (AlfredoConnect.buttonHeld(0, 12)) {
    n20_down = 1;
  }
  else {
    n20_down = 0;
  }

  if (AlfredoConnect.buttonHeld(0, 13)) {
    Climber.setInverted(true);
    n20_up = 1;
  }
  else {
    n20_up = 0;
    Climber.setInverted(false);
  }



  //climber if statement variable control
  if (n20_up > n20_down) {
    Climber.set(n20_up);
  }
  else if (n20_down > n20_up) {
    Climber.set(n20_down);
  }
  else {
    Climber.set(0);
  }

  //End of Match Climber continuation
  if (AlfredoConnect.buttonHeld(0, 8)) {
    Climber.set(-1);
    delay(20000);
    Climber.set(0); 
  }

  //autos
  //auto depending on position and defense ahead

  //2 and 5: start back corner of the defense line up
  // 3 and 4: start middle of box, little more forward then 2 and 5
  
  //defense #2
  if (auto_2 && AlfredoConnect.keyHeld(Key::Digit2)) {
    drivetrain.arcadeDrive(0, 1);
    delay(1200); //orginial  800
    drivetrain.arcadeDrive(-1, 1);
    delay(400);
    drivetrain.arcadeDrive(0, 0);
    
    //shoot sequence start
    Shooter_Flywheel.set(.83);
    delay(500);
    Shooter_Flywheel.set(.85);
    Intake.write(-20);
    delay(500);
    Intake_Roller.set(1);
    delay(1000);
    //end things
    Intake_Roller.set(0);
    Shooter_Flywheel.set(0);
    //end of shoot sequence;
    
    drivetrain.arcadeDrive(0, 0);

    auto_2 = false;
  }

   //defense #3
   if (auto_3 && AlfredoConnect.keyHeld(Key::Digit3)) {
     drivetrain.arcadeDrive(0, 1);
     delay (900);
     drivetrain.arcadeDrive(-1, 1);
     delay(200);
     drivetrain.arcadeDrive(0, 0);

    //shoot sequence start
    Shooter_Flywheel.set(.84);
    delay(500);
    Shooter_Flywheel.set(.84);
    Intake.write(-20);
    delay(500);
    Intake_Roller.set(1);
    delay(1000);
    //end things
    Intake_Roller.set(0);
    Shooter_Flywheel.set(0);
    //end of shoot sequence;
    
    drivetrain.arcadeDrive(0, 0);

     auto_3 = false;
   }



  //defense #4
  if (auto_4 && AlfredoConnect.keyHeld(Key::Digit4)) {
    drivetrain.arcadeDrive(0, 1);
    delay (900);
    drivetrain.arcadeDrive(1, 1);
    delay (200);
    drivetrain.arcadeDrive(0, 0);
    
    //shoot sequence start
    Shooter_Flywheel.set(.84);
    delay(500);
    Shooter_Flywheel.set(.84);
    Intake.write(-20);
    delay(500);
    Intake_Roller.set(1);
    delay(1000);
    //end things
    Intake_Roller.set(0);
    Shooter_Flywheel.set(0);
    //end of shoot sequence;
    
    drivetrain.arcadeDrive(0, 0);
    auto_4 = false;
    //auto done
  }

  //defense #5
  if (auto_5 && AlfredoConnect.keyHeld(Key::Digit5)) { 
    drivetrain.arcadeDrive(0, 1);
    delay(1100); //rock wall setup
    drivetrain.arcadeDrive(1, 1);
    delay(600);
    drivetrain.arcadeDrive(0, 0);
    
    //shoot sequence start
    Shooter_Flywheel.set(.83);
    delay(500);
    Shooter_Flywheel.set(.85);
    Intake.write(-20);
    delay(500);
    Intake_Roller.set(1);
    delay(1000);
    //end things
    Intake_Roller.set(0);
    Shooter_Flywheel.set(0);
    //end of shoot sequence;
    
    drivetrain.arcadeDrive(0, 0);

    auto_5 = false;
    //auto done
  }

 //test auto
 if (test && AlfredoConnect.keyHeld(Key::P)) {
  drivetrain.arcadeDrive(0, 1);
  delay(1000);
  drivetrain.arcadeDrive(0, 0);
  
  Shooter_Flywheel.set(1);
  delay(500);
  Shooter_Flywheel.set(1);
  Intake_Roller.set(1);
  delay(1000);
    //end things
  Intake_Roller.set(0);
  Shooter_Flywheel.set(0);

  test = false;
 }

 if (AlfredoConnect.keyHeld(Key::R)) {
  auto_2 = true;
  auto_3 = true;
  auto_4 = true;
  auto_5 = true; 
  drive = true;
  aim = true;
 }

  //drive forward auto
  if (drive && AlfredoConnect.keyHeld(Key::W)) {
    drivetrain.arcadeDrive(0, 1);
    delay(2000);
    drivetrain.arcadeDrive(0, 0);

    drive = false;
  }


  if (aim && AlfredoConnect.keyHeld(Key::Space)) {
    drivetrain.arcadeDrive(0, -1);
    delay(185);
    drivetrain.arcadeDrive(0, 0);
    aim = false;
  }
 
 //rocks - .2 sec
 //moat - .2
 //big block - .4 sec
  //45/45 - .05
 
 //end of autos

 int servoAngletest = 0;

 if (AlfredoConnect.keyHeld(Key::S)) {
 
  servoAngletest = 0;
  Arm_Test.write(servoAngletest);
 }

 



 
  
  AlfredoConnect.update();
  RSL::update();
}
