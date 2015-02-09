#include "NexaSelfLearningReceiver.h"

#define RX_PIN          10
#define RX_LED          13

NexaSelfLearningReceiver receiver = NexaSelfLearningReceiver(RX_PIN, RX_LED);
uint32_t transmitter = 0;
bool on = false;
bool group = false;
uint8_t channel = 0;
short dim = 0;
uint64_t receivedSignal = 0;

void setup(){
    Serial.begin(9600);
};

void loop(){
    receivedSignal = receiver.receiveSignal(&transmitter, &on, &group, &channel, &dim, 1000);
    
    if(receivedSignal != 0){
        Serial.print("Recieved a signal: ");
        Serial.print("transmitter_id="); Serial.print(transmitter);
        Serial.print(", group="); Serial.print(group);
        Serial.print(", on="); Serial.print(on);
        Serial.print(", channel="); Serial.print(channel);
        Serial.print(", dim="); Serial.print(dim);
        Serial.println("");
    }
};