#include <MeMCore.h>

MeUltrasonicSensor ultrasonic(PORT_3);  
MeDCMotor leftMotor(M1);                
MeDCMotor rightMotor(M2);               

#define TURN_SPEED 100      
#define TURN_DELAY 1150     
#define OBSTACLE_DISTANCE 3 


unsigned long lastTurnTime = 0;  
unsigned long AVOID_TIME = TURN_DELAY * 1.5;


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
