#include "NexaSelfLearningReceiver.h"

#define RX_PIN          10
#define RX_LED          13

NexaSelfLearningReceiver receiver = NexaSelfLearningReceiver(RX_PIN, RX_LED);
uint32_t transmitter = 0;
bool on = false;
bool group = false;
uint8_t channel = 0;
uint8_t dim = 0;
uint64_t receivedSignal = 0;

void setup(){
    Serial.begin(9600);
};

void loop(){
    receivedSignal = receiver.receiveSignal(&transmitter, &on, &group, &channel, &dim, 1000);
    if(receivedSignal != 0){
        Serial.println("*******************************");
        Serial.println("Recieved a signal:");
        Serial.println(receivedSignal, BIN);
        Serial.print("Transmitter ID = "); Serial.println(transmitter);
        Serial.print("ON = "); Serial.println(on);
        Serial.print("Group = "); Serial.println(group);
        Serial.print("Channel = "); Serial.println(channel);
        Serial.print("Dim = "); Serial.println(dim);
        Serial.println("*******************************");
    }
};