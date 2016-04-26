

// define PIN

const short ALERT_STATE = 13;
const short LEFT_RIGHT_RED = 12;
const short LEFT_RIGHT_GREEN = 11;
const short FRONT_BOTTOM_RED = 10;
const short FRONT_BOTTOM_GREEN = 9;


//  -- enum state of crosslights
enum VTLState { GREEN, RED };

typedef struct VehicleTrafficLight
{
    VTLState state; 
};

VehicleTrafficLight TLLR, TLFB;

void initialState() {
    TLLR.state = GREEN;
    TLFB.state = RED;
}

bool verifier() {
  if (TLLR.state==GREEN && TLFB.state==GREEN){
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

// the loop function runs over and over again forever
void loop() {
  delay(1000); 

  if (TLLR.state == GREEN){
    TLLR.state = RED;
    TLFB.state = GREEN;
    digitalWrite(LEFT_RIGHT_RED, HIGH);
    digitalWrite(LEFT_RIGHT_GREEN, LOW);
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
  }
  else{
    TLFB.state = RED;
    TLLR.state = GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(FRONT_BOTTOM_RED, HIGH);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);
  }
  
  if (!verifier()) {
    digitalWrite(ALERT_STATE, HIGH);
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, LOW);
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
  }
}
