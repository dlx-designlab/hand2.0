/*
 * M5 S3 Atom Finger Wrestling Judge System - 2 independent devices version
 * 負けている方だけが判定する
 */
/*
 * 1: Not Capasitive Commercial Pressure Sensor
 * 2: M5 Touch Sensor for game start trigger
 * 3: Result display on M5
 */
#include <Wire.h>
#include <M5AtomS3.h>

//Pin set
#define TOUCH_PIN1 G8
#define HANDSHAKE_PIN G2
const int vol_1 = G5;  // FSR sensor

//Thresholds config
const int threshold = 3000;//FSR, if pressed
const int capasiter_threshold = 1000; //if thumb touches hand when pressed
const int handshake_threshold = 100000; //if players touch their hands each other
const unsigned long winDuration = 5000;
const unsigned long readyDuration = 1000;

// Player state struct
struct PlayerState {
  unsigned long winStartTime = 0;
  bool winConditionMet = false;
};

// Game state
enum GameState {WAITING, READY, PLAYING, GAME_OVER};

// Global variable set
GameState currentState = WAITING;
PlayerState player1;
//PlayerState player2;
unsigned long stateStartTime = 0;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextFont(4);

  Serial.begin(9600);

  pinMode(vol_1, INPUT);

  //Serial.println("System Ready - Please join hands");
  M5.Lcd.setCursor(10, 20); 
  M5.Lcd.println("System Ready, Please join hands");

}

void loop() {
  // Read the touch values
  long touch = touchRead(TOUCH_PIN1);
  long handshake = touchRead(HANDSHAKE_PIN);

  //Read the capasitive pressure value
  int Value1 = analogRead(vol_1);

  // State management
  switch(currentState) {
    case WAITING:
      handleWaitingState(handshake);
      break;
    case PLAYING:
      handlePlayingState(Value1, touch, handshake);
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
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("Game Start!");
      currentState = PLAYING;
      stateStartTime = millis();
    }
  } else {
    stateStartTime = millis();
  }
}

void handlePlayingState(int val1, long touch1, long handshake) {
  // Pause whem hands are off
  if(handshake < handshake_threshold) {
    //Serial.println("Hands disconnected! Game paused.");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Hands disconnected! Game paused.");
    resetCounters();
    currentState = WAITING;
    return;
  }
  // Judge
  checkPlayer1Win(val1, touch1, handshake);
}

void checkPlayer1Win(int val1, long touch, long handshake) {
  bool condition = (val1 > threshold && touch > capasiter_threshold);
  if(condition) {
    if(!player1.winConditionMet) {
      player1.winStartTime = millis();
      player1.winConditionMet = true;
      //Serial.println("Counting...");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("Counting..");
    } 
    else if(millis() - player1.winStartTime >= winDuration) {
      //Serial.println("YOU LOSE");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("YOU LOSE!");
      currentState = GAME_OVER;
    }
  } else {
    resetPlayer1Counter();
  }
}

void resetCounters() {
  resetPlayer1Counter();
}

void resetPlayer1Counter() {
  if(player1.winConditionMet) {
    //Serial.println("Player1 count reset");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Count reset");
    player1.winConditionMet = false;
  }
}

void resetGame() {
  player1 = PlayerState();
  //player2 = PlayerState();
  currentState = WAITING;
  stateStartTime = millis();
  //Serial.println("Game Reset. Please join hands to start.");
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 20); 
  M5.Lcd.println("Game Reset. Please join hands to start");
}
