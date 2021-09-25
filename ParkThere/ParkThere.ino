/*
 * ParkThere
 * View more details at iangohy.com/projects/ParkThere
 * Submission for What the Hack 2021 by Team HZAUP
 * 
 * 
 * References
 * Seven segment display code reference: https://www.jameco.com/Jameco/workshop/TechTip/working-with-seven-segment-displays.html
 */

// For 7 segment displays (x2)
int segPins[] = {15, 2, 4, 5, 18, 19, 21, 22 };   // { a b c d e f g . )
int displayPins[] = {12, 14};   // pin 10 controls D0, pin 11 controls D1
int displayBuf[2];              // The display buffer contains the digits to be displayed.
                                // displayBuf[0] contains the LSD, displayBuf[1] contains the MSD
int ledPin = 27;
int irPin = 13;
bool DEBUG = 0;

byte segCode[20][8] = {
// 7 segment code table
//  a  b  c  d  e  f  g  .
  { 1, 1, 1, 1, 1, 1, 0, 0},  // 0
  { 0, 1, 1, 0, 0, 0, 0, 0},  // 1
  { 1, 1, 0, 1, 1, 0, 1, 0},  // 2
  { 1, 1, 1, 1, 0, 0, 1, 0},  // 3
  { 0, 1, 1, 0, 0, 1, 1, 0},  // 4
  { 1, 0, 1, 1, 0, 1, 1, 0},  // 5
  { 1, 0, 1, 1, 1, 1, 1, 0},  // 6
  { 1, 1, 1, 0, 0, 0, 0, 0},  // 7
  { 1, 1, 1, 1, 1, 1, 1, 0},  // 8
  { 1, 1, 1, 1, 0, 1, 1, 0},  // 9
  { 1, 1, 1, 1, 1, 1, 0, 1},  // 0.
  { 0, 1, 1, 0, 0, 0, 0, 1},  // 1.
  { 1, 1, 0, 1, 1, 0, 1, 1},  // 2.
  { 1, 1, 1, 1, 0, 0, 1, 1},  // 3.
  { 0, 1, 1, 0, 0, 1, 1, 1},  // 4.
  { 1, 0, 1, 1, 0, 1, 1, 1},  // 5.
  { 1, 0, 1, 1, 1, 1, 1, 1},  // 6.
  { 1, 1, 1, 0, 0, 0, 0, 1},  // 7.
  { 1, 1, 1, 1, 1, 1, 1, 1},  // 8.
  { 1, 1, 1, 1, 0, 1, 1, 1},  // 9.
};


void refreshDisplay(int digit1, int digit0)
{
  if (digit1 != 0 && digit0 != 0)
  {
    digitalWrite(displayPins[0], HIGH);  // displays digit 0 (least significant)
    digitalWrite(displayPins[1], LOW );
    setSegments(digit0);
    delay(5);
    digitalWrite(displayPins[0], LOW);    // then displays digit 1
    digitalWrite(displayPins[1], HIGH);
    setSegments(digit1);
    delay(5);
    digitalWrite(displayPins[0], HIGH);    // then displays digit 1
    digitalWrite(displayPins[1], HIGH); 
  }
}

void setSegments(int n)
{
  for (int i=0; i < 8; i++)
  {
    digitalWrite(segPins[i], segCode[n][i]);
  }
}

void setPrice(float n)
{
  int intpart = (int) n;
  int decpart = ((int) (n * 10)) % 10;

  displayBuf[1] = intpart + 10;
  displayBuf[0] = decpart;

  if (DEBUG)
  {
    Serial.print("n: ");
    Serial.println(n);
    Serial.print(displayBuf[0]);
    Serial.print(" ");
    Serial.println(displayBuf[1]); 
  }
}
//******************************************************************************
//******************************************************************************
void setup()
{
  Serial.begin(115200);
  
  for (int i=0; i < 8; i++)
  {
    pinMode(segPins[i], OUTPUT);
  }
  pinMode(displayPins[0], OUTPUT);
  pinMode(displayPins[1], OUTPUT);

  pinMode(ledPin, OUTPUT);

  displayBuf[1] = 11;    // initializes the display
  displayBuf[0] = 2;

  Serial.println("=== ParkThere system initialised ===");
}

float i = 0.0;
int startTime = 0;
int endTime;
bool ledState = 0;
bool ledActivate = 0;
int lastIn = 0;

//******************************************************************************
//******************************************************************************
void loop()
{
   refreshDisplay(displayBuf[1],displayBuf[0]);  // Refreshes the display with the contents of displayBuf
                                                     // each iteration of the main loop.
   digitalWrite(ledPin, ledState); 
   
   if (Serial.available() > 0) {
    // read the incoming byte:
    String incoming = Serial.readString();

    float incomingFloat = incoming.toFloat();

    if (incoming.equals("on\n")) {
      Serial.println("Turning on warning lights");
      ledActivate = 1;
      ledState = 1;
    }
    else if (incoming.equals("off\n")) {
      Serial.println("Turning off warning lights");
      ledActivate = 0;
      ledState = 0;
    }
    else {
      Serial.println("Setting parking price");
      setPrice(incomingFloat);
    }

    if (DEBUG)
    {
      Serial.print("ESP32 received: ");
      Serial.print(incoming);
    }
   }
   
   endTime = millis();                  // increments the counter approximately once a second
   if ((endTime - startTime) >= 100)
   {
      i = i + 0.1;
//      setPrice(i);
      startTime = endTime;
      if (ledActivate == 1) {
        ledState = !ledState;
      }
   }
   
   if ((endTime - lastIn) >= 3000 && ledActivate == 0)
   {
      if (DEBUG)
      {
        Serial.print(endTime);
        Serial.print(" ");
        Serial.println(lastIn);
      }
      Serial.println("Turning on warning lights");
      ledActivate = 1;
      ledState = 1;
   }

   int irMeasure = digitalRead(irPin);
   if (irMeasure && ledActivate == 1)
   {
    Serial.println("Turning off warning lights");
    ledActivate = 0;
    ledState = 0;
    lastIn = endTime;
   }
   else if (irMeasure)
   {
      lastIn = endTime;
   }
}
