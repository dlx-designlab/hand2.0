/*
 * M5 S3 Atom Finger Wrestling Judge System - 2 independent devices version - will add wireless communication
 * For now, display 3 information every 100ms
 */

/*
 * 1: Using Commercial FSR
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
const int threshold = 3000; // Threshold for the FSR to detect a press
const int capacitor_threshold = 100000; // Capacitance threshold when the thumb touches the hand
const int handshake_threshold = 100000; // Capacitance threshold when players shake hands
const unsigned long winDuration = 5000; // Duration to win

//time management variables
const unsigned long readyDuration = 1000;
unsigned long lastDisplayUpdate = 0;

//To display on M5
bool touchState = 0;
bool handshakeState = 0;
int Value1 =0; //FSR value

//Dummy value for the opponent * Replace these with the values received from the opponent
bool touchState2 = 1;
bool handshakeState2 = 1;
int Value2 =0; //FSR value

int prevTouchState = -1;
int prevHandshakeState = -1;
int prevValue1 = -1;

// Player state struct
struct PlayerState {
  unsigned long winStartTime = 0;
  bool winConditionMet = false;
};

// Game states
enum GameState {WAITING, READY, PLAYING, GAME_OVER};

// Global variable set
GameState currentState = WAITING;
PlayerState player1;
PlayerState player2;
unsigned long stateStartTime = 0;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextFont(2);
  M5.Lcd.setTextSize(1);
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

  //Read the capacitive pressure value
  Value1 = analogRead(vol_1);

  touchState = touch > capacitor_threshold;
  handshakeState = handshake > handshake_threshold;

  displayValuesOnM5();

  // States management
  switch(currentState) {
    case WAITING:
      handleWaitingState(handshake);
      break;
    case PLAYING:
      handlePlayingState(Value1, Value2, touchState, touchState2, handshakeState);
      break;   
    case GAME_OVER:
      delay(2000);
      resetGame();
      break;
  }
  delay(50);
}

void handleWaitingState(long handshake) {
  if(handshakeState) {
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


void handlePlayingState(int val1, int val2, bool touch1, bool touch2, bool handshake) {
  // Pause when hands are off
  if(!handshake) {
    //Serial.println("Hands disconnected! Game paused.");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Hands disconnected! Game paused.");
    resetCounters();
    currentState = WAITING;
    return;
  }
  
  // Win condition check
  checkPlayer1Lose(val1, val2, touch1, handshake);
  checkPlayer2Lose(val1, val2, touch2, handshake);
  
  // Error check
  if(val1 > threshold && val2 > threshold) {
    //Serial.println("error - Both pressing");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Error - Both pressed.");
    currentState = GAME_OVER;
  }
}

void checkPlayer1Lose(int val1, int val2, bool touch, bool handshake) {
  bool condition = (val1 > threshold && val2 < threshold && touch);
  if(condition) {
    if(!player1.winConditionMet) {
      player1.winStartTime = millis();
      player1.winConditionMet = true;
      //Serial.println("Player2 Counting...");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("Rival Counting..");
    } 
    else if(millis() - player1.winStartTime >= winDuration) {
      //Serial.println("YOU LOSE!");
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
  resetPlayer2Counter();
}

void checkPlayer2Lose(int val1, int val2, bool touch, bool handshake) {
  bool condition = (val1 < threshold && val2 > threshold && touch);
  
  if(condition) {
    if(!player2.winConditionMet) {
      player2.winStartTime = millis();
      player2.winConditionMet = true;
      //Serial.println("You Counting...");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("You Counting...");

    } 
    else if(millis() - player2.winStartTime >= winDuration) {
      //Serial.println("YOU WIN!");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("YOU WIN!");//Player2 is the opponent, so you win when he lose.
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
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Rival count reset");
    player1.winConditionMet = false;
  }
}

void resetPlayer2Counter() {
  if(player2.winConditionMet) {
    //Serial.println("Player2 count reset");
    player2.winConditionMet = false;
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Your Count reset");
  }
}

void resetGame() {
  player1 = PlayerState();
  player2 = PlayerState();
  currentState = WAITING;
  stateStartTime = millis();
  //Serial.println("Game Reset. Please join hands to start.");
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 20); 
  M5.Lcd.println("Game Reset. Please join hands to start");
}

void displayValuesOnM5() {

  if (touchState == prevTouchState &&
      handshakeState == prevHandshakeState &&
      Value1 == prevValue1) {
    return;  // Do not refresh if the value hasn't changed
  }
  
  // Display position (3 lines on the lower half of the screen)
  int startX = 0;
  int startY = 60;
  int lineHeight = 20;

  // Partially clear the background (for overwriting)
  M5.Lcd.fillRect(startX, startY, 128, lineHeight * 3, BLACK);

  M5.Lcd.setCursor(startX, startY);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(1); // Adjust font size (if necessary)
  
  M5.Lcd.println("TouchState: " + String(touchState));
  M5.Lcd.println("Handshake:  " + String(handshakeState));
  M5.Lcd.println("Value1:     " + String(Value1));

  prevTouchState = touchState;
  prevHandshakeState = handshakeState;
  prevValue1 = Value1;
}

