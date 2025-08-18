/*
 * Arduino Thumbwrestling Jugde system
 */

/*
 * Using Arduino, Commercial FSR sensor, I2C LCD
 */

#include <CapacitiveSensor.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

CapacitiveSensor cs_4_2 = CapacitiveSensor(4,2);  // Player 1 touch sensor
CapacitiveSensor cs_8_10 = CapacitiveSensor(10,8); // Player 2 touch sensor
CapacitiveSensor cs_7_5 = CapacitiveSensor(7,5);   // Detect hands joined

const int vol_1 = A0;  //FSR
const int vol_2 = A1;
const int threshold = 300;
const int capasiter_threshold = 1000;
const int handshake_threshold = 350;
const unsigned long winDuration = 4000;
const unsigned long readyDuration = 1000;

// Player state structs
struct PlayerState {
  unsigned long winStartTime = 0;
  bool winConditionMet = false;
};

// Game states
enum GameState {WAITING, READY, PLAYING, GAME_OVER};

// Global variables
GameState currentState = WAITING;
PlayerState player1;
PlayerState player2;
unsigned long stateStartTime = 0;

void setup() {
  // Initialising capacitors
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_8_10.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_7_5.set_CS_AutocaL_Millis(0xFFFFFFFF);

  lcd.init();
  lcd.backlight();
  
  Serial.begin(9600);
  pinMode(vol_1, INPUT);
  pinMode(vol_2, INPUT);
  //Serial.println("System Ready - Please join hands");
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  lcd.setCursor(0, 1);
  lcd.print("Please join hands");
}

void loop() {
  // Read the sensors
  long touch1 = cs_4_2.capacitiveSensor(30);
  long touch2 = cs_8_10.capacitiveSensor(30);
  long handshake = cs_7_5.capacitiveSensor(30);
  int Value1 = analogRead(vol_1);
  int Value2 = analogRead(vol_2);
  Serial.println(handshake);
  
  switch(currentState) {
    case WAITING:
      handleWaitingState(handshake);
      break;
      
    case PLAYING:
      handlePlayingState(Value1, Value2, touch1, touch2, handshake);
      break;
      
    case GAME_OVER:
      delay(2000);
      resetGame();
      break;
  }
  
  delay(50);
}

void handleWaitingState(long handshake) {
  if(handshake > handshake_threshold) {
    if(millis() - stateStartTime > readyDuration) {
      //Serial.println("Game Start!");
      lcd.clear();
      lcd.print("Game Start!");
      currentState = PLAYING;
      stateStartTime = millis();
    }
  } else {
    stateStartTime = millis();
  }
}

void handlePlayingState(int val1, int val2, long touch1, long touch2, long handshake) {

  // Pause when hands are off
  if(handshake < handshake_threshold) {
    //Serial.println("Hands disconnected! Game paused.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hands disconnected!");
    lcd.setCursor(0, 1);
    lcd.print("Game paused.");
    resetCounters();
    currentState = WAITING;
    return;
  }

  // check winning
  checkPlayer1Win(val1, val2, touch1, handshake);
  checkPlayer2Win(val1, val2, touch2, handshake);
  
  // error check
  if(val1 > threshold && val2 > threshold) {
    //Serial.println("error - Both pressing");
    lcd.clear();
    lcd.print("error - Both pressing");
    currentState = GAME_OVER;
  }
  
}

void checkPlayer1Win(int val1, int val2, long touch, long handshake) {
  bool condition = (val1 > threshold && val2 < threshold && touch > capasiter_threshold);
  
  if(condition) {
    if(!player1.winConditionMet) {
      player1.winStartTime = millis();
      player1.winConditionMet = true;
      //Serial.println("Player1 Counting...");
      lcd.clear();
      lcd.print("Player1 Counting...");
    } 
    else if(millis() - player1.winStartTime >= winDuration) {
      //Serial.println("Player 1 WIN!");
      lcd.clear();
      lcd.print("Player 1 WIN!");
      currentState = GAME_OVER;
    }
  } else {
    //player1.winConditionMet = false;
    resetPlayer1Counter();
  }

}

void resetCounters() {
  resetPlayer1Counter();
  resetPlayer2Counter();
}

void checkPlayer2Win(int val1, int val2, long touch, long handshake) {
  bool condition = (val1 < threshold && val2 > threshold && touch > capasiter_threshold);
  
  if(condition) {
    if(!player2.winConditionMet) {
      player2.winStartTime = millis();
      player2.winConditionMet = true;
      //Serial.println("Player2 Counting...");
      lcd.clear();
      lcd.print("Player2 Counting...");

    } 
    else if(millis() - player2.winStartTime >= winDuration) {
      //Serial.println("Player 2 WIN!");
      lcd.clear();
      lcd.print("Player 2 WIN!");
      currentState = GAME_OVER;
    }
  } else {
    //player2.winConditionMet = false;
    resetPlayer2Counter();
  }

}
void resetPlayer1Counter() {
  if(player1.winConditionMet) {
    //Serial.println("Player1 count reset");
    player1.winConditionMet = false;
    lcd.clear();
    lcd.print("P1 Count reset");
  }
}

void resetPlayer2Counter() {
  if(player2.winConditionMet) {
    //Serial.println("Player2 count reset");
    player2.winConditionMet = false;
    lcd.clear();
    lcd.print("P2 Count reset");
  }
}

void resetGame() {
  player1 = PlayerState();
  player2 = PlayerState();
  currentState = WAITING;
  stateStartTime = millis();
  //Serial.println("Game Reset. Please join hands to start.");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Reset.");
  lcd.setCursor(0, 1);
  lcd.print("Please join hands to start.");
}