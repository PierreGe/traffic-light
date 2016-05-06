// Arduino code for traffic lights
// Maxime, Nicolas, Julian, Walter, Pierre


// ====== config ======

const int GLOBAL_PERIOD = 4000;


// ====== Arduino PIN ======
// alert
const byte ALERT_STATE = 13;
// green-red light
const byte LEFT_RIGHT_RED = 12;
const byte LEFT_RIGHT_GREEN = 11;
const byte FRONT_BOTTOM_RED = 10;
const byte FRONT_BOTTOM_GREEN = 9;
const byte CROSSWALK_RED = 7;
const byte CROSSWALK_GREEN = 6;
const byte BUSPATH_RED = 5;
const byte BUSPATH_GREEN = 4;

// button
const byte CROSSWALK_CALL = 14;
const byte CROSSWALK_BUTTON = 15;
const byte BUSPATH_CALL = 16;
const byte BUSPATH_BUTTON = 17;


// ====== Enum state of crosslights ======
enum VehicleTrafficLightStates { V_GREEN, V_RED, V_CROSSWALK,V_BUSPATH };
enum CrosswalkTrafficLightStates { C_GREEN, C_RED, C_CALLED, C_PREEMPTED, C_DELAYEDCALL, C_BUSPATH };
enum BusTrafficLightStates { B_GREEN, B_RED, B_CALLED, B_PREEMPTED, B_DELAYEDCALL, B_CROSSWALK };
typedef struct VehicleTrafficLight
{
    VehicleTrafficLightStates state;
};
typedef struct CrosswalkTrafficLight
{
    CrosswalkTrafficLightStates state;
};
typedef struct BusTrafficLight
{
    BusTrafficLightStates state;
};


// ====== Working var ======

volatile bool isrBuspath = false;
volatile bool isrCrosswalk = false;
int delayer = 0;
bool prec = false;
bool justCalled = false;
bool busc, pedc = false;
bool preemptible = true;
VehicleTrafficLight trafficLightLeftRight, trafficLightFrontBack;
CrosswalkTrafficLight crosswalk;
BusTrafficLight buspath;


// ====== Function ======

inline void debug(const char* toPrint){
    Serial.write(toPrint);
    Serial.write("\n");
    delay(50);
}


void initialState() {
    // first state of every crosslight
    trafficLightLeftRight.state = V_GREEN;
    trafficLightFrontBack.state = V_RED;
    crosswalk.state = C_RED;
    buspath.state = B_RED;

    // according pin

    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);
    digitalWrite(FRONT_BOTTOM_RED, HIGH);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
    digitalWrite(CROSSWALK_RED, HIGH);
    digitalWrite(CROSSWALK_GREEN, LOW);
    digitalWrite(BUSPATH_RED, HIGH);
    digitalWrite(BUSPATH_GREEN, LOW);

    // we check it is coherent
    if (!verifier()) {
        alertState();
    }
}


bool verifier() {
    // crosswalk variable value
    if (crosswalk.state == C_GREEN){
        if (trafficLightLeftRight.state==V_GREEN){
            debug("verifier() : FATAL ERROR : state : crosswalk and trafficLightLeftRight");
            return 0;
        }
        if (trafficLightFrontBack.state==V_GREEN){
            debug("verifier() : FATAL ERROR : state : crosswalk and trafficLightFrontBack");
            return 0;
        }
        if (buspath.state == B_GREEN){
            debug("verifier() : FATAL ERROR : state : crosswalk and buspath");
            return 0;
        }
    }
    // crosswalk pin value
    if (CROSSWALK_GREEN==HIGH){
        if (LEFT_RIGHT_GREEN==HIGH){
            debug("verifier() : FATAL ERROR : pin : crosswalk and left right");
            return 0;
        }
        if (FRONT_BOTTOM_GREEN==HIGH){
            debug("verifier() : FATAL ERROR : pin : crosswalk and front bottow");
            return 0;
        }
        if (BUSPATH_GREEN==HIGH){
            debug("verifier() : FATAL ERROR : pin : crosswalk and buspath");
            return 0;
        }
    }

    // bus variable value
    if (buspath.state == B_GREEN){
        if (trafficLightLeftRight.state==V_GREEN){
            debug("verifier() : FATAL ERROR : state : buspath and left right");
            return 0;
        }
        if (trafficLightFrontBack.state==V_GREEN){
            debug("verifier() : FATAL ERROR : state : buspath and front back");
            return 0;
        }
        if (crosswalk.state == B_GREEN){
            debug("verifier() : FATAL ERROR : state : buspath and crosswalk");
            return 0;
        }
    }

    // bus pin value
    if (BUSPATH_GREEN==HIGH){
        if (LEFT_RIGHT_GREEN==HIGH){
            debug("verifier() : FATAL ERROR : pin : buspath and left right");
            return 0;
        }
        if (FRONT_BOTTOM_GREEN==HIGH){
            debug("verifier() : FATAL ERROR : pin : buspath and front back");
            return 0;
        }
        if (CROSSWALK_GREEN==HIGH){
            debug("verifier() : FATAL ERROR : pin : buspath and crosswalk");
            return 0;
        }
    }

    //  basic variable crosslight
    if (trafficLightLeftRight.state==V_GREEN && trafficLightFrontBack.state==V_GREEN){
        debug("verifier() : FATAL ERROR : state : leftright and frontback");
        return 0;
    }
    //  basic pin crosslight
    if (LEFT_RIGHT_GREEN==HIGH && FRONT_BOTTOM_GREEN==HIGH){
        debug("verifier() : FATAL ERROR : pin : leftrigh and frontback");
        return 0;
    }
    debug("verifier() : OKAY");
    return 1;
}

// the setup function runs once when you press reset or power the board
void setup(){
    // serial for debug
    Serial.begin(250000);
    debug("========= RESET =========");
    // initialize OUTPUT pin
    pinMode(ALERT_STATE, OUTPUT);
    pinMode(LEFT_RIGHT_RED, OUTPUT);
    pinMode(LEFT_RIGHT_GREEN, OUTPUT);
    pinMode(FRONT_BOTTOM_RED, OUTPUT);
    pinMode(FRONT_BOTTOM_GREEN, OUTPUT);
    pinMode(CROSSWALK_GREEN,OUTPUT);
    pinMode(CROSSWALK_RED,OUTPUT);
    pinMode(BUSPATH_RED, OUTPUT);
    pinMode(BUSPATH_GREEN, OUTPUT);
    pinMode(CROSSWALK_CALL, OUTPUT);
    pinMode(BUSPATH_CALL, OUTPUT);

    // initialize INPUT pin
    pinMode(CROSSWALK_BUTTON,INPUT);
    pinMode(BUSPATH_BUTTON,INPUT);
    attachInterrupt(digitalPinToInterrupt(CROSSWALK_BUTTON),crosswalkCallISR,RISING);
    attachInterrupt(digitalPinToInterrupt(BUSPATH_BUTTON),buspathCallISR,RISING);
    initialState();
    debug("setup() : initialisation done");
}


void alertState(){
    debug("alertState() : entering failure state");
    digitalWrite(ALERT_STATE, HIGH);
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, LOW);
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
    digitalWrite(CROSSWALK_CALL, LOW);
    digitalWrite(CROSSWALK_GREEN, LOW);
    digitalWrite(CROSSWALK_RED, LOW);
    digitalWrite(BUSPATH_RED, LOW);
    digitalWrite(BUSPATH_GREEN, LOW);
    digitalWrite(BUSPATH_CALL, LOW);
    // we don't go out of alert state
    while (true) {
        digitalWrite(ALERT_STATE, HIGH);
        delay(100);
        digitalWrite(ALERT_STATE, LOW);
        delay(75);
    }
}

// ================ SIGNALS ================

void signalBusCall() {
    if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToBuspath();
    } else if (trafficLightLeftRight.state == V_CROSSWALK) {
        if (preemptible) {
            trafficLightLeftRightFromCrosswalkToBusPath();
        }
    }

    if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToBuspath();
    } else if (trafficLightFrontBack.state == V_CROSSWALK) {
        if (preemptible) {
            trafficLightFrontBackFromCrosswalkToBusPath();
        }
    }

    if (crosswalk.state == C_CALLED) {
        crosswalkFromCalledToPreempted();
    } else if (crosswalk.state == C_DELAYEDCALL) {
        crosswalkFromDelayedCallToPreempted();
    }

    
}

void signalPedestrianCall() {
    if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToCrosswalk();
    }
    if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToCrosswalk();
    }
}

void signalFree() {
    if (trafficLightLeftRight.state == V_CROSSWALK) {
        if (busc) {
            trafficLightLeftRightFromCrosswalkToBusPath();
        } else if (prec) { // busc==false
            trafficLightLeftRightFromCrosswalkToRed();
        } else { // busc==false && prec==false
            trafficLightLeftRightFromCrosswalkToGreen();
        }
    } else if (trafficLightLeftRight.state == V_BUSPATH) {
        if (pedc) {
            trafficLightLeftRightFromBusPathToCrosswalk();
        } else if (prec) { // pedc==false
            trafficLightLeftRightFromBusPathToRed();
        } else { // pedc==false && prec==false
            trafficLightLeftRightFromBusPathToGreen();
        }
    }

    if (trafficLightFrontBack.state == V_CROSSWALK) {
        if (busc) {
            trafficLightFrontBackFromCrosswalkToBusPath();
        } else if (prec) { // busc==false
            trafficLightFrontBackFromCrosswalkToGreen();
        } else { // busc==false && prec==false
            trafficLightFrontBackFromCrosswalkToRed();
        }
    } else if (trafficLightFrontBack.state == V_BUSPATH) {
        if (pedc) {
            trafficLightFrontBackFromBusPathToCrosswalk();
        } else if (prec) { // pedc==false
            trafficLightFrontBackFromBusPathToGreen();
        } else { // pedc==false && prec==false
            trafficLightFrontBackFromBusPathToRed();
        }
    }

    if (crosswalk.state == C_PREEMPTED) {
        crosswalkFromPreemptedToGreen();
    } else if (crosswalk.state == C_BUSPATH) {
        crosswalkFromBusPathToRed();
    }

    if (buspath.state == B_PREEMPTED) {
        buspathFromPreemptedToGreen();
    } else if (buspath.state == B_CROSSWALK) {
        buspathFromCrosswalkToRed();
    }
}


// ================ TRAFFIC LIGHT LEFT RIGHT ================

void trafficLightLeftRightFromGreenToRed() {
    debug("trafficLightLeftRight: Green --> Red");
    trafficLightLeftRight.state = V_RED;
    digitalWrite(LEFT_RIGHT_RED, HIGH);
    digitalWrite(LEFT_RIGHT_GREEN, LOW);
}

void trafficLightLeftRightFromCrosswalkToBusPath() {
    debug("trafficLightLeftRight: Crosswalk --> Buspath");
    trafficLightLeftRight.state = V_BUSPATH;
}

void trafficLightLeftRightFromRedToGreen() {
    debug("trafficLightLeftRight: Red --> Green");
    trafficLightLeftRight.state = V_GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);

}

// crosswalk
void trafficLightLeftRightFromRedToCrosswalk() {
    debug("trafficLightLeftRight: Red --> Crosswalk");
    trafficLightLeftRight.state = V_CROSSWALK;
    prec = true;
}

void trafficLightLeftRightFromCrosswalkToGreen() {
    debug("trafficLightLeftRight: Crosswalk --> Green");
    trafficLightLeftRight.state = V_GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);
}

void trafficLightLeftRightFromCrosswalkToRed() {
    debug("trafficLightLeftRight: Crosswalk --> Red");
    trafficLightLeftRight.state = V_RED;
}

// bus
void trafficLightLeftRightFromRedToBuspath() {
    debug("trafficLightLeftRight: Red --> Buspath");
    trafficLightLeftRight.state = V_BUSPATH;
    prec = true;
}

void trafficLightLeftRightFromBusPathToGreen() {
    debug("trafficLightLeftRight: Buspath --> Green");
    trafficLightLeftRight.state = V_GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);
}

void trafficLightLeftRightFromBusPathToRed() {
    debug("trafficLightLeftRight: Buspath --> Red");
    trafficLightLeftRight.state = V_RED;
}

void trafficLightLeftRightFromBusPathToCrosswalk() {
    debug("trafficLightLeftRight: Buspath --> Crosswalk");
    trafficLightLeftRight.state = V_CROSSWALK; 
}

// ================ TRAFFIC LIGHT FRONT BACK ================

void trafficLightFrontBackFromGreenToRed() {
    debug("trafficLightFrontBack: Green --> Red");
    trafficLightFrontBack.state = V_RED;
    digitalWrite(FRONT_BOTTOM_RED, HIGH);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
}

void trafficLightFrontBackFromCrosswalkToBusPath() {
    debug("trafficLightFrontBack: Crosswalk --> Buspath");
    trafficLightFrontBack.state = V_BUSPATH;
}

void trafficLightFrontBackFromRedToGreen() {
    debug("trafficLightFrontBack: Red --> Green");
    trafficLightFrontBack.state = V_GREEN;
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

// crosswalk
void trafficLightFrontBackFromCrosswalkToGreen() {
    debug("trafficLightFrontBack: Crosswalk --> Green");
    trafficLightFrontBack.state = V_GREEN;
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

void trafficLightFrontBackFromCrosswalkToRed() {
    debug("trafficLightFrontBack: Crosswalk --> Red");
    trafficLightFrontBack.state = V_RED;
}

void trafficLightFrontBackFromRedToCrosswalk() {
    debug("trafficLightFrontBack: Red --> Crosswalk");
    trafficLightFrontBack.state = V_CROSSWALK;
    prec = false;
}

// bus
void trafficLightFrontBackFromBusPathToGreen() {
    debug("trafficLightFrontBack: Buspath --> Green");
    trafficLightFrontBack.state = V_GREEN;
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

void trafficLightFrontBackFromBusPathToRed() {
    debug("trafficLightFrontBack: Buspath --> Red");
    trafficLightFrontBack.state = V_RED;
}

void trafficLightFrontBackFromRedToBuspath() {
    debug("trafficLightFrontBack: Red --> Buspath");
    trafficLightFrontBack.state = V_BUSPATH;
    prec = false;
}

void trafficLightFrontBackFromBusPathToCrosswalk() {
    debug("trafficLightFrontBack: Buspath --> Crosswalk");
    trafficLightFrontBack.state = V_CROSSWALK;
}


// ======================== CROSSWALK ========================

void crosswalkCallISR(){
    isrCrosswalk = true;
}

// Interupt routine
void crosswalkCall(){
    debug("crosswalkCall() : ISR : button pressed");
    if (crosswalk.state == C_RED) {
        if (busc) {
            crosswalkFromRedToPreempted();
        } else {
            if (justCalled) {
                crosswalkFromRedToDelayedCall();
            } else {
                crosswalkFromRedToCalled();
            }
        }
    }
}

void crosswalkFromRedToPreempted() {
    debug("crosswalk: Red --> Preempted");
    crosswalk.state = C_PREEMPTED;
    pedc=true;
    preemptible=false;
    signalPedestrianCall();
    digitalWrite(CROSSWALK_CALL,HIGH);
}

void crosswalkFromRedToDelayedCall() {
    debug("crosswalk: Red --> DelayedCall");
    crosswalk.state = C_DELAYEDCALL;
    pedc=true;
    digitalWrite(CROSSWALK_CALL,HIGH);
}

void crosswalkFromRedToCalled() {
    debug("crosswalk: Red --> Called");
    crosswalk.state = C_CALLED;
    pedc=true;
    signalPedestrianCall();
    digitalWrite(CROSSWALK_CALL,HIGH);
}

void crosswalkFromDelayedCallToCalled() {
    debug("crosswalk: DelayedCall --> Called");
    crosswalk.state = C_CALLED;
    signalPedestrianCall();
}

void crosswalkFromCalledToGreen() {
    debug("crosswalk: Called --> Green");

    signalPedestrianCall();
    preemptible = false;
    
    crosswalk.state = C_GREEN;
    digitalWrite(CROSSWALK_GREEN,HIGH);
    digitalWrite(CROSSWALK_RED,LOW);
    digitalWrite(CROSSWALK_CALL,LOW);
}

void crosswalkFromGreenToBuspath() {
  debug("crosswalk: Green --> Buspath");
  crosswalk.state = C_BUSPATH;
  pedc=false;
  signalFree();
  digitalWrite(CROSSWALK_GREEN,LOW);
  digitalWrite(CROSSWALK_RED,HIGH);
}

void crosswalkFromGreenToRed() {
    debug("crosswalk: Green --> Red");
    crosswalk.state = C_RED;
    preemptible = true;
    pedc=false;
    justCalled = true;
    signalFree();
    digitalWrite(CROSSWALK_GREEN,LOW);
    digitalWrite(CROSSWALK_RED,HIGH);
}

void crosswalkFromCalledToPreempted() {
    debug("crosswalk: Called --> Preempted");
    crosswalk.state = C_PREEMPTED;
}

void crosswalkFromDelayedCallToPreempted() {
    debug("crosswalk: DelayedCall --> Preempted");
    crosswalk.state = C_PREEMPTED;
}

void crosswalkFromPreemptedToGreen() {
    debug("crosswalk: Preempted --> Green");
    crosswalk.state = C_GREEN;
    preemptible = false;
    digitalWrite(CROSSWALK_GREEN,HIGH);
    digitalWrite(CROSSWALK_RED,LOW);
    digitalWrite(CROSSWALK_CALL,LOW);
}

void crosswalkFromBusPathToRed() {
    debug("crosswalk: Buspath --> Red");
    crosswalk.state = C_RED;
    preemptible = true;
    justCalled = true;
    digitalWrite(CROSSWALK_GREEN,LOW);
    digitalWrite(CROSSWALK_RED,HIGH);
}

// ======================== BUSPATH ========================

void buspathCallISR(){
    isrBuspath = true;
}

//interupt routine
void buspathCall(){
    debug("buspathCall() : ISR : button pressed");
    if (buspath.state == B_RED) {
        if (!preemptible) {
            buspathFromRedToPreempted(); 
        } else if (!justCalled) { // preemptible==true
            buspathFromRedToCalled();
        } else {
            buspathFromRedToDelayedCall();
        }
    }
    
    if (!verifier()) {
        alertState();
    }
}

void buspathFromRedToPreempted() {
    debug("buspath: Red --> Preempted");
    buspath.state = B_PREEMPTED;
    busc = true;
    signalBusCall();
    digitalWrite(BUSPATH_CALL,HIGH);
}

void buspathFromRedToDelayedCall() {
    debug("buspath: Red --> DelayedCall");
    buspath.state = B_DELAYEDCALL;
    busc = true;
    digitalWrite(BUSPATH_CALL,HIGH);
}

void buspathFromRedToCalled() {
    debug("buspath: Red --> Called()");
    buspath.state = B_CALLED;
    busc = true;
    signalBusCall();
    digitalWrite(BUSPATH_CALL,HIGH);
}

void buspathFromDelayedCallToCalled() {
    debug("buspath: DelayedCall --> Called");
    buspath.state = B_CALLED;
    signalBusCall();
}

void buspathFromDelayedCallToPreempted() {
    debug("buspath: DelayedCall --> Preempted");
    buspath.state = B_PREEMPTED;
}

void buspathFromCalledToGreen() {
    debug("buspath: Called --> Green");
    buspath.state = B_GREEN;
    signalBusCall();
    digitalWrite(BUSPATH_GREEN,HIGH);
    digitalWrite(BUSPATH_RED,LOW);
    digitalWrite(BUSPATH_CALL,LOW);
}

void buspathFromGreenToRed() {
    debug("buspath: Green --> Red");
    buspath.state = B_RED;
    busc = false;
    justCalled = true;
    signalFree();
    digitalWrite(BUSPATH_GREEN,LOW);
    digitalWrite(BUSPATH_RED,HIGH);    
}

void buspathFromGreenToCrosswalk() {
    debug("buspath: Green --> Crosswalk");
    buspath.state = B_CROSSWALK;
    busc = false;
    signalFree();
    digitalWrite(BUSPATH_GREEN,LOW);
    digitalWrite(BUSPATH_RED,HIGH);   
}

void buspathFromPreemptedToGreen() {
    debug("buspath: Preempted --> Green");
    buspath.state = B_GREEN;
    digitalWrite(BUSPATH_GREEN,HIGH);
    digitalWrite(BUSPATH_RED ,LOW);
    digitalWrite(BUSPATH_CALL,LOW);
}

void buspathFromCrosswalkToRed() {
    debug("buspath: Crosswalk --> Red");
    buspath.state = B_RED;
    justCalled = true;
}

// ======================== MAIN ========================


// the loop function runs over and over again forever
void loop() {

    // GLOBAL DELAY with interrupt flag pooling

    delayer = GLOBAL_PERIOD;
    while(delayer>0) {
        if (isrBuspath){
            isrBuspath = false;
            buspathCall();
        }
        if (isrCrosswalk){
            isrCrosswalk = false;
            crosswalkCall();
        }
        delayer = delayer - 50;
        delay(50);
    }

    // no troll
    justCalled = false;


    if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToGreen();
    } else if (trafficLightFrontBack.state == V_GREEN) {
        trafficLightFrontBackFromGreenToRed();
    }
    
    if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToGreen();
    } else if (trafficLightLeftRight.state == V_GREEN) {
        trafficLightLeftRightFromGreenToRed();
    }

    if (crosswalk.state == C_CALLED) {
        crosswalkFromCalledToGreen();
    } else if (crosswalk.state == C_GREEN) {
        if (busc) {
            crosswalkFromGreenToBuspath();
        } else {
            crosswalkFromGreenToRed();
        }
    } else if (crosswalk.state == C_DELAYEDCALL) {
        crosswalkFromDelayedCallToCalled();
    }

    if (buspath.state == B_CALLED) {
        buspathFromCalledToGreen();
    } else if (buspath.state == B_GREEN) {
        if (!pedc) {
            buspathFromGreenToRed();
        } else {
            buspathFromGreenToCrosswalk();
        }
    } else if (buspath.state == B_DELAYEDCALL) {
        if (preemptible) {
            buspathFromDelayedCallToCalled();
        } else {
            buspathFromDelayedCallToPreempted();
        }
    }


    if (!verifier()) {
        alertState();
    }
}




