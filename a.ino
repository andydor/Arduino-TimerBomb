#include <Keypad.h>
#include <LiquidCrystal.h>

unsigned int value = 0;
int pBari = 0;
int detonation = 15;
 
// custom charaters
byte pBar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

int Seconds = 30;
int keycount = 0;

const int buttonPin1 = 12;
const int buttonPin2 = 13;
int buttonRosu = 0; // buton "fir rosu taiat" - opreste timerul bombei armate
int buttonGalben = 0; // buton "incearca si vezi ce se intampla"

String armcode1 = "115";
String armcode2 = "481";
String entered1 = "";
String entered2 = "";
bool timer = false;
bool flagB = false;
bool flag1 = false;

long previousMillis = 0;

LiquidCrystal lcd(5, 4, 3, 2, A4, A5); // the pins we use on the LCD

float resolutionADC = .0049 ; // rezolutia implicita (pentru referinta 5V) = 0.049 [V] / unitate
float resolutionSensor = .01 ; // rezolutie senzor = 0.01V/°C

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {A0, A1, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  lcd.createChar(5, pBar);
	
  lcd.begin(16, 2);
  Serial.begin(9600);

  lcd.clear();
  
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{  
	float temp = (analogRead(2) * 5.0f / 1024 - 0.5) / 0.01; // citeste temperatura de 10 ori, face media
	
	if (temp < 35.0f) {
		unsigned long currentMillis = millis();
		
		if (keycount < 3) {
			//idle
			lcd.setCursor(0, 0);
			lcd.print("Bomb disarmed!");
			lcd.setCursor(0, 1);
			lcd.print("Code 1:");
		
			lcd.setCursor(keycount + 8, 1);
			char armcodeent = keypad.getKey();
			armcodeent == NO_KEY;
			if (armcodeent != NO_KEY)
			{
				if ((armcodeent != '*') && (armcodeent != '#'))
				{
					lcd.print(armcodeent);
					entered1 += armcodeent;
					keycount++;
				}
			}
		}
		else {
			if (keycount == 3) {
				//verificare cod1
				if (armcode1 == entered1) {
					//valid
					timer = true;
					flag1 = true;
					lcd.setCursor(keycount + 5, 1);
					lcd.print("   ");
				}
				else {
					//mergem in idle
                  	lcd.setCursor(14, 0);
					lcd.print("  ");
					lcd.setCursor(keycount + 5, 1);
					lcd.print("   ");
					keycount = 0;
					entered1 = "";
					flagB = false;	
				}
			}
			if (flag1 == true) {
				//construire cod2, verif timer
				if (Seconds > 0) {
					if(currentMillis - previousMillis > 1000) {
						// salvam ultima data cand decrementam
						previousMillis = currentMillis;
						Seconds--;
					}
					if (keycount < 6) {
						lcd.setCursor(14, 0);
						lcd.print(Seconds);
						lcd.setCursor(0, 0);
						lcd.print("Bomb disarmed!");
						lcd.setCursor(0, 1);
						lcd.print("Code 2:");
			
						lcd.setCursor(keycount + 5, 1);
						char armcodeent = keypad.getKey();
						armcodeent == NO_KEY;
						if (armcodeent != NO_KEY) {
							if ((armcodeent != '*') && (armcodeent != '#')) {
								lcd.print(armcodeent);
								entered2 += armcodeent;
								keycount++;
							}
						}
					}
					else {
						//verificare timer si cod2
						if (keycount == 6) {
							if (armcode2 == entered2) {
								if (Seconds > 0) {
									//bomba armata.
									flagB = true;
									lcd.setCursor(keycount + 2, 1);
									lcd.print("   ");
								}
							}
							else {
								//mergi in idle
                              	lcd.setCursor(14, 0);
								lcd.print("  ");
								lcd.setCursor(keycount + 2, 1);
								lcd.print("   ");
								flagB = false;
								flag1 = false;
								keycount = 0;
								entered1 = "";
								entered2 = "";
								//reset timer
								timer = false;
								Seconds = 30;
							}
						}
					}
				}
				// nu mai e timp
				else {
					//mergi in idle
                  	lcd.setCursor(14, 0);
					lcd.print("  ");
					lcd.setCursor(keycount + 2, 1);
					lcd.print("   ");
					flagB = false;
					flag1 = false;
					keycount = 0;
					entered1 = "";
					entered2 = "";
					//reset timer
					timer = false;
					Seconds = 30;
				}
			}
		}
		//bomba armata
		if (flagB == true) {
			previousMillis = 0;
			lcd.clear();
			while(true) {
              	//loop cu ledbar
				buttonRosu = digitalRead(buttonPin1);
				buttonGalben = digitalRead(buttonPin2);
				
              	if (buttonRosu == HIGH) {
					lcd.clear();
                  	delay(50);
					lcd.setCursor(0, 0);
					lcd.print("You picked good");
                  	tone(A3, 5000, 30);
                  	tone(A3, 5000, 30);
					delay(100000);
				}
				if (buttonGalben == HIGH) {
					lcd.clear();
                  	delay(50);
					lcd.setCursor(3, 1);
					lcd.print("You failed");
                 	lcd.setCursor(5, 0);
                  	lcd.print("BOOM!");
                  	tone(A3, 5000, 300);
                  	delay(100000);
				}
				
				unsigned long currentMillis = millis();
				
              	if (detonation >= 10) {
                  lcd.setCursor(0, 0);
                  lcd.print(detonation);
                  lcd.setCursor(3, 0);
                  lcd.print("seconds left");
              	}
              	else if (detonation == 0) {
              		lcd.clear();
                  	lcd.setCursor(0, 0);
                  	lcd.print("BOOM!");
                  	tone(A3, 5000, 300);
                  	delay(100000);
                	}
              		else {
                      	lcd.setCursor(0, 0);
                  		lcd.print(detonation);
                      	lcd.setCursor(1, 0);
                      	lcd.print(" ");
                  		lcd.setCursor(3, 0);
                  		lcd.print("seconds left");
              		}
				
				if(currentMillis - previousMillis > 1000) {
					// salvam ultima data cand decrementam
					previousMillis = currentMillis;
                  	digitalWrite(LED_BUILTIN, HIGH);
                  	tone(A3, 5000, 50);
                  	if (value <= detonation) {
						value++;
                  	}
                  	if (detonation > 0) {
                      	detonation--;
                  	}
				}
	 
				lcd.setCursor(0,1);
              
              	pBari = map(value, 0, detonation, 0, 17);
              
                for (int i = 0; i < pBari; i++)
  				{
    				lcd.setCursor(i, 1);   
    				lcd.write(byte(5));  
  				}
              	digitalWrite(LED_BUILTIN, LOW);
			}
		}
		else {
          	lcd.setCursor(12, 1);
			lcd.print("T=");
			lcd.setCursor(14, 1);
			lcd.print(temp);
		}
	}
	else {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Temp too high!");
		lcd.setCursor(0, 1);
		lcd.print("Bomb exploded!");
      	tone(A3, 5000, 300);
		delay(100000);
	}
}
  
/* 	if (keycount<3){
		//idle
        printf cod1
        entered += key
        keycount++;
        
	}
    else{
    	if (keycount==3){
        	//verificare cod1
            if (cod1 == entered)
            {
            	//cod1 valid
                start timer
                entered = ""
                flag1=true
                
            }
            else{
            	//mergi in idle
            	keycount=0
                entered=""
                flagB=false
            }
        }
        if (flag1== true){
        	//verificare timer + cod2
            if (keycount<6){
            	printf cod2
        		entered += key
                keycount++
            }
            else{
            	if (keycount==6){
                	if (cod2 == entered)
            		{
                    	if (mai ai timp)
            				//bomba armata.
                            flagB=true
                
            		}
            		else{
            			//mergi in idle
            			keycount=0
                		entered=""
						flagB = false
						flag1 = false
                        reset timer
            		}
                    if ( NU MAI AI TIMP){
                    	flagB=false
						flag1 = false
                        keycount=0
                        entered=""
                        reset timer
                    }
                }
            }
        }
    }
    if (flagB=true)
    {
    	while(true)
    		loop cu ledbar
    }
*/