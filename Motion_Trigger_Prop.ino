/*
 * Hardware setup:
 * 
 */

// establish pin constants
const int sensorTrigPin = 14;          // sensorTrigPin tells distance sensor to start/stop a reading
const int sensorEchoPin = 15;          // sensorEchoPin is where distance sensor provides feedback
const int transistorGatePin = 38;      // transistorGatePin will flip the transistor to power the pneumatic solenoid valve's 12v power

// establish other constants
long ConsoleBps = 19200;
const int minTriggerDistance = 10;      // minimum distance, inches,  object must be away in order to trigger
const int maxTriggerDistance = 72;      // maximum distance, inches, object must be away in order to trigger
const int requiredHitCount = 2;       // number of matching hits from distance sensor to trigger rise
const int holdTriggerAPinMillis = 10000;       // number of milliseconds to maintain trigger pin signal
const int resetDelayMillis = 10000;       // number of milliseconds before prop can be triggered again after being triggered

// establish variables for ultrasonic distance sensor
long duration, inches, countOfHits, sensorATriggerTime;

boolean serialOn = true; //sometimes writing to Serial hangs board, so this flag turns off writing to Serial in log()
#define pollingLED RED_LED
#define triggeredLED GREEN_LED
#define BLED BLUE_LED

void setup() {
  // initialize serial communication bits per second:
  Serial.begin(ConsoleBps); 

  log("Setting output on transistorGatePin ()");
  pinMode(transistorGatePin, OUTPUT);

  // ensure triggerA is not going off
  digitalWrite(transistorGatePin, LOW);      
  log("transistorGatePin set to LOW voltage - ensuring it is off");

 //setup sensor pins
  pinMode(sensorTrigPin, OUTPUT);
  log("Sensor Trig pin set to output");
  pinMode(sensorEchoPin, INPUT);
  log("Sensor Echo pin set to input");

  sensorATriggerTime = 0;
  
  pinMode(pollingLED, OUTPUT);
  pinMode(triggeredLED, OUTPUT);
  
  //mark pollingLED on
  digitalWrite(pollingLED, HIGH);
  digitalWrite(BLED, HIGH);

  log("Finished setup()...");
}

void loop() {

  if(millis() - sensorATriggerTime > resetDelayMillis) {
    if(isSensorATriggered() )
    {
      log("setting LEDs to triggered mode...");
      //mark pollingLED off
      digitalWrite(pollingLED, LOW);
      //mark triggeredLED on
      digitalWrite(triggeredLED, HIGH);
     
      log("Activate trigger pin...");
      digitalWrite(transistorGatePin, HIGH);     // powers transistor and an LED so we can see it happen
     
      delay(holdTriggerAPinMillis);
      
      log("Deactivate trigger pin...");
      digitalWrite(transistorGatePin, LOW);      // power off transistor 
    } else {
      log("Not considered triggered");
      //mark pollingLED on
      digitalWrite(pollingLED, HIGH);
      //mark triggeredLED off
      digitalWrite(triggeredLED, LOW);
    }
  } else {
      log("in reset delay window");
  }
}



boolean isSensorATriggered() {
  countOfHits = 0;
  inches = getSensorDistance();
  log("Run 1: " + String(inches) + "in");
  if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
  }
  delay(1);
  inches = getSensorDistance();
  log("Run 2: " + String(inches) + "in");
  if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
  }
  delay(1);
  inches = getSensorDistance();
  log("Run 3: " + String(inches) + "in");
  if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
  }
  
  // print out distance to console
  log("Count of hits: " + String(countOfHits));
  
//   delay(2000);

 if(countOfHits >= requiredHitCount )
 {
    return true;
 }  
  return false; 
}

long getSensorDistance()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:  
  digitalWrite(sensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(sensorEchoPin, HIGH);

  // convert the time into a distance
  return microsecondsToInches(duration);
}

long microsecondsToInches(long microseconds)
{
// According to Parallax's datasheet for the PING))), there are
// 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
// second). This gives the distance travelled by the ping, outbound
// and return, so we divide by 2 to get the distance of the obstacle.
// See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
return microseconds / 74 / 2;
}

void log(String logLine)
{
  if(serialOn) Serial.println(String(millis()) + ": " + logLine);
}
