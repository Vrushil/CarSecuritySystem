#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint_MEGA.h>

int getFingerprintIDez();

#define mySerial Serial1



/*if using Serial2 Rx3 is IN from sensor (GREEN wire), Tx3 is OUT from arduino  (WHITE wire) */
//#define mySerial Serial3
Adafruit_Fingerprint_MEGA finger = Adafruit_Fingerprint_MEGA(&mySerial);
/* MQ-7 Carbon Monoxide Sensor Circuit with Arduino */

const int AOUTpin = 0; //the AOUT pin of the Air quality sensor goes into analog pin A0 of the arduino air quality SENSR VALUE INPUT
const int COpin = 1; //the AOUT pin of the CO sensor goes into analog pin A1 of the arduino CO SENSOR VALUE INPUT
//const int DOUTpin=52;//the DOUT pin of the CO sensor goes into digital pin D8 of the arduino CO SENSOR DIGITAL READING INPUT
const int led_a = 8; //output of air quality sensor which displays light in car if it requires service
const int motor = 22; // POwer window output motor
const int led_f = 9;
const int c_lock = 23;


//GSm module
SoftwareSerial mySSerial(2, 3);//RX, TX(tx from  gsm, rx from gsm) ///mySSerial for air qulaity i.e.  GSM
int x = 1; // flag for air quality
int i;
int temp;
int limit;
int COvalue;
int window = 0; // flag for co sensor
int value;
///**/




void setup()
{
  finger.begin(57600);
  if (finger.verifyPassword())
  {
    Serial.println("Found fingerprint sensor!");
  }
  else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial.println("Waiting for valid finger...");
  pinMode(led_a, OUTPUT);
  pinMode(motor, OUTPUT); //window opening pin
  pinMode(c_lock, OUTPUT);//central lock opening pin
  pinMode(led_f, OUTPUT);
  Serial.begin(9600);
  mySSerial.begin(9600);   // Setting the baud rate of GSM Module
  Serial.println("fingertest");
  //  pinMode(DOUTpin, INPUT);//sets the pin as an input to the arduino
  pinMode(led_a, OUTPUT);
  // set the data rate for the sensor serial port
}



void loop()                     // run over and over again
{
  getFingerprintIDez();

  ////**/
  Serial.print("Air quality flag:                          ");// for air quality
  Serial.println(x);
  Serial.print("Co flag:                                           ");
  Serial.println(window);
  //Serial.println(" "+window);

  value = analogRead(AOUTpin); //reads the analaog value from  air qulaity sensor AOUT pin0
  COvalue = analogRead(COpin); //reads the analaog value from the CO sensor's AOUT pin1
  //limit= digitalRead(DOUTpin);//reads the digital value from the CO sensor's DOUT pin

  //for(i=0;i<10;i++)
  //{value += analogRead(AOUTpin);
  //   COvalue+= analogRead(COpin);
  //  value=value/2;
  //  COvalue=COvalue/2;
  //  }

  Serial.print("Air quality value: ");
  Serial.println(value);//prints the CO value
  //Serial.println();

  //Serial.print("CO value:    ");
  Serial.println(COvalue);//prints the CO value
  //Serial.println();
  //Serial.print("Limit: ");
  //Serial.print(limit);//prints the limit reached as either LOW or HIGH (above or underneath)
  delay(500);

  if (COvalue > 450)
  {
    window = window + 1;
  }
  else
  {
    window = 0;
    //digitalWrite(motorpin,LOW);
  }

  switch (window)
  {
    case 1:
      openwindows();
      break;
    case 0:
      closewindows();
      break;
    default:
      break;
  }

  if (value > 700)
  {
    x = x + 1;
    //digitalWrite(ledPin,HIGH);
  }
  else
  {
    x = 0;
    temp = 2;
    digitalWrite(led_a, LOW); //if threshold not reached, LED remains off

  }

  switch (x) {
    case 2:
      SendMessage();
      //temp=3
      break;
    case 0:
      displayy();
      break;
    default:
      break;
  }


}




//Fingerprint functions


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");


  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  {
    Serial.println("Did not match Ahha!!");
    digitalWrite(led_f, HIGH);
    delay(2000);
    digitalWrite(led_f, LOW);

  }

  // found a match!
  // turn the LED off by making the voltage LOW

  else {
    digitalWrite(c_lock, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(2000);              // wait for a second
    digitalWrite(c_lock, LOW);
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.print(finger.confidence); Serial.print(" Doors will open-Authentic User");
    Serial.println("");
    return finger.fingerID;
  }
}



// Function for sending a message to user
void SendMessage()
{
  mySSerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySSerial.println("AT+CMGS=\"+91xxxxxxxxxx\"\r"); // Replace x with mobile number
  delay(1000);
  mySSerial.println("Your car requires maintenance");// The SMS text you want to send
  delay(100);
  mySSerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  digitalWrite(led_a, HIGH);

}



void displayy()
{
  Serial.println("Normal level of  Air quality sensor");
}



void openwindows()
{
  Serial.println("Windows open");
  digitalWrite(motor, HIGH);
  delay(2000);
  digitalWrite(motor, LOW);
}


void closewindows()
{ //digitalWrite(motorpin,LOW);
  Serial.println("Windows closed");
}
