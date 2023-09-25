// C++ code
// References: https://docs.arduino.cc/built-in-examples/sensors/Ping

//			   https://www.tinkercad.com/projects/Ultrasonic-Distance-Sensor-Arduino-Tinkercad

//			   https://www.tinkercad.com/things/3kCYZfx9fXN-multiple-leds-/
//             editel?lessonid=ELB4FYAJD0K8TZV&projectid=O94BCSIL26F9EV3&collectionid=O0K87SQL1W5N4P2&tenant=circuits#/lesson-viewer

//			   https://www.tinkercad.com/things/8ZN0hPJWOfw-ultrasonic-distance-sensor/
//			   editel?lessonid=EAYGNT8JL6UYXE2&projectid=O3TAWASL26F9H1J&collectionid=O0K87SQL1W5N4P2#/lesson-viewer

const byte SIGNAL_PIN = 7;
const byte LED_GREEN = 2;
const byte LED_YELLOW = 3;
const byte LED_RED = 4;

int distanceThreshold = 300;
int distance = 0;

byte ledState_GREEN = LOW;
byte ledState_YELLOW = LOW;
byte ledState_RED = LOW;

byte threatStatus = 0;

long readUltrasonicDistance(int SIGNAL_PIN)
{
  pinMode(SIGNAL_PIN, OUTPUT); //clears the signal
  digitalWrite(SIGNAL_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(SIGNAL_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SIGNAL_PIN, LOW);
  pinMode(SIGNAL_PIN, INPUT);
  // Reads the signal pin, and returns the sound wave travel time in microseconds
  
  distance = 0.01723 * pulseIn(SIGNAL_PIN, HIGH);  
  
  handleDetection(distance);
}

void updateStatus(byte green, byte yellow, byte red)
{
	digitalWrite(LED_GREEN, green);
  	digitalWrite(LED_YELLOW, yellow);
  	digitalWrite(LED_RED, red);
}

void threatAssessment()
{
  switch(threatStatus)
  {
    case 0: 
    	Serial.print("Nothing Detected: ");
    	Serial.println("All is well!");
    break;
    case 1:
    	Serial.print("Object Detected at: ");
    	Serial.println(distance);
    break;
    case 2:
    	Serial.print("Warning, Object Getting Closer at: ");
    	Serial.println(distance);
    break;
    case 3:
    	Serial.println("ALERT!! ALERT!! ALERT!!");
		Serial.print("Possible Threat to Security Now at: ");
        Serial.println(distance);
    break;
  } 
  
}

void setup()
{
  Serial.begin(9600); 
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
}

void handleDetection(int distance)
{

  if (distance > distanceThreshold) {
    	threatStatus = 0;
    	updateStatus(LOW, LOW, LOW);
    	threatAssessment();
  }
  if (distance <= distanceThreshold && distance > distanceThreshold - 100) {
    	threatStatus = 1;
    	updateStatus(HIGH, LOW, LOW);
    	threatAssessment();    	
  }
  if (distance <= distanceThreshold - 100 && distance > distanceThreshold - 200) {
    	threatStatus = 2;
    	updateStatus(LOW, HIGH, LOW);
    	threatAssessment(); 
  }
  if (distance <= distanceThreshold - 200) {
    	threatStatus = 3;
    	updateStatus(LOW, LOW, HIGH);
    	threatAssessment(); 
  }  
}

void loop()
{
  // measure the ping time in cm   
  readUltrasonicDistance(SIGNAL_PIN);  
  delay(100); // Wait for 100 millisecond(s)
}