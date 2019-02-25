
#include "Arduino.h"
#include <Servo.h>

// #define SERIAL_DEBUG

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

#define SERVO_PIN   9
#define LED_PIN     11
#define TOUCH_PIN   10

#define ANGLE_MID 83
#define ANGLE_LEFT 20
#define ANGLE_RIGHT 160
#define ANGLE_STEP 1
#define SPEED_MS 30
#define HOLD_MS 1000

#define TOUCH_DETECT_MS   1500
#define TOUCH_RELEASE_MS  1000

#define TOUCH_NONE  0
#define TOUCH_ONE   1
#define TOUCH_TWO   2
#define TOUCH_THREE 3
#define TOUCH_HOLD  255

#define TOUCH_TIME  

unsigned long lastPatrolMs = 0;
const unsigned long  PATROL_WAIT_MS = 1800000;

void setup() {
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT); 
  pinMode(TOUCH_PIN, INPUT);
  Serial.begin(9600);
  Serial.println("Let's go Zaku Servo!\n\n");
  myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  delay(500);
  Serial.println("Go to Center");
  breath(2, 1);
  analogWrite(LED_PIN, 255);
  myservo.write(ANGLE_MID);              // tell servo to go to position in variable 'pos'
  breath(5,5);
  Serial.print(millis());
  Serial.print(" - Patrol in every ");
  Serial.print(PATROL_WAIT_MS);
  Serial.println(" ms");
  //patrol();
}

void loop() {
  if ((millis() - lastPatrolMs) > PATROL_WAIT_MS) {
    // make sure patrol in fixed period, not affected by touch action
    lastPatrolMs += PATROL_WAIT_MS;
    patrol();
  }
  uint8_t action = CheckTouchAction();
  switch (action) {
    
    case TOUCH_ONE:
      patrol();
      break;

    case TOUCH_TWO:
      breath_up(5,5);
      break;

    case TOUCH_THREE:
      breath_down(5,5);
      break;

    case TOUCH_HOLD:
      alert();
      break;

    default:
      break;
  }
}

void breath(int ms, uint8_t step) {
  breath_up(ms, step);
  breath_down(ms, step);
}

void breath_up(int ms, uint8_t step) {
  for (int i = 0; i < 255; i += step) {
    analogWrite(LED_PIN, i);
    delay(ms);
  }
  analogWrite(LED_PIN, 255);
}

void breath_down(int ms, uint8_t step) {
  for (int i = 255; i >= 0; i -= step) {
    analogWrite(LED_PIN, i);
    delay(ms);
  }
  analogWrite(LED_PIN, 0);
}

bool _lastStatus = false;
int _touchCount = 0;
unsigned long _touchReleaseMs = 0;
unsigned long _touchStartMs = 0;
bool _waitRelease = false;

bool IsTouchPressed() {
    pinMode(TOUCH_PIN, INPUT);
    return  digitalRead(TOUCH_PIN);
}

void ResetTouchAction() {
    _touchStartMs = 0;
    _touchReleaseMs = 0;
    _touchCount = 0;
    _waitRelease = false;
}


uint8_t CheckTouchAction() {

    uint8_t retCode = TOUCH_NONE;

    unsigned long currMs = millis();    // for safety, use same time in ms for all checking later
    bool currStatus = IsTouchPressed();

#ifdef SERIAL_DEBUG
    Serial.print("Current Status: ");
    Serial.println(currStatus);

    Serial.print("Digital Read: ");
    Serial.println(digitalRead(TOUCH_PIN));
#endif

    if (currStatus != _lastStatus) {
      #ifdef SERIAL_DEBUG        
        Serial.println("Status checked");
      #endif  
        if (currStatus) {
            _touchCount++;
            _touchReleaseMs = 0;
            if (!_touchStartMs) _touchStartMs = millis();
        } else {
            _touchReleaseMs = currMs;
        }
        _lastStatus = currStatus;
    }
    
    if (_touchStartMs) {
        // Touch detected
        if ((millis() - _touchStartMs) > TOUCH_DETECT_MS) {
            if (!_waitRelease) {
                _waitRelease = true;
                // End of detection
                if (_touchCount == 1) {
                    if (currStatus) {
                        // Long hold
                        retCode = TOUCH_HOLD;
                    } else {
                        // Single click
                        retCode = TOUCH_ONE;
                    }
                } else if (_touchCount == 2) {
                    // double click
                    retCode = TOUCH_TWO;
                } else {
                    // triple click
                    retCode = TOUCH_THREE;
                }
            } else {
                // must release for reasonable time to stop action
                if ((!currStatus) && ((currMs - _touchReleaseMs) > TOUCH_RELEASE_MS)) {
                    ResetTouchAction();
                }
            }
        }
    } 

    return retCode;  
}


void patrol() {
  Serial.print(millis());
  Serial.println(" - Start patrol");
  breath_up(10, 1);
  analogWrite(LED_PIN, 255);
  for (pos = ANGLE_MID; pos >= ANGLE_LEFT; pos -= ANGLE_STEP) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(SPEED_MS);                       // waits 15ms for the servo to reach the position
  }
  delay(HOLD_MS);
  for (pos = ANGLE_LEFT; pos <= ANGLE_RIGHT; pos += ANGLE_STEP) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(SPEED_MS);                       // waits 15ms for the servo to reach the position
  }
  delay(HOLD_MS);
  for (pos = ANGLE_RIGHT; pos >= ANGLE_MID; pos -= ANGLE_STEP) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(SPEED_MS );                       // waits 15ms for the servo to reach the position
  }
  delay(HOLD_MS / 4);
  breath_down(2, 3);
  delay(HOLD_MS / 4);
  breath(2, 3);
  delay(HOLD_MS / 4);
  breath(2, 3);
  delay(HOLD_MS / 4);
  breath_up(2, 3);
  delay(HOLD_MS / 4);
  breath_down(10, 1);
}



void alert() {
  Serial.print(millis());
  Serial.println(" - Alert");
  while (IsTouchPressed()) {
    breath(2,5);
  }
  ResetTouchAction();
}
