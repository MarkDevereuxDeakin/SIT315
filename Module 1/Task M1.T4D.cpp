// C++ code
// References: https://docs.arduino.cc/built-in-examples/sensors/Ping

//			   https://www.tinkercad.com/projects/Ultrasonic-Distance-Sensor-Arduino-Tinkercad

//			   https://www.tinkercad.com/things/3kCYZfx9fXN-multiple-leds-/
//             editel?lessonid=ELB4FYAJD0K8TZV&projectid=O94BCSIL26F9EV3&collectionid=O0K87SQL1W5N4P2&tenant=circuits#/lesson-viewer

//			   https://www.tinkercad.com/things/8ZN0hPJWOfw-ultrasonic-distance-sensor/
//			   editel?lessonid=EAYGNT8JL6UYXE2&projectid=O3TAWASL26F9H1J&collectionid=O0K87SQL1W5N4P2#/lesson-viewer

//			   https://www.youtube.com/watch?v=Uv9UeYUsA8A

//			   https://github.com/Arduino-IRremote/Arduino-IRremote#examples-for-this-library

//			   https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/examples/SendAndReceive/SendAndReceive.ino#L167-L170



#include <IRremote.hpp>

const uint8_t TILT_PIN = 10;

const uint8_t IR_RECEIVE_PIN = 2;

const uint8_t TRIGGER_PIN = 7;
const uint8_t ECHO_PIN = 6;

const uint8_t LED_GREEN = 3;
const uint8_t LED_YELLOW = 4;
const uint8_t LED_RED = 5;
const uint8_t LED_BLUE = 8;
const uint8_t LED_ORANGE = 9;

int distanceThreshold = 300;
int distance = 0;

uint8_t ledState_GREEN = LOW;
uint8_t ledState_YELLOW = LOW;
uint8_t ledState_RED = LOW;
uint8_t ledState_BLUE = LOW;
uint8_t ledState_ORANGE = LOW;

uint8_t threatStatus = 0;

volatile bool tiltStatus = false;
volatile uint8_t systemStatus = HIGH;

void setup()
{
  Serial.begin(115200); 
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);//start the receiver
  
  pinMode(TILT_PIN, INPUT_PULLUP);
  
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);
  
  startTimer();
  attachInterrupt(digitalPinToInterrupt(IR_RECEIVE_PIN), systemToggle, RISING);
   
   //Enable Pin Change Interrupt on Port C   
   PCICR  |= B00000001; //Enable PCMSK0 (Group 0: PCINT0 to PCINT7 (D8 - D13)
   PCMSK0 |= B00000100;//D10 will trigger interrupt   
}  

void systemToggle()
{  
  if (IrReceiver.decode() && IrReceiver.decodedIRData.command == 0 && tiltStatus == 0)
  {
    systemStatus = !systemStatus;   
    
    if (systemStatus == 0)
    {
        digitalWrite(ECHO_PIN, LOW);      	
       	blinkLights();
      	digitalWrite(LED_BLUE, LOW);
    	IrReceiver.resume();
    }
    else
    { 
      Serial.println("System Restart Commenced!!");
      Serial.println("");
      Serial.println("System Now Armed!!");
      Serial.println("");
      IrReceiver.resume();
    }   
   
  } 
  
}

void tiltToggle()
{ 
    if(tiltStatus == 1)
    {
        systemStatus = 0;
        digitalWrite(ECHO_PIN, LOW);      	
        blinkLights();
        digitalWrite(LED_BLUE, LOW);
    }
    else
    {
        Serial.println(" - ");
        Serial.println("Earthquake has ceased");
        Serial.println("System Restarting");
        Serial.println(" - ");
      	systemStatus = 1;
    }
    
}

void blinkLights()
{
  for (int i = 0; i < 7; i++)
  {
    updateStatus(LOW, LOW, LOW);	
    
    switch(i)
    {      
      case 1:
      		if(tiltStatus == 0)
            {
              Serial.println("Identity Confirmed!");  
            }
      		if(tiltStatus == 1)
            {
              Serial.println(" - ");
              Serial.println("Warning!!!");
            }
      		    		
      break;
      case 3:
      		if(tiltStatus == 0)
            {
              Serial.println("Security System Powering Down");   
            }
      		if(tiltStatus == 1)
            {
              Serial.println("Earthquake in Progress");
            }
      		     		
      break;
      case 5:
      		if(systemStatus == 0 && tiltStatus == 0)
            {
              Serial.println("Access Granting");  
            }
      		if(tiltStatus == 1)
            {
              Serial.println("System Shutting Down");
      		  Serial.println(" - ");      
            }     
      		      		
      break;
      default:
      		Serial.println("");      		
      break;            
    }
    
    delay(4096);
    updateStatus(HIGH, HIGH, HIGH);
    delay(4096);
        
  }
  
  updateStatus(LOW, LOW, LOW);  
}

void startTimer()
{
  	//turn off interrupts while configuring registers
    noInterrupts();//cli(); does the same thing
	
    //define registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    TCCR1B |= (1 << WGM12);// CTC mode. Clears timer on compare

    //Clock Select Bit Description settings
    TCCR1B |= (1 << CS12);// CS12 and CS10 need to be ON for 1024 prescaler
    TCCR1B |= (1 << CS10);// CS12 and CS10 need to be ON for 1024 prescaler
    //TCCR1B |= B00000101; is the same as above
  
    //Output compare register (adjusted clock speed for timing)
    OCR1A = 15625/4; // (16000000/(1024))
    
    //Time Mask Register compared to Output compare register
    TIMSK1 = (1 << OCIE1A); //enable time1 compare interrupt

    interrupts();//sei(); does the same thing 
  
}

ISR(TIMER1_COMPA_vect)
{
  if(systemStatus == 1)
  {
    ledState_BLUE = !ledState_BLUE;
  	digitalWrite(LED_BLUE, ledState_BLUE);
    readUltrasonicDistance();
  }  
       
}

ISR(PCINT0_vect)
{
    ledState_ORANGE = !ledState_ORANGE;
    digitalWrite(LED_ORANGE, ledState_ORANGE);
    tiltStatus = !tiltStatus;     
      
    tiltToggle(); 
  
}

void readUltrasonicDistance()
{  
    pinMode(TRIGGER_PIN, OUTPUT); //clears the signal
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);  
    // Sets the trigger pin to HIGH state for 10 microseconds
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    pinMode(ECHO_PIN, INPUT);
	
  if (systemStatus == 1)
   {    	
      distance = 0.01723 * pulseIn(ECHO_PIN, HIGH);
      handleDetection(distance);
   }
    
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
  //NOTHING TO SEE HERE  
}