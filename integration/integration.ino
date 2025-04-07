#include <MeMCore.h>

MeUltrasonicSensor ultrasonic(PORT_2);  
MeLineFollower lineFinder(PORT_5); /* Line Finder module can only be connected to PORT_3, PORT_4, PORT_5, PORT_6 of base shield. */
MeDCMotor leftMotor(M1);                
MeDCMotor rightMotor(M2);    
MeBuzzer buzzer;


#define TURN_SPEED 100      
#define TURN_DELAY 1500     
#define OBSTACLE_DISTANCE 8 
#define ALIGN_DELAY 100
#define NOTE_DS8 4978
#define NOTE_A3  220

#define OPAMP_PIN A1
#define FORCE_SENSOR A0
#define FORCE_DIGITAL 11
#define IR_DIGITAL 12


unsigned long lastTurnTime = 0;  
unsigned long AVOID_TIME = TURN_DELAY * 1.5;
int force = 0; // the reading from the force sensor
int ir = 0; //reading from the ir op amp sensor
int digipin11force;
int digipin12ir;


float getFilteredDistance(int numSamples = 3);
void moveForward(int speed);
void turnLeft(int speed, int delayTime);
void stopMotors();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(FORCE_DIGITAL, INPUT);
  pinMode(IR_DIGITAL, INPUT);
}

void loop() {

analogRead(A0);      // Dummy read to flush the ADC
delayMicroseconds(5); 
int a0 = analogRead(A0);
delayMicroseconds(5);          // A short delay for the ADC to settle
force = analogRead(FORCE_SENSOR);
  int a1 = analogRead(A1);
  int a2 = analogRead(A2);
  int a3 = analogRead(A3);

  Serial.print("A0 is ");
  Serial.println(A0);
    Serial.print("A1 is ");
  Serial.println(A1);
    Serial.print("A2 is ");
  Serial.println(A2);
    Serial.print("A3 is ");
  Serial.println(A3);

  digipin11force = digitalRead(11);
  
  digipin12ir = digitalRead(12);

  Serial.print("11 is");
  Serial.println(digipin11force);
  Serial.print("12 is");
  Serial.println(digipin12ir);


  // first check if the dust bag is full by reading the force sensors.
  force = analogRead(FORCE_SENSOR);
  Serial.println(force);
  //IF selection statement : do sth if force reading exceeds certain val

  if(digipin11force == 1){
    buzzer.tone(NOTE_DS8,20);
    stopMotors();
    return;
  }
  //buzzer.tone(NOTE_DS8,20);



  ir = analogRead(OPAMP_PIN); //check if the ir emitter is in certain distance ahead
  // do sth if the ir station is in front ( the voltage value is smaller than 2.5V)
  if(digipin12ir == 1){ 
    stopMotors();
    return;
  }


   int sensorState = lineFinder.readSensors();
   if(sensorState ==  S1_OUT_S2_OUT){
      //Serial.println("Sensor left(1) and right(2) are outside of black line. Safe to go!");
   }

   while (sensorState == S1_IN_S2_OUT || sensorState == S1_OUT_S2_IN  ) {
   
  
   switch(sensorState)
   {
    //case S1_IN_S2_IN:
    //  Serial.println("Sensor left(1) and right(2)  are on black line"); 
    //  break;
    case S1_OUT_S2_IN: 
      //Serial.println("Sensor right(2) is on black line");  //needs to turn right to align first
      turnLeft(-TURN_SPEED, ALIGN_DELAY);
      break;
    case S1_IN_S2_OUT: 
      //Serial.println("Sensor left(1) is on black line"); //needs to turn left to align first
      turnLeft(TURN_SPEED, ALIGN_DELAY);
      break;
    //case S1_OUT_S2_OUT: 
    //  Serial.println("Sensor left(1) and right(2) are outside of black line");
    //  break;
    default: break;
   }
   sensorState = lineFinder.readSensors();
  }

  if (sensorState == S1_IN_S2_IN)
  {
    delay(100);
    turnLeft(TURN_SPEED, TURN_DELAY); 

  }

    float distance = getFilteredDistance();  
    //Serial.print("Distance: ");
    //Serial.println(distance);


    if (distance > 0 && distance <= OBSTACLE_DISTANCE) {  
        stopMotors();
        delay(300);
        
        
        float newDistance = getFilteredDistance();
        if (newDistance > 0 && newDistance <= OBSTACLE_DISTANCE) {
            //Serial.println("Confirmed obstacle! Turning...");
            turnLeft(TURN_SPEED, TURN_DELAY); 
        } else {
            //Serial.println("False alarm, continue moving...");
        }
    } else {
        moveForward(100); 
    }
}


float getFilteredDistance(int numSamples = 3) {
    float totalDistance = 0;
    int validSamples = 0;

    for (int i = 0; i < numSamples; i++) {
        float d = ultrasonic.distanceCm();
        if (d > 0) {  
            totalDistance += d;
            validSamples++;
        }
        delay(50);  
    }

    return (validSamples > 0) ? totalDistance / validSamples : 0;  
}


void moveForward(int speed) {
    leftMotor.run(-speed);
    rightMotor.run(speed);
}

// 左转 170°
void turnLeft(int speed, int delayTime) {
    Serial.println("Turning left 170°...");
    leftMotor.run(speed);
    rightMotor.run(+speed);
    delay(delayTime);
    stopMotors();
    //delay(500);  
    lastTurnTime = millis();  
}


void stopMotors() {
    leftMotor.run(0);
    rightMotor.run(0);
}