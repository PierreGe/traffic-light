

// define PIN

const short ALERT_STATE = 13;
const short LEFT_RIGHT_RED = 12;
const short LEFT_RIGHT_GREEN = 11;
const short FRONT_BOTTOM_RED = 10;
const short FRONT_BOTTOM_GREEN = 9;


const int delay = 1000;

//  -- enum state of crosslights
enum VehicleTrafficLightState { GREEN, RED };

typedef struct VehicleTrafficLight
{
    VehicleTrafficLightState state; 
} 

VehicleTrafficLight trafficLightLeftRight, trafficLightFrontBack;

void initialState() {
    trafficLightLeftRight.state = GREEN;
    trafficLightFrontBack.state = RED;
}

bool verifier() {
  if (trafficLightLeftRight.state==GREEN && trafficLightFrontBack.state==GREEN){
    return 0;
  }
  if (LEFT_RIGHT_GREEN==HIGH && FRONT_BOTTOM_GREEN==HIGH){
    return 0;
  }
  return 1;
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(ALERT_STATE, OUTPUT);
  pinMode(LEFT_RIGHT_RED, OUTPUT);
  pinMode(LEFT_RIGHT_GREEN, OUTPUT);
  pinMode(FRONT_BOTTOM_RED, OUTPUT);
  pinMode(FRONT_BOTTOM_GREEN, OUTPUT);
  initialState(); 
}

void alertState() {
  digitalWrite(ALERT_STATE, HIGH);
  digitalWrite(LEFT_RIGHT_RED, LOW);
  digitalWrite(LEFT_RIGHT_GREEN, LOW);
  digitalWrite(FRONT_BOTTOM_RED, LOW);
  digitalWrite(FRONT_BOTTOM_GREEN, LOW);
}

void trafficLightLeftRightFromGreenToRed() {
  trafficLightLeftRight.state = RED;
  digitalWrite(LEFT_RIGHT_RED, HIGH);
  digitalWrite(LEFT_RIGHT_GREEN, LOW);
}

void trafficLightLeftRightFromRedToGreen() {
  trafficLightLeftRight.state = GREEN;
  digitalWrite(LEFT_RIGHT_RED, LOW);
  digitalWrite(LEFT_RIGHT_GREEN, HIGH);
}

void trafficLightFrontBackFromGreenToRed() {
  trafficLightFrontBack.state = RED;
  digitalWrite(FRONT_BOTTOM_RED, HIGH);
  digitalWrite(FRONT_BOTTOM_GREEN, LOW);
}

void trafficLightFrontBackFromRedToGreen() {
  trafficLightFrontBack.state = GREEN;
  digitalWrite(FRONT_BOTTOM_RED, LOW);
  digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

// the loop function runs over and over again forever
void loop() {
  delay(delay); 

  if (trafficLightLeftRight.state == GREEN){
    trafficLightLeftRightFromGreenToRed();
  } else {
    trafficLightLeftRightFromRedToGreen()
  }

  if (trafficLightFrontBack.state == GREEN){
    trafficLightFrontBackFromGreenToRed();
  } else {
    trafficLightFrontBackFromRedToGreen()
  }
  
  if (!verifier()) {
    alertState();
  }
}
