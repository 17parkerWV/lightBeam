//My display is 240x320, in portrait mode (USB port on top)
//It goes from 0-239 by 0-319
#include <Wire.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_MonoOLED.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>    //For MCUFRIEND
#include <TouchScreen.h>  //TOUCHSCREEN library
#include <stdint.h> //FOR TOUCHSCREEN library

MCUFRIEND_kbv tftDisplay;


//These are for the display part of the touchscreen
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

//These are for the touch part of the touchscreen
//I think *M means *minus and *P means *plus
#define YP A3  //This is LCD_RS on mine
#define XM A2  //This is LCD_CS on mine
#define YM 9   //This is LCD_D1 on mine
#define XP 8 //This is LCD_D0 on mine
//My resistance is 335
#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 335);


// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

const byte whiteLed = 45;
const byte redLed = 13;
const byte blueLed = 44;
const byte greenLed = 46;
const byte yellowLed = 12;
int whiteBright, redBright, blueBright, greenBright, yellowBright;
int colorArr[5] = { whiteBright, redBright, blueBright, greenBright, yellowBright };
int pinArr[5] = { whiteLed, redLed, blueLed, greenLed, yellowLed };
int intensity = 0;

const byte buzzerPin = 22;
bool buzzState = false;

//Putting my prototypes here
void mainMenu();
void resetLights();
void blackLights();
void progMenu();
void progDisplay();
void sequenceLights();
void Strobe();
void painStrobe();
void buzzer();

void setup() {
	//pinMode(XM,OUTPUT);
	//pinMode(YP,OUTPUT);
	pinMode(XP, OUTPUT);
	pinMode(YM, OUTPUT);
	pinMode(whiteLed, OUTPUT);
	pinMode(redLed, OUTPUT);
	pinMode(blueLed, OUTPUT);
	pinMode(greenLed, OUTPUT);
	pinMode(yellowLed, OUTPUT);
	pinMode(buzzerPin, OUTPUT);
	digitalWrite(buzzerPin, LOW);
	tftDisplay.begin();
	mainMenu();
}

void loop() {
	TSPoint p = ts.getPoint();
	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
		pinMode(XM, OUTPUT);
		pinMode(YP, OUTPUT);
		p.x = map(p.x, 912, 112, 0, 240);
		if (p.x < 0)
			p.x = 0;
		p.y = map(p.y, 81, 905, 0, 320);
		if (p.y < 0)
			p.y = 0;
		if (p.y >= 270 && p.y <= 319) {
			if (p.x >= 0 && p.x <= 120) {
				progDisplay();
				progMenu();
			}
			if (p.x >= 125 && p.x <= 240) {
				for (int i = 0; i <= 4; i++) {
					colorArr[i] = 0;
				}
				resetLights();
			}
		}
		intensity = map(p.x, 0, 240, 0, 255);
		if (p.x > 235)
			intensity = 255;
		if (p.x <= 9)
			intensity = 0;
		if (p.y >= 0 && p.y <= 49) {
			if (intensity > colorArr[0]) {
				colorArr[0] = intensity;
				analogWrite(whiteLed, colorArr[0]);
				tftDisplay.fillRect(2, 0, p.x, 40, WHITE);
			}
			else   if (intensity < colorArr[0]) {
				colorArr[0] = intensity;
				analogWrite(whiteLed, colorArr[0]);
				tftDisplay.fillRect(p.x + 2, 0, (238 - p.x), 40, BLACK);
			}
		}
		else if (p.y >= 53 && p.y <= 103) {
			if (intensity > colorArr[1]) {
				colorArr[1] = intensity;
				analogWrite(redLed, colorArr[1]);
				tftDisplay.fillRect(2, 53, p.x, 40, RED);
			}
			else if (intensity < colorArr[1]) {
				colorArr[1] = intensity;
				analogWrite(redLed, colorArr[1]);
				tftDisplay.fillRect(p.x + 2, 53, (238 - p.x), 40, BLACK);
			}
		}
		else if (p.y >= 107 && p.y <= 157) {
			if (intensity > colorArr[2]) {
				colorArr[2] = intensity;
				analogWrite(blueLed, colorArr[2]);
				tftDisplay.fillRect(2, 107, p.x, 40, BLUE);
			}
			else if (intensity < colorArr[2]) {
				colorArr[2] = intensity;
				analogWrite(blueLed, colorArr[2]);
				tftDisplay.fillRect(p.x + 2, 107, (238 - p.x), 40, BLACK);
			}
		}
		else if (p.y >= 161 && p.y <= 211) {
			if (intensity > colorArr[3]) {
				colorArr[3] = intensity;
				analogWrite(greenLed, colorArr[3]);
				tftDisplay.fillRect(2, 161, p.x, 40, GREEN);
			}
			else if (intensity < colorArr[3]) {
				colorArr[3] = intensity;
				analogWrite(greenLed, colorArr[3]);
				tftDisplay.fillRect(p.x + 2, 161, (238 - p.x), 40, BLACK);
			}
		}
		else if (p.y >= 215 && p.y <= 265) {
			if (intensity > colorArr[4]) {
				colorArr[4] = intensity;
				analogWrite(yellowLed, colorArr[4]);
				tftDisplay.fillRect(2, 215, p.x, 40, YELLOW);
			}
			else if (p.x < colorArr[4] && p.x >= 3) {
				colorArr[4] = intensity;
				analogWrite(yellowLed, colorArr[4]);
				tftDisplay.fillRect(p.x + 2, 215, (238 - p.x), 40, BLACK);
			}
		}
	}
}

//I'm making this separate so I can reset easier
void resetLights() {
	digitalWrite(buzzerPin, LOW);
	analogWrite(whiteLed, 0);
	analogWrite(redLed, 0);
	analogWrite(blueLed, 0);
	analogWrite(greenLed, 0);
	analogWrite(yellowLed, 0);
	tftDisplay.fillRect(9, 0, 234, 265, BLACK);
	tftDisplay.fillRect(0, 0, 9, 40, WHITE);
	tftDisplay.fillRect(0, 53, 9, 40, RED);
	tftDisplay.fillRect(0, 107, 9, 40, BLUE);
	tftDisplay.fillRect(0, 161, 9, 40, GREEN);
	tftDisplay.fillRect(0, 215, 9, 40, YELLOW);
	for (int i = 0; i <= 4; i++) {
		colorArr[i] = 0;
	}
}

void blackLights() {
	analogWrite(whiteLed, 0);
	analogWrite(redLed, 0);
	analogWrite(blueLed, 0);
	analogWrite(greenLed, 0);
	analogWrite(yellowLed, 0);
	for (int i = 0; i <= 4; i++) {
		colorArr[i] = 0;
	}
}

//This is going to have all the sliders and stuff and the menu for programs
void mainMenu() {
	tftDisplay.fillScreen(BLACK);
	tftDisplay.fillRect(0, 0, 9, 40, WHITE);
	tftDisplay.fillRect(0, 53, 9, 40, RED);
	tftDisplay.fillRect(0, 107, 9, 40, BLUE);
	tftDisplay.fillRect(0, 161, 9, 40, GREEN);
	tftDisplay.fillRect(0, 215, 9, 40, YELLOW);
	tftDisplay.fillRect(3, 0, colorArr[0], 40, WHITE);
	tftDisplay.fillRect(3, 53, colorArr[1], 40, RED);
	tftDisplay.fillRect(3, 107, colorArr[2], 40, BLUE);
	tftDisplay.fillRect(3, 161, colorArr[3], 40, GREEN);
	tftDisplay.fillRect(3, 215, colorArr[4], 40, YELLOW);
	//program button (below)
	tftDisplay.fillRoundRect(0, 270, 115, 40, 2, RED);
	//All off button (below)
	tftDisplay.fillRoundRect(125, 270, 115, 40, 2, RED);
	tftDisplay.setTextSize(1);
	tftDisplay.setCursor(95, 312);
	tftDisplay.setTextColor(WHITE);
	tftDisplay.print("Main Menu");
	tftDisplay.setTextColor(BLACK);
	tftDisplay.setCursor(25, 286);
	tftDisplay.print("Programs");
	tftDisplay.setCursor(157, 286);
	tftDisplay.print("All Off");
}

void progMenu() {
	while (1) {
		TSPoint progPoint = ts.getPoint();
		if (progPoint.z > MINPRESSURE && progPoint.z < MAXPRESSURE) {
			pinMode(XM, OUTPUT);
			pinMode(YP, OUTPUT);
			progPoint.x = map(progPoint.x, 903, 106, 0, 240);
			progPoint.y = map(progPoint.y, 81, 905, 0, 320);
			if (progPoint.x >= 0 && progPoint.x <= 120 && progPoint.y >= 275 && progPoint.y <= 313) {    //BACK button press
				mainMenu();
				return;
			}
			if (progPoint.y >= 0 && progPoint.y < 49) {   //SEQUENCE BUTTON (TOPMOST) press
				sequenceLights();
			}
			if (progPoint.y >= 53 && progPoint.y < 103) {
				Strobe();
			}
			if (progPoint.y >= 107 && progPoint.y <= 157) {
				painStrobe();
			}
			if (progPoint.x >= 125 && progPoint.x <= 240 && progPoint.y >= 275 && progPoint.y <= 320) {
				buzzState = !buzzState;
				buzzer();
				delay(100);
			}
		}
	}
}

void progDisplay() {
	tftDisplay.fillScreen(BLACK);
	tftDisplay.fillRoundRect(0, 0, 240, 50, 5, YELLOW);
	tftDisplay.fillRoundRect(0, 53, 240, 50, 5, GREEN);
	tftDisplay.fillRoundRect(0, 107, 240, 50, 5, BLUE);
	tftDisplay.fillRoundRect(0, 161, 240, 50, 5, RED);
	tftDisplay.fillRoundRect(0, 215, 240, 50, 5, WHITE);
	tftDisplay.fillRoundRect(0, 270, 115, 38, 4, RED);
	tftDisplay.fillRoundRect(125, 270, 115, 38, 4, RED);
	buzzer();
	tftDisplay.setTextSize(2);
	tftDisplay.setTextColor(BLACK);
	tftDisplay.setCursor(75, 16);   //sequence
	tftDisplay.print(F("Sequence"));  //sequence
	tftDisplay.setCursor(81, 68);  //strobe
	tftDisplay.print(F("Strobe"));  //strobe
	tftDisplay.setCursor(84, 120);
	tftDisplay.print("pain");
	tftDisplay.setCursor(25, 282);   //back
	tftDisplay.print("Back");       //back
	tftDisplay.setTextColor(WHITE);
	tftDisplay.setTextSize(1);
	tftDisplay.setCursor(93, 312);
	tftDisplay.print("Programs");
}

void sequenceLights() {
	blackLights();
	tftDisplay.fillScreen(RED);
	tftDisplay.setTextColor(BLACK);
	tftDisplay.setCursor(55, 160);
	tftDisplay.setTextSize(2);
	tftDisplay.print(F("Tap to stop"));
	while (1) {
		for (int index = 0; index <= 4; index++) {
			for (int bright = 0; bright <= 255; bright++) {
				TSPoint L = ts.getPoint();
				if (L.z > MINPRESSURE && L.z < MAXPRESSURE) {
					pinMode(XM, OUTPUT);
					pinMode(YP, OUTPUT);
					blackLights();
					progDisplay();
					return;
				}
				analogWrite(pinArr[index], bright);
			}
			analogWrite(pinArr[index], LOW);
		}
	}
}

void Strobe() {
	blackLights();
	tftDisplay.fillScreen(RED);
	tftDisplay.setTextColor(BLACK);
	tftDisplay.setCursor(55, 160);
	tftDisplay.setTextSize(2);
	tftDisplay.print(F("Tap to stop"));
	unsigned long oldMillis;
	int lightChoice = 0;
	while (1) {
		oldMillis = millis();
		analogWrite(pinArr[lightChoice], 255);
		while (millis() - oldMillis <= 270) {
			TSPoint L = ts.getPoint();
			if (L.z > MINPRESSURE && L.z < MAXPRESSURE) {
				pinMode(XM, OUTPUT);
				pinMode(YP, OUTPUT);
				blackLights();
				progDisplay();
				return;
			}
		}
		analogWrite(pinArr[lightChoice], LOW);
		oldMillis = millis();
		while (millis() - oldMillis <= 100) {
			TSPoint L = ts.getPoint();
			if (L.z > MINPRESSURE && L.z < MAXPRESSURE) {
				pinMode(XM, OUTPUT);
				pinMode(YP, OUTPUT);
				blackLights();
				progDisplay();
				return;
			}
		}
		lightChoice = (lightChoice + 1) % 5;
	}
}

void painStrobe() {
	blackLights();
	tftDisplay.fillScreen(RED);
	tftDisplay.setTextColor(BLACK);
	tftDisplay.setCursor(55, 160);
	tftDisplay.setTextSize(2);
	tftDisplay.print(F("Tap to stop"));
	digitalWrite(buzzerPin, HIGH);
	bool buzzerPinState = LOW;
	unsigned long oldMillis;
	while (1) {
		oldMillis = millis();
		analogWrite(whiteLed, 255);
		while (millis() - oldMillis <= 100) {
			TSPoint L = ts.getPoint();
			if ((millis() - oldMillis) % 24 == 0) {
				buzzerPinState = !buzzerPinState;
				digitalWrite(buzzerPin, buzzerPinState);
			}
			if (L.z > MINPRESSURE && L.z < MAXPRESSURE) {
				pinMode(XM, OUTPUT);
				pinMode(YP, OUTPUT);
				digitalWrite(buzzerPin, LOW);
				blackLights();
				progDisplay();
				return;
			}
		}
		analogWrite(whiteLed, LOW);
		oldMillis = millis();
		while (millis() - oldMillis <= 100) {
			TSPoint L = ts.getPoint();
			if ((millis() - oldMillis) % 24 == 0) {
				buzzerPinState = !buzzerPinState;
				digitalWrite(buzzerPin, buzzerPinState);
			}
			if (L.z > MINPRESSURE && L.z < MAXPRESSURE) {
				pinMode(XM, OUTPUT);
				pinMode(YP, OUTPUT);
				digitalWrite(buzzerPin, LOW);
				blackLights();
				progDisplay();
				return;
			}
		}
	}
}

void buzzer() {
	tftDisplay.setTextSize(1);
	tftDisplay.setCursor(150, 282);
	tftDisplay.setTextColor(BLACK, RED);
	if (buzzState == false) {
		digitalWrite(buzzerPin, LOW);
		tftDisplay.print("Buzz off");
	}
	if (buzzState == true) {
		digitalWrite(buzzerPin, HIGH);
		tftDisplay.print("Buzz on ");
	}
	return;
}