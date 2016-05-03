

//  Enum state of crosslights
enum VehicleTrafficLightState { V_GREEN, V_RED, V_CROSSWALK };
enum CrosswalkTrafficLightState { C_GREEN, C_RED, C_CALLED, C_DELAYEDCALL };

typedef struct VehicleTrafficLight
{
    VehicleTrafficLightState state;
};

typedef struct CrosswalkTrafficLight
{
    CrosswalkTrafficLightState state;
};

// define PIN

const short ALERT_STATE = 13;
const short LEFT_RIGHT_RED = 12;
const short LEFT_RIGHT_GREEN = 11;
const short FRONT_BOTTOM_RED = 10;
const short FRONT_BOTTOM_GREEN = 9;
const byte CROSSWALK_RED = 7;
const byte CROSSWALK_GREEN = 6;
const byte CROSSWALK_CALL = 5;
const byte CROSSWALK_BUTTON = 2;

const int period = 4000;
bool prec;
bool justCalled = false;

VehicleTrafficLight trafficLightLeftRight, trafficLightFrontBack;
CrosswalkTrafficLight crosswalk;

void initialState() {
    trafficLightLeftRight.state = V_GREEN;
    trafficLightFrontBack.state = V_RED;
    crosswalk.state = C_RED;
}

bool verifier() {
    if (trafficLightLeftRight.state==V_GREEN && trafficLightFrontBack.state==V_GREEN){
        return 0;
    }
    if (LEFT_RIGHT_GREEN==HIGH && FRONT_BOTTOM_GREEN==HIGH){
        return 0;
    }
    return 1;
}

// the setup function runs once when you press reset or power the board
void setup(){
    // initialize digital pin 13 as an output.
    pinMode(ALERT_STATE, OUTPUT);
    pinMode(LEFT_RIGHT_RED, OUTPUT);
    pinMode(LEFT_RIGHT_GREEN, OUTPUT);
    pinMode(FRONT_BOTTOM_RED, OUTPUT);
    pinMode(FRONT_BOTTOM_GREEN, OUTPUT);
    pinMode(CROSSWALK_CALL, OUTPUT);
    pinMode(CROSSWALK_GREEN,OUTPUT);
    pinMode(CROSSWALK_RED,OUTPUT);
    pinMode(CROSSWALK_BUTTON,INPUT);
    attachInterrupt(digitalPinToInterrupt(CROSSWALK_BUTTON),crosswalkCall,RISING);
    initialState();
}

void alertState(){
    digitalWrite(ALERT_STATE, HIGH);
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, LOW);
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
    digitalWrite(CROSSWALK_CALL, LOW);
    digitalWrite(CROSSWALK_GREEN, LOW);
    digitalWrite(CROSSWALK_RED, LOW);
}

// ================ TRAFFIC LIGHT LEFT RIGHT ================

void trafficLightLeftRightFromGreenToRed() {
    trafficLightLeftRight.state = V_RED;
    digitalWrite(LEFT_RIGHT_RED, HIGH);
    digitalWrite(LEFT_RIGHT_GREEN, LOW);
}

void trafficLightLeftRightFromRedToGreen() {
    trafficLightLeftRight.state = V_GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);
    
}

void trafficLightLeftRightFromRedToCrosswalk() {
  trafficLightLeftRight.state = V_CROSSWALK;
  prec = true;
}

void trafficLightLeftRightFromCrosswalkToGreen() {
  trafficLightLeftRight.state = V_GREEN;
  digitalWrite(LEFT_RIGHT_RED, LOW);
  digitalWrite(LEFT_RIGHT_GREEN, HIGH);
}

void trafficLightLeftRightFromCrosswalkToRed() {
  trafficLightLeftRight.state = V_RED;
}

// ================ TRAFFIC LIGHT FRONT BACK ================

void trafficLightFrontBackFromGreenToRed() {
    trafficLightFrontBack.state = V_RED;
    digitalWrite(FRONT_BOTTOM_RED, HIGH);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
}

void trafficLightFrontBackFromRedToGreen() {
    trafficLightFrontBack.state = V_GREEN;
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

void trafficLightFrontBackFromCrosswalkToGreen() {
  trafficLightFrontBack.state = V_GREEN;
  digitalWrite(FRONT_BOTTOM_RED, LOW);
  digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

void trafficLightFrontBackFromCrosswalkToRed() {
  trafficLightFrontBack.state = V_RED;
}

void trafficLightFrontBackFromRedToCrosswalk() {
  trafficLightFrontBack.state = V_CROSSWALK;
  prec = false;
}

// ======================== CROSSWALK ========================

void crosswalkCall(){
  if (!justCalled) {
    if (crosswalk.state == C_RED) {
      crosswalkFromRedToCalled();
      if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToCrosswalk();
      }
      if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToCrosswalk();
      }
    }
  } else {
    crosswalkFromRedToDelayedCall();
  }
}

void crosswalkFromRedToDelayedCall() {
  crosswalk.state = C_DELAYEDCALL;
  digitalWrite(CROSSWALK_CALL,HIGH);
}

void crosswalkFromRedToCalled() {
  crosswalk.state = C_CALLED;
  digitalWrite(CROSSWALK_CALL,HIGH);
}

void crosswalkFromDelayedCallToCalled() {
  crosswalk.state = C_CALLED;
  if (trafficLightFrontBack.state == V_RED) {
    trafficLightFrontBackFromRedToCrosswalk();
  }
  if (trafficLightLeftRight.state == V_RED) {
    trafficLightLeftRightFromRedToCrosswalk();
  }
}

void crosswalkFromCalledToGreen() {
  if (trafficLightFrontBack.state == V_RED) {
    trafficLightFrontBackFromRedToCrosswalk();
  }
  if (trafficLightLeftRight.state == V_RED) {
    trafficLightLeftRightFromRedToCrosswalk();
  }
  crosswalk.state = C_GREEN;
  digitalWrite(CROSSWALK_GREEN,HIGH);
  digitalWrite(CROSSWALK_RED,LOW);
  digitalWrite(CROSSWALK_CALL,LOW);
}

void crosswalkFromGreenToRed() {
  crosswalk.state = C_RED;
  digitalWrite(CROSSWALK_GREEN,LOW);
  digitalWrite(CROSSWALK_RED,HIGH);
  if ( prec == true ) {
    trafficLightFrontBackFromCrosswalkToGreen();
    trafficLightLeftRightFromCrosswalkToRed();
  } else { // prec == false
    trafficLightFrontBackFromCrosswalkToRed();
    trafficLightLeftRightFromCrosswalkToGreen();
  }
  justCalled = true;
}



// the loop function runs over and over again forever
void loop() {
    
    delay(period);
    justCalled = false;
    

    if (trafficLightLeftRight.state == V_GREEN){
        trafficLightLeftRightFromGreenToRed();
    }
    else if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToGreen();
    }

    if (trafficLightFrontBack.state == V_GREEN){
        trafficLightFrontBackFromGreenToRed();
    }
    else if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToGreen();
    }

    if (crosswalk.state == C_CALLED) {
      crosswalkFromCalledToGreen();
    } else if (crosswalk.state == C_GREEN) {
      crosswalkFromGreenToRed();
    } else if (crosswalk.state == C_DELAYEDCALL) {
      crosswalkFromDelayedCallToCalled();
    }
    
    if (!verifier()) {
        alertState();
    }
}






