/*
TODO:
 - Average out the temperature over time for both sensors so we do not flop the 
 furnace on and off 
 - Calculate heating temp to go beyond current temp (and below) so as to not 
 be turning the furnace on over and over for small changes in temprature 
 (maybe +/- two degrees?)
 
 */


#include <uOLED.h>
#include <colors.h>
#include <SPI.h>

int thermo1Pin = 0; 
int thermo2Pin = 1; 
long previousMillis = 0;       
long interval = 1000;   

int heatPin = 13;
char heatStatus;

const int buttonPinUp = 6;
const int buttonPinDown = 5;
const int buttonPinMode = 10;
int buttonStateUp = 0;
int buttonStateDown = 0;
int buttonStateMode = 0;

int currentStateUp = 0;
int previousStateUp = 0;
int currentStateDown = 0;
int previousStateDown = 0;
int currentStateMode = 0;
int previousStateMode = 0;



const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int settemp = 20;

uOLED uoled; 


int state = LOW;                       // The current state of the output pin
int reading;                           // The current reading from the input pin
int previous = HIGH;   
boolean allow = false;


void setup()
{
  Serial.begin(115200);

  uoled.begin(8,256000, &Serial);     
  uoled.SetContrast(5);

  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
  pinMode(buttonPinMode, INPUT_PULLUP);

  pinMode(heatPin, OUTPUT);

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;  

}

void loop()                     
{
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   

    total = total - readings[index];         
    // read from the sensor:  

    int reading1 = analogRead(thermo1Pin);  
    int reading2 = analogRead(thermo2Pin); 
    int readingAvg = (reading1 + reading2) / 2;

    readings[index] = readingAvg; 
    // add the reading to the total:
    total= total + readings[index];       
    // advance to the next position in the array:  
    index = index + 1;                    

    // if we're at the end of the array...
    if (index >= numReadings)              
      // ...wrap around to the beginning: 
      index = 0;                           

    // calculate the average:
    int readingFinal = total / numReadings; 

    float voltage = readingFinal * 5.0;
    voltage /= 1024.0; 

    float temperatureC = (voltage - 0.5) * 100 ;  
    char temp[10];
    itoa(temperatureC, temp, 10);

    uoled.Text(0,0,SMALL_FONT,WHITESMOKE,"Current Temp:",1);
    uoled.Text(14,0,SMALL_FONT,MIDNIGHTBLUE,temp,1);

    if(temperatureC <= settemp) { 
      heatStatus = HIGH; 
    } 
    else {
      heatStatus = LOW;
    }
    digitalWrite(heatPin, heatStatus);
  }  



  if(state == 1) {


    buttonStateUp = digitalRead(buttonPinUp);
    if (buttonStateUp == HIGH) {
      currentStateUp = 1;
    }
    else {
      currentStateUp = 0;
    }

    if(currentStateUp != previousStateUp) {
      if(currentStateUp == 1) {
        settemp = settemp + 1;
      }
    }

    previousStateUp = currentStateUp;

    buttonStateDown = digitalRead(buttonPinDown);
    if (buttonStateDown == HIGH) {
      currentStateDown = 1;
    }
    else {
      currentStateDown = 0;
    }

    if(currentStateDown != previousStateDown) {
      if(currentStateDown == 1) {
        settemp = settemp - 1;
      }
    }

    previousStateDown = currentStateDown;


  }






  buttonStateMode = digitalRead(buttonPinMode);


  if ( buttonStateMode == LOW && previous == HIGH) {
    //start_hold = millis();
    allow = true;
  }

  if (allow == true && buttonStateMode == LOW && previous == LOW) {
    //if ((millis() - start_hold) >= OnDelay) {
    state = !state;
    allow = false;

    //}
  }

  previous = buttonStateMode;



  char dotemp[10];
  itoa(settemp, dotemp, 10);

  uoled.Text(0,1,SMALL_FONT,WHITESMOKE,"Set Temp:",1);
  uoled.Text(14,1,SMALL_FONT,MIDNIGHTBLUE,dotemp,1);

  uoled.Text(0,2,SMALL_FONT,WHITESMOKE,"Mode:",1);
  if(state == 0) {
    uoled.Text(6,2,SMALL_FONT,MIDNIGHTBLUE,"Auto  ",1);
  } 
  else {
    uoled.Text(6,2,SMALL_FONT,MIDNIGHTBLUE,"Manual",1);
  }



}














