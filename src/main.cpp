#include <Arduino.h>
#include <OneWire.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NUMPIXELS 13
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

const int ledPin = 6, button0 = 12, button1 = 11, button2 =10;


// DS18S20 Temperature chip i/o
OneWire ds0(2), ds1(3), ds2(4), ds3(5);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, ledPin, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool activeGame = 0, error = 0;
byte gameMode = 0, attempts = 0;


// List of known addresses (example values)
byte knownAddresses[6][8] = {
    {0x28, 0xFD, 0xD4, 0x82, 0xF, 0x0, 0x0, 0xE6}, 
    {0x28, 0x4E, 0x19, 0x82, 0xF, 0x0, 0x0, 0x76},
    {0x28, 0xB, 0xAA, 0x7A, 0xB, 0x0, 0x0, 0x1A},
    {0x28, 0x5A, 0xD9, 0x79, 0xB, 0x0, 0x0, 0xA8},
    {0x28, 0x18, 0x72, 0x81, 0xF, 0x0, 0x0, 0x3}, 
    {0x28, 0x87, 0xBA, 0x82, 0xF, 0x0, 0x0, 0x2}  
};

// Corresponding names for known addresses
const char* addressNames[6] = {
    "Yellow",
    "Blue",
    "Black",
    "White",
    "Red",
    "Pink"
};


const uint32_t figureColors[6] = {
    strip.Color(60,60,0),
    strip.Color(0,0,60),
    strip.Color(0,0,0),
    strip.Color(60,60,60),
    strip.Color(60,0,0),
    strip.Color(60,5,34)

};

byte gameKey[4]={0,0,0,0}, currentKey[4]={0,0,0,0}, result[3]={0,0,0};



void setup(void) {
    Serial.begin(9600); // Start serial communication
    randomSeed(analogRead(0));

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  pinMode(button0, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Welcome toCogmind"));
  display.display();

  strip.begin();
  strip.fill(strip.Color(0,60,0), 0, NUMPIXELS);
  strip.show();
  delay(1000);
  
}

int checkOneWireDevice(OneWire &ds, const char* dsName, int position) {
    byte i;
    byte addr[8];


    ds.reset_search();
    if (!ds.search(addr)) {
        Serial.print("No more addresses for ");
        Serial.print(dsName);
        Serial.println(".");
        strip.fill(strip.Color(60,0,5), position*3+2, 2);
        currentKey[position]=9;
        ds.reset_search();
        return 0;
    }

    // Check if the found address is in the known list
    bool known = false;
    for (i = 0; i < 6; i++) {
        bool match = true;
        for (byte j = 0; j < 8; j++) {
            if (knownAddresses[i][j] != addr[j]) {
                match = false;
                /*
                Serial.print("Known: ");
                Serial.print(knownAddresses[i][j], HEX);
                Serial.print(" Read: ");
                Serial.println(addr[j], HEX);
                */
                break;
            }
        }
        if (match) {
            Serial.print(dsName);
            Serial.print(" found: ");
            Serial.println(addressNames[i]);
            strip.fill(figureColors[i], position*3+2, 2);
            strip.show();
            currentKey[position]=i;
            known = true;
            break;
        }
        else{
            strip.fill(strip.Color(60,0,5), position*3+2, 2);
            strip.show();
            currentKey[position]=9;
        }
    }

    if (!known) {
        Serial.print(dsName);
        Serial.print(" R=");
        for (i = 0; i < 8; i++) {
            Serial.print(addr[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    return 1;
}

void generateGameKey() {
  int usedNumbers[6] = {0, 0, 0, 0, 0, 0};  // Array to track used numbers
  
  for (int i = 0; i < 4; i++) {
    int newNumber;
    do {
      newNumber = random(0, 6);  // Generate a random number from 0 to 5
    } while (usedNumbers[newNumber] == 1);  // Repeat if the number is already used

    gameKey[i] = newNumber;
    usedNumbers[newNumber] = 1;  // Mark this number as used
  }
}

void loop(void) {
    if(activeGame!=1){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Dificulty:"));
        display.println(F("G=EASY"));
        display.println(F("Y=MEDIUM"));
        display.println(F("R=HARD"));
        display.display();
    
        while(activeGame!=1){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Difficulty"));
            //poll the buttons
            if(digitalRead(button0)==0){//easy
                while(digitalRead(button0)==0){//wait for button release
            }
                gameMode=0;
                activeGame=1;
                strip.fill(strip.Color(0,60,0), 0, NUMPIXELS);
                display.println(F("Easy"));

            }
            if(digitalRead(button1)==0){//medium
                while(digitalRead(button1)==0){//wait for button release
            }
                gameMode=1;
                activeGame=1;
                strip.fill(strip.Color(60,60,0), 0, NUMPIXELS);
                display.println(F("Medium"));
            }
            if(digitalRead(button2)==0){//hard
                while(digitalRead(button2)==0){//wait for button release
            }
                gameMode=2;
                activeGame=1;
                strip.fill(strip.Color(60,0,0), 0, NUMPIXELS);
                display.println(F("Hard"));
            }
        }
        Serial.println(gameMode);
        strip.show();
        display.println(F("Generating"));
        display.display();

        generateGameKey();
        delay(2000);
        strip.fill(strip.Color(60,0,5), 2, NUMPIXELS-2);
        strip.show();
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("Attempts:"));
        display.println(attempts);
        display.println(F("Place"));
        display.println(F("figures"));
        display.display();
        
        
    }
    checkOneWireDevice(ds0, "ds0", 0);
    checkOneWireDevice(ds1, "ds1", 1);
    checkOneWireDevice(ds2, "ds2", 2);
    checkOneWireDevice(ds3, "ds3", 3);

    if((currentKey[0]!=9)&&(currentKey[1]!=9)&&(currentKey[2]!=9)&&(currentKey[3]!=9)){
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("Attempts:"));
        display.println(attempts);
        display.println(F("Press G"));
        display.println(F("to check"));

    }
    else{
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("Attempts:"));
        display.println(attempts);
        display.println(F("Place"));
        display.println(F("figures"));

    }
    strip.show();
    display.display();
    delay(200); // Delay between reads


    if(digitalRead(button0)==0){//poll check button
        attempts+=1;
        display.clearDisplay();
        display.setCursor(0,0);
        display.print(F("Attempts:"));
        display.println(attempts);
        display.println(F("Checking..."));
        display.display();
        result[0]=0;//greens
        result[1]=0;//yellows
        result[2]=0;//reds
        error = 0;

        
        for(int i=0; i<4; i++){
            if(currentKey[i]==gameKey[i]){
                if(gameMode==0) strip.fill(strip.Color(0,60,0),i*3+2 , 2);
                result[0]+=1;
            }
            else{
                error = 1;
                if(gameMode==0) strip.fill(strip.Color(60,0,0),i*3+2 , 2);
                result[2]+=1;
                for (int j=0; j<4; j++){
                    if(currentKey[i]==gameKey[j]){
                        if(gameMode==0) strip.fill(strip.Color(30,30,0),i*3+2 , 2);
                        if ((gameMode==0)||(gameMode==1)){
                            result[1]+=1;
                            result[2]-=1;
                        }
                        
                        break;
                    }
                    
                }
            }
        }
        /*if(gameMode!=0){
            Serial.println(result[0])
            strip.fill(strip.Color(30,30,0),3, result[0]); //Greens
            strip.fill(strip.Color(30,30,0),3+result[0] , result[1]); //Yellows
            strip.fill(strip.Color(30,30,0),3+result[0]+result[1] , result[2]); //Reds
        }*/

        strip.show();
        if(error==0){
            display.clearDisplay();
            display.setCursor(0,0);
            display.println(F("Congrats!"));
            display.print(F("Attempts:"));
            display.println(attempts);
            display.println(F("Press R to continue"));
            display.display();
            strip.fill(strip.Color(0,60,0), 0, NUMPIXELS);
            strip.show();

            while(digitalRead(button2)==1){//wait for button press
            }
            while(digitalRead(button2)==0){//wait for button release
            }

            activeGame=0;
            attempts=0;
            display.clearDisplay();
            display.setCursor(0,0);
            display.println(F("Quitting"));
            display.display();
            delay(1000); 
            strip.fill(strip.Color(0,60,0), 0, NUMPIXELS);
            strip.show();

            
        }
        else{
            display.clearDisplay();
            display.setCursor(0,0);
            display.println(F("Press Y"));
            display.print(F("G:"));
            display.println(result[0]);
            display.print(F("Y:"));
            display.println(result[1]);
            display.print(F("R:"));
            display.println(result[2]);
            display.display();
            while(digitalRead(button1)==1){//wait for button press
            }
            while(digitalRead(button1)==0){}//wait for button release
            delay(2000);
            display.clearDisplay();
            display.setCursor(0,0);
            display.print(F("Attempts:"));
            display.println(attempts);
            display.println(F("Place"));
            display.println(F("figures"));
            display.display();
            
        }
    }
    else if(digitalRead(button2)==0){//wait for button press
        while(digitalRead(button2)==1){//wait for button release
        }

        activeGame=0;
        attempts=0;
        delay(1000);
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(F("Quitting"));
        display.display();
        delay(1000);
        strip.fill(strip.Color(0,60,0), 0, NUMPIXELS);
        strip.show();
        
    }


}
