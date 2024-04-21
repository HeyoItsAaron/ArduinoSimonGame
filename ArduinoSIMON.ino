/*
  ARDUINO CLONE OF 'SIMON SAYS' BY HASBRO
  Written by Aaron Williams
  3/19/2024
*/
#include <stdio.h>
#include "ListLib.h"

enum GameState {
  PREGAME,
  INGAME,
  JUSTWON,
  JUSTLOST
};

enum Difficulty {
  EASY,
  MEDIUM,
  HARD,
  EXPERT
};

// const int buttonPinRed = A0;
// const int buttonPinGreen = A1;
// const int buttonPinBlue = A2;
// const int buttonPinYellow = A3;

const int buzzerPin = 9; 

const int ledPinRed = 3;
const int ledPinGreen = 4;
const int ledPinBlue = 5;
const int ledPinYellow = 6;

const int winFlash = 100; 
const int loseFlash = 250; 

List<int> sequence;
List<int> userSequence;

GameState gameState;

int winSequenceLength;
int startSequenceLength;
int speed = 10000;

void setDifficulty(Difficulty difficulty){
  if(difficulty == EASY){
    startSequenceLength = 3;
    winSequenceLength = 6;
    speed = 1000; 
  } else if(difficulty == MEDIUM){
    startSequenceLength = 4;
    winSequenceLength = 10;
    speed = 1000; 
  } else if(difficulty == HARD){
    startSequenceLength = 5;
    winSequenceLength = 20;
    speed = 1000;
  } else if(difficulty == EXPERT){
    startSequenceLength = 6;
    winSequenceLength = 31;
    speed = 1000;
  }
}

void turnOffAllPins(){
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen,  LOW);
  digitalWrite(ledPinBlue, LOW);
  digitalWrite(ledPinYellow, LOW);
}

void turnOnAllPins(){
  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinGreen,  HIGH);
  digitalWrite(ledPinBlue, HIGH);
  digitalWrite(ledPinYellow, HIGH);
}

void RESET(){
  turnOffAllPins();
  sequence.Clear();
  userSequence.Clear();
  speed = 1000;
  gameState = PREGAME;
}

void playSequence(){
  turnOffAllPins();

  for (int i = 0; i < sequence.Count(); i++)
  {
    digitalWrite(sequence[i],  HIGH);
    delay(speed);
    digitalWrite(sequence[i], LOW);
    delay(200);
  }
}

void addToSequence(){
  int nextSignal = (rand() % 4) + 3;
  sequence.Add(nextSignal);
}

bool anyButtonIsPressed(){
  if(digitalRead(A0) == HIGH || digitalRead(A1) == HIGH || digitalRead(A2) == HIGH || digitalRead(A3) == HIGH){
    return true;
  }
  return false;
}

void chooseDifficulty(){
  while(true){
    if(digitalRead(A0) == HIGH){
        setDifficulty(EASY);
        break;
      }
      if(digitalRead(A1) == HIGH){
        setDifficulty(MEDIUM);
        break;
      }
      if(digitalRead(A2) == HIGH){
        setDifficulty(HARD);
        break;
      }
      if(digitalRead(A3) == HIGH){
        setDifficulty(EXPERT);
        break;
      }
  }
}

void readInputSequence(){
  int counter = 0;
  while(counter < sequence.Count()){
    bool hasFailed = false;

    // RED
    if(digitalRead(A0) == HIGH){
      digitalWrite(ledPinRed, HIGH);
      userSequence.Add(ledPinRed);
      counter += 1;
      delay(200);
      if(userSequence[counter] != sequence[counter]){
        gameState = JUSTLOST;
        counter = 32;
      }
      digitalWrite(ledPinRed, LOW);
    }
    
    // GREEN
    if(digitalRead(A1) == HIGH){
      digitalWrite(ledPinGreen, HIGH);
      userSequence.Add(ledPinGreen);
      counter += 1;
      delay(200);
      if(userSequence[counter] != sequence[counter]){
        gameState = JUSTLOST;
        counter = 32;
      }
      digitalWrite(ledPinGreen, LOW);
    }

    // BLUE
    if(digitalRead(A2) == HIGH){
      digitalWrite(ledPinBlue, HIGH);
      userSequence.Add(ledPinBlue);
      counter += 1;
      delay(200);
      if(userSequence[counter] != sequence[counter]){
        gameState = JUSTLOST;
        counter = 32;
      }
      digitalWrite(ledPinBlue, LOW);
    }

    // YELLOW
    if(digitalRead(A3) == HIGH){
      digitalWrite(ledPinYellow, HIGH);
      userSequence.Add(ledPinYellow);
      counter += 1;
      if(userSequence[counter] != sequence[counter]){
        gameState = JUSTLOST;
        counter = 32;
      }
      digitalWrite(ledPinYellow, LOW);
    }
  }
}

void pregame(){
  setDifficulty(EASY);
  gameState = INGAME;
}

void ingame(){
  // IN GAME LOGIC HERE
  if(sequence.Count() < winSequenceLength){
    addToSequence();

    for (int i = 0; i<3; i++){
      turnOnAllPins();
      delay(100);
      turnOffAllPins();
      delay(100);
    }

    delay(250);
    playSequence();
    readInputSequence();
    speed -= 50;
  }
  else{
    gameState = JUSTWON;
  }
}

void justwon(){
  // Logic for light and speaker of win celebration
  // play lights one after the other light a looping line rainbow thing
  // play brighter buh buh buhhhhh or something
  for (int j = 0; j < 4;  j++){
    for(int i = 0; i < 4; i++){
      // use i to iterate cleanly because the led pins should be 0, 1, 2, 3
      digitalWrite(i, HIGH);
      tone(buzzerPin, 1000);

      delay(winFlash);

      digitalWrite(i, LOW);
      noTone(buzzerPin);

      delay(winFlash);
    }
  }
  RESET();
}

void justlost(){
  // Logic for light and speaker sequence for a loss
  // flash all lights light 5 times
  // play harsher behhhhhhhhhh noise
  for (int i = 0; i < 5;  i++){
    turnOnAllPins();
    // turn on buzzer at 1KHz
    tone(buzzerPin, 1000);
    delay(500);
    turnOffAllPins();
    // turn off buzzer
    noTone(buzzerPin);
    delay(500);
  }
  RESET();
}

void setup() {
  // initialize the LED pins as an output:
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);

  // initialize the pushbutton pins as an input:
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  // initialize the buzzer
  pinMode(buzzerPin, OUTPUT);

  gameState = PREGAME;
}

void loop() {
  if(gameState == PREGAME){
    pregame();
  } else if(gameState == INGAME){
    ingame();
  } else if(gameState == JUSTWON){
    justwon();
  } else if(gameState == JUSTLOST){
    justlost();
  }
}