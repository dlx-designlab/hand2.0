/*
 * M5 S3 Atom Finger Wrestling Judge System - 2 independent devices version
 * 100ms に一回、3つの情報を出し続ける
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

//time management
const unsigned long readyDuration = 1000;
unsigned long lastDisplayUpdate = 0;

//To display on M5
bool touchState = 0;
bool handshakeState = 0;
int Value1 =0; //FSR value

//Dummy value as an oppnent's state
bool touchState2 = 1;
bool handshakeState2 = 1;
int Value2 =4000; //FSR value

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
  M5.Lcd.setTextFont(2);

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
  Value1 = analogRead(vol_1);

  touchState = touch > capasiter_threshold;
  handshakeState = handshake > handshake_threshold;

  if (millis() - lastDisplayUpdate >= 100) {
    displayValuesOnM5();
    lastDisplayUpdate = millis();
  }

  // State management
  switch(currentState) {
    case WAITING:
      handleWaitingState(handshake);
      break;
    case PLAYING:
      //handlePlayingState(Value1, Value2, touch1, touch2, handshake);
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
  // 勝利判定
  checkPlayer1Win(val1, touch1, handshake);
  //checkPlayer2Win(val1, val2, touch2, handshake);
  // エラーチェック
  /*
  if(val1 > threshold && val2 > threshold) {
    //Serial.println("error - Both pressing");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Error - Both pressed.");
    currentState = GAME_OVER;
  }
  */
}

void checkPlayer1Win(int val1, long touch, long handshake) {
  bool condition = (val1 > threshold && touch > capasiter_threshold);
  if(condition) {
    if(!player1.winConditionMet) {
      player1.winStartTime = millis();
      player1.winConditionMet = true;
      //Serial.println("Player1 Counting...");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("Player1 Counting..");
    } 
    else if(millis() - player1.winStartTime >= winDuration) {
      //Serial.println("Player 1 WIN!");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(10, 20); 
      M5.Lcd.println("Player1 WIN!");
      currentState = GAME_OVER;
    }
  } else {
    resetPlayer1Counter();
  }
}

void resetCounters() {
  resetPlayer1Counter();
  //resetPlayer2Counter();
}

void resetPlayer1Counter() {
  if(player1.winConditionMet) {
    //Serial.println("Player1 count reset");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 20); 
    M5.Lcd.println("Player1 count reset");
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

void displayValuesOnM5() {
  // 表示位置（画面下半分に3行分）
  int startX = 0;
  int startY = 100;
  int lineHeight = 20;

  // 背景を部分的にクリア（上書き用）
  M5.Lcd.fillRect(startX, startY, 128, lineHeight * 3, BLACK);

  M5.Lcd.setCursor(startX, startY);
  M5.Lcd.setTextColor(WHITE, BLACK);
  //M5.Lcd.setTextSize(1); // フォントサイズ調整（必要に応じて）
  
  M5.Lcd.println("TouchState: " + String(touchState));
  M5.Lcd.println("Handshake:  " + String(handshakeState));
  M5.Lcd.println("Value1:     " + String(Value1));
}

