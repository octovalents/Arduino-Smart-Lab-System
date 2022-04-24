// C++ code
//
#include <Wire.h>

const int kPinBulb = 2;
const int kPinButton = 3;
const int kPinSensor = 4;

/* ----- MAIN PROGRAM ----- */
bool countOne = false;
bool doorOpen = false;
bool isPerSecond = false;
int buttonState = 0;
int sensorState = 0;
int studentsInside = 0;
int enterCountdown = 10;
String messageStatus = "";
unsigned long startMillis = 0;
unsigned long currentMillis;
unsigned long period = 1000;
unsigned long threshold;

void setup()
{
    pinMode(kPinBulb, OUTPUT);
    pinMode(kPinSensor, INPUT);
    pinMode(kPinButton, INPUT);
    digitalWrite(kPinButton, HIGH);

    Serial.begin(9600);

    Wire.begin(8);

    // call a function when receive data from master
    Wire.onReceive(receiveFunc);
}

void loop()
{
    /* --- SETUP PER SECOND BOOLEAN --- */
    currentMillis = millis();
    threshold = startMillis + period;

    if (currentMillis >= threshold)
    {
        startMillis = currentMillis;
        isPerSecond = true;
    }
    else
    {
        isPerSecond = false;
    }

    /* --- READ INPUT STATE FROM PIN --- */
    buttonState = digitalRead(kPinButton);
    sensorState = digitalRead(kPinSensor);

    Serial.println(messageStatus);

    /* --- UPDATE DOOR STATUS --- */
    if (messageStatus == "enteropen" ||
        messageStatus == "exitopen")
    {
        countOne = true;
        doorOpen = true;
    }
    else if (messageStatus == "closed")
    {
        doorOpen = false;
    }

    /* --- UPDATE STUDENTS COUNT --- */
    if (sensorState == HIGH &&
        countOne &&
        doorOpen)
    {
        if (messageStatus == "enteropen")
        {
            studentsInside += 1;
        }
        else if (messageStatus == "exitopen" &&
                 studentsInside > 0)
        {
            studentsInside -= 1;
        }

        enterCountdown = 10;
        messageStatus = "";
        countOne = false;
    }

    /* --- UPDATE LIGHT BULB STATE --- */
    if (studentsInside > 0)
    {
        digitalWrite(kPinBulb, HIGH);
    }
    else
    {
        digitalWrite(kPinBulb, LOW);
    }

    /* --- UPDATE OPEN DOOR COUNTDOWN --- */
    if (isPerSecond &&
        doorOpen)
    {
        enterCountdown -= 1;
    }

    if (enterCountdown <= 0 &&
        doorOpen)
    {
        enterCountdown = 10;
    }

    delay(5);
}

void receiveFunc(int byteNum)
{
    // get message from master
    while (Wire.available())
    {
        char c = Wire.read();
        messageStatus += c; // receive byte
    }
}
