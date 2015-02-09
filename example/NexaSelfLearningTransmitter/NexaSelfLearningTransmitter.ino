#include "NexaSelfLearningTransmitter.h"

#define TX_PIN          10
#define TX_LED          13
#define TRANSMITTER_ID  37

NexaSelfLearningTransmitter transmitter = NexaSelfLearningTransmitter(TX_PIN, TX_LED);

void setup(){
    Serial.begin(9600);
    
    Serial.println("--------------[device 0 ON]--------------");
    transmitter.deviceOn(TRANSMITTER_ID, 0);
    delay(5000);
    
    Serial.println("--------------[device 1 OFF]--------------");
    transmitter.deviceOff(TRANSMITTER_ID, 1);
    delay(5000);
    
    Serial.println("--------------[device 2 DIM 13]--------------");
    transmitter.deviceDim(TRANSMITTER_ID, 2, 13);
    delay(5000);
    
    Serial.println("--------------[group ON]--------------");
    transmitter.groupOn(TRANSMITTER_ID);
    delay(5000);
    
    Serial.println("--------------[group OFF]--------------");
    transmitter.groupOff(TRANSMITTER_ID);
};

void loop(){
    
};