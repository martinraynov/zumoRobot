#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
/*
 * This example uses the ZumoMotors library to drive each motor on the Zumo
 * forward, then backward. The yellow user LED is on when a motor should be
 * running forward and off when a motor should be running backward. If a
 * motor on your Zumo has been flipped, you can correct its direction by
 * uncommenting the call to flipLeftMotor() or flipRightMotor() in the setup()
 * function.
 */
#define QTR_THRESHOLD  1000
#define LED_PIN        13
#define FORWARD_SPEED     200
#define SUPERFORWARD_SPEED     400
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        150
#define FORWARD_SPEED     100
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     400 // ms
#define MELODY_LENGTH     12

const int trigPin = 8;
const int echoPin = 11;

unsigned char note[MELODY_LENGTH] = 
{
  NOTE_E(5), SILENT_NOTE, NOTE_E(5), SILENT_NOTE, NOTE_E(5), SILENT_NOTE, NOTE_C(5), NOTE_E(5),
  NOTE_G(5), SILENT_NOTE, NOTE_G(4), SILENT_NOTE,
};

unsigned int duration[MELODY_LENGTH] =
{
  100, 25, 125, 125, 125, 125, 125, 250, 250, 250, 250, 250
};

const char march[] PROGMEM = 
"! O2 T100 MS"
"a8. r16 a8. r16 a8. r16 f8 r16 >c16" 
"ML"
"a8. r16 f8 r16" "MS" ">c16 a. r8"
"O3"
"e8. r16 e8. r16 e8. r16 f8 r16 <c16"
"O2 ML"
"a-8. r16" "MS" "f8 r16 >c16 a r"

"O3 ML"
"a8. r16 <a8 r16 <a16" "MS" "a8. r16 a-8 r16 g16"
"ML V10"
"g-16 f16 g-16 r16 r8 <b-16 r16" "MS" "e-8. r16 d8." "ML" "d-16"
"c16 <c-16 c16 r16 r8" "MS O2" "f16 r16 a-8. r16 f8. a-16"
"O3"
"c8. r16 <a8 r16 c16 e2. r8"

"O3 ML"
"a8. r16 <a8 r16 <a16" "MS" "a8. r16 a-8 r16 g16"
"ML V10"
"g-16 f16 g-16 r16 r8 <b-16 r16" "MS" "e-8. r16 d8." "ML" "d-16"
"c16 <c-16 c16 r16 r8" "MS O2" "f16 r16 a-8. r16 f8. >c16"
"ML"
"a8. r16 f8 r16 >c16 a2. r8"
;

Pushbutton button(ZUMO_BUTTON);   
ZumoBuzzer buzzer;
ZumoMotors motors;

#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

void waitForButtonAndCountDown()
{
  digitalWrite(LED_PIN, HIGH);
  button.waitForButton();
  digitalWrite(LED_PIN, LOW);
   
  // play audible countdown
//  for (int i = 0; i < 3; i++)
//  {
//    delay(1000);
//    buzzer.playNote(NOTE_G(3), 200, 15);
//  }
//  delay(1000);
//  buzzer.playNote(NOTE_G(4), 500, 15);  
  playStartMusic();
  delay(1125);
  //buzzer.playFromProgramSpace(march);
}

void setup()
{
  Serial.begin(9600);
  // uncomment one or both of the following lines if your motors' directions need to be flipped
  //motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  waitForButtonAndCountDown();
}

void loop()
{
  if (button.isPressed())
  {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    waitForButtonAndCountDown();
  }

// 
// Capteur IR 
//
  
  sensors.read(sensor_values);
  Serial.println(sensor_values[0]);
  Serial.println(sensor_values[5]);
  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
//    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
//    delay(REVERSE_DURATION);
//    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
//    delay(TURN_DURATION);
//    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    if (!buzzer.isPlaying()) {
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(1000);
      buzzer.playFromProgramSpace(march);
    }
    motors.setSpeeds(0, 0);
  }
  else if (sensor_values[5] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
//    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
//    delay(REVERSE_DURATION);
//    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
//    delay(TURN_DURATION);
//    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    if (!buzzer.isPlaying()) {
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(1000);
      buzzer.playFromProgramSpace(march);
    }
    motors.setSpeeds(0, 0);
  }
  else if (!buzzer.isPlaying())
  {
    //
    //
    // Capteur ultrason
    //
    //
    long duration, distance;
    
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
  
    if( distance < 2000) {
      if(distance < 20)
      {
        buzzer.stopPlaying();
        motors.setSpeeds(SUPERFORWARD_SPEED, SUPERFORWARD_SPEED);
        delay(500);
      } else {
        buzzer.stopPlaying();
        motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      }
    }
  }
}

void playStartMusic() {
  int currentIdx;
  for (currentIdx = 0; currentIdx < MELODY_LENGTH ;currentIdx++)
  {
    // play note at max volume
    buzzer.playNote(note[currentIdx], duration[currentIdx], 15);
    delay(duration[currentIdx]);
  }
}
