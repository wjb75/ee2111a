#include <MeMCore.h>
#include "MeMCore.h"

MeUltrasonicSensor ultrasonic(PORT_2);  
MeDCMotor leftMotor(M1);                
MeDCMotor rightMotor(M2); 
MeLineFollower lineFinder(PORT_3); /* Line Finder module can only be connected to PORT_3, PORT_4, PORT_5, PORT_6 of base shield. */


#define TURN_SPEED 100      
#define TURN_DELAY 1150     
#define OBSTACLE_DISTANCE 3 


unsigned long lastTurnTime = 0;  
unsigned long AVOID_TIME = TURN_DELAY * 1.5;


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
    leftMotor.run(speed);
    rightMotor.run(speed);
}

// 左转 170°
void turnLeft(int speed, int delayTime) {
    Serial.println("Turning left 170°...");
    leftMotor.run(-speed);
    rightMotor.run(speed);
    delay(delayTime);
    stopMotors();
    delay(500);  
    lastTurnTime = millis();  
}


void stopMotors() {
    leftMotor.run(0);
    rightMotor.run(0);
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    float distance = getFilteredDistance();  
    Serial.print("Distance: ");
    Serial.println(distance);

    
    if (millis() - lastTurnTime < AVOID_TIME) {
        Serial.println("Recently turned, skipping obstacle check...");
        moveForward(100);
        return;
    }

    if (distance > 0 && distance <= OBSTACLE_DISTANCE) {  
        stopMotors();
        delay(300);
        
        
        float newDistance = getFilteredDistance();
        if (newDistance > 0 && newDistance <= OBSTACLE_DISTANCE) {
            Serial.println("Confirmed obstacle! Turning...");
            turnLeft(TURN_SPEED, TURN_DELAY); 
        } else {
            Serial.println("False alarm, continue moving...");
        }
    } else {
        moveForward(100); 
    }
}