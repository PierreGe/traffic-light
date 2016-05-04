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
enum CrosswalkTrafficLightStates { C_GREEN, C_RED, C_CALLED, C_DELAYEDCALL };
enum BusTrafficLightStates { B_GREEN, B_RED, B_CALLED, B_DELAYEDCALL };
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
bool justCalledCrosswalk = false;
bool justCalledBusPath = false;
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

// ================ TRAFFIC LIGHT LEFT RIGHT ================

void trafficLightLeftRightFromGreenToRed() {
    debug("trafficLightLeftRightFromGreenToRed()");
    trafficLightLeftRight.state = V_RED;
    digitalWrite(LEFT_RIGHT_RED, HIGH);
    digitalWrite(LEFT_RIGHT_GREEN, LOW);
}

void trafficLightLeftRightFromRedToGreen() {
    debug("trafficLightLeftRightFromRedToGreen()");
    trafficLightLeftRight.state = V_GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);

}

// crosswalk
void trafficLightLeftRightFromRedToCrosswalk() {
    debug("trafficLightLeftRightFromRedToCrosswalk()");
    trafficLightLeftRight.state = V_CROSSWALK;
    prec = true;
}

void trafficLightLeftRightFromCrosswalkToGreen() {
    debug("trafficLightLeftRightFromCrosswalkToGreen()");
    trafficLightLeftRight.state = V_GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);
}

void trafficLightLeftRightFromCrosswalkToRed() {
    debug("trafficLightLeftRightFromCrosswalkToRed()");
    trafficLightLeftRight.state = V_RED;
}

// bus
void trafficLightLeftRightFromRedToBuspath() {
    debug("trafficLightLeftRightFromRedToBuspath()");
    trafficLightLeftRight.state = V_BUSPATH;
    prec = true;
}

void trafficLightLeftRightFromBuspathToGreen() {
    debug("trafficLightLeftRightFromBuspathToGreen()");
    trafficLightLeftRight.state = V_GREEN;
    digitalWrite(LEFT_RIGHT_RED, LOW);
    digitalWrite(LEFT_RIGHT_GREEN, HIGH);
}

void trafficLightLeftRightFromBuspathToRed() {
    debug("trafficLightLeftRightFromBuspathToRed()");
    trafficLightLeftRight.state = V_RED;
}

// ================ TRAFFIC LIGHT FRONT BACK ================

void trafficLightFrontBackFromGreenToRed() {
    debug("trafficLightFrontBackFromGreenToRed()");
    trafficLightFrontBack.state = V_RED;
    digitalWrite(FRONT_BOTTOM_RED, HIGH);
    digitalWrite(FRONT_BOTTOM_GREEN, LOW);
}

void trafficLightFrontBackFromRedToGreen() {
    debug("trafficLightFrontBackFromRedToGreen()");
    trafficLightFrontBack.state = V_GREEN;
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

// crosswalk
void trafficLightFrontBackFromCrosswalkToGreen() {
    debug("trafficLightFrontBackFromCrosswalkToGreen()");
    trafficLightFrontBack.state = V_GREEN;
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

void trafficLightFrontBackFromCrosswalkToRed() {
    debug("trafficLightFrontBackFromCrosswalkToRed()");
    trafficLightFrontBack.state = V_RED;
}

void trafficLightFrontBackFromRedToCrosswalk() {
    debug("trafficLightFrontBackFromRedToCrosswalk()");
    trafficLightFrontBack.state = V_CROSSWALK;
    prec = false;
}

// bus
void trafficLightFrontBackFromBuspathToGreen() {
    debug("trafficLightFrontBackFromBuspathToGreen()");
    trafficLightFrontBack.state = V_GREEN;
    digitalWrite(FRONT_BOTTOM_RED, LOW);
    digitalWrite(FRONT_BOTTOM_GREEN, HIGH);
}

void trafficLightFrontBackFromBuspathToRed() {
    debug("trafficLightFrontBackFromBuspathToRed()");
    trafficLightFrontBack.state = V_RED;
}

void trafficLightFrontBackFromRedToBuspath() {
    debug("trafficLightFrontBackFromRedToBuspath()");
    trafficLightFrontBack.state = V_BUSPATH;
    prec = false;
}


// ======================== CROSSWALK ========================

void crosswalkCallISR(){
    isrCrosswalk = true;
}

// Interupt routine
void crosswalkCall(){
    debug("crosswalkCall() : ISR : button pressed");
    if (!justCalledCrosswalk and buspath.state == B_GREEN) {
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
    debug("crosswalkFromRedToDelayedCall()");
    crosswalk.state = C_DELAYEDCALL;
    digitalWrite(CROSSWALK_CALL,HIGH);
}

void crosswalkFromRedToCalled() {
    debug("crosswalkFromRedToCalled()");
    crosswalk.state = C_CALLED;
    digitalWrite(CROSSWALK_CALL,HIGH);
}

void crosswalkFromDelayedCallToCalled() {
    debug("crosswalkFromDelayedCallToCalled()");
    crosswalk.state = C_CALLED;
    if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToCrosswalk();
    }
    if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToCrosswalk();
    }
}

void crosswalkFromCalledToGreen() {
    debug("crosswalkFromCalledToGreen()");
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
    debug("crosswalkFromGreenToRed()");
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
    justCalledCrosswalk = true;
}

// ======================== BUSPATH ========================

void buspathCallISR(){
    isrBuspath = true;
}

//interupt routine
void buspathCall(){
    debug("buspathCall() : ISR : button pressed");
    if (!justCalledBusPath and crosswalk.state == C_GREEN) {
        if (buspath.state == B_RED) {
            buspathFromRedToCalled();
            if (trafficLightFrontBack.state == V_RED) {
                trafficLightFrontBackFromRedToCrosswalk();
            }
            if (trafficLightLeftRight.state == V_RED) {
                trafficLightLeftRightFromRedToCrosswalk();
            }
        }
    }
    else {
        buspathFromRedToDelayedCall();
    }
    if (!verifier()) {
        alertState();
    }
}

void buspathFromRedToDelayedCall() {
    debug("buspathFromRedToDelayedCall()");
    buspath.state = B_DELAYEDCALL;
    digitalWrite(BUSPATH_CALL,HIGH);
}

void buspathFromRedToCalled() {
    debug("buspathFromRedToCalled()");
    buspath.state = B_CALLED;
    digitalWrite(BUSPATH_CALL,HIGH);
}

void buspathFromDelayedCallToCalled() {
    debug("buspathFromDelayedCallToCalled()");
    buspath.state = B_CALLED;
    if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToBuspath();
    }
    if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToBuspath();
    }
}

void buspathFromCalledToGreen() {
    debug("buspathFromCalledToGreen()");
    if (trafficLightFrontBack.state == V_RED) {
        trafficLightFrontBackFromRedToBuspath();
    }
    if (trafficLightLeftRight.state == V_RED) {
        trafficLightLeftRightFromRedToBuspath();
    }
    buspath.state = B_GREEN;
    digitalWrite(BUSPATH_GREEN,HIGH);
    digitalWrite(BUSPATH_RED,LOW);
    digitalWrite(BUSPATH_CALL,LOW);
}

void buspathFromGreenToRed() {
    debug("buspathFromGreenToRed()");
    buspath.state = B_RED;
    digitalWrite(BUSPATH_GREEN,LOW);
    digitalWrite(BUSPATH_RED,HIGH);
    if ( prec == true ) {
        trafficLightFrontBackFromBuspathToGreen();
        trafficLightLeftRightFromBuspathToRed();
    } else { // prec == false
        trafficLightFrontBackFromBuspathToRed();
        trafficLightLeftRightFromBuspathToGreen();
    }
    justCalledBusPath = true;
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

    justCalledBusPath = false;
    justCalledCrosswalk = false;


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


    if (buspath.state != B_CALLED){
        if (crosswalk.state == C_CALLED) {
            crosswalkFromCalledToGreen();
        } else if (crosswalk.state == C_GREEN) {
            crosswalkFromGreenToRed();
        } else if (crosswalk.state == C_DELAYEDCALL) {
            crosswalkFromDelayedCallToCalled();
        }
    }

    if (buspath.state == B_CALLED) {
        buspathFromCalledToGreen();
    } else if (buspath.state == B_GREEN) {
        buspathFromGreenToRed();
    } else if (buspath.state == B_DELAYEDCALL) {
        buspathFromDelayedCallToCalled();
    }


    if (!verifier()) {
        alertState();
    }
}




