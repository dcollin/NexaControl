#include "NexaSelfLearningTransmitter.h"
#include "NexaSelfLearningConstants.inc"

NexaSelfLearningTransmitter::NexaSelfLearningTransmitter(uint8_t pin, uint8_t led) : txPin(pin), txLED(led) {
    pinMode(txPin, OUTPUT);
    if(txLED != NULL){
        pinMode(txLED, OUTPUT);
    }
};

void NexaSelfLearningTransmitter::deviceOn(const uint32_t transmitter, const uint8_t channel) const {
    transmitSignal(transmitter, false, true, channel);
};

void NexaSelfLearningTransmitter::deviceOff(const uint32_t transmitter, const uint8_t channel) const {
    transmitSignal(transmitter, false, false, channel);
};

void NexaSelfLearningTransmitter::deviceDim(const uint32_t transmitter, const uint8_t channel, const short dim) const {
    transmitSignal(transmitter, false, true, channel, dim);
};

void NexaSelfLearningTransmitter::groupOn(const uint32_t transmitter) const {
    transmitSignal(transmitter, true, true, 0);
};

void NexaSelfLearningTransmitter::groupOff(const uint32_t transmitter) const {
    transmitSignal(transmitter, true, false, 0);
};

void NexaSelfLearningTransmitter::transmitSignal(const uint32_t transmitter, const bool group, const bool on, const uint8_t channel, const short dim) const {
    
    //fill send buffer with data
    uint64_t sendBuffer = 0;
    sendBuffer |= (transmitter & 0x3FFFFFF);
    sendBuffer <<= 1;
    sendBuffer |= (group & 0x01);
    sendBuffer <<= 1;
    sendBuffer |= (on & 0x01);
    sendBuffer <<= 4;
    sendBuffer |= (channel & 0x0F);
    if(dim != -1){
        sendBuffer <<= 4;
        sendBuffer |= (dim & 0x0F);
    }
    
    short bufferLength = (dim == -1 ? 32 : 36);
    
    // turn on TX led if specified
    if(txLED != NULL){digitalWrite(txLED, HIGH);}
    
    //send the buffer 4 times
    for (uint8_t i = 0; i < 4; ++i) {
        
        sendPreamble();
        
        //send the data
        for(short j = bufferLength-1; j >= 0; --j){   //send all bits in buffer
            bool bit = (sendBuffer >> j) & 0x01;
            if(bit){
                //send a binary "1" as "10" over air
                sendOne();
                sendZero();
            }else{
                //send a binary "0" as "01" over air
                sendZero();
                sendOne();
            }
        }
        
        sendEnd();
    
    }
    
    // turn off TX led if specified
    if(txLED != NULL){digitalWrite(txLED, LOW);}
    
};

void NexaSelfLearningTransmitter::sendPreamble() const {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(PREAMBLE_HIGH);
    digitalWrite(txPin, LOW);
    delayMicroseconds(PREAMBLE_LOW);
}

void NexaSelfLearningTransmitter::sendEnd() const {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(END_HIGH);
    digitalWrite(txPin, LOW);
    delayMicroseconds(END_LOW);
}

void NexaSelfLearningTransmitter::sendZero() const {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(ZERO_HIGH);
    digitalWrite(txPin, LOW);
    delayMicroseconds(ZERO_LOW);
};

void NexaSelfLearningTransmitter::sendOne() const {
    digitalWrite(txPin, HIGH);
    delayMicroseconds(ONE_HIGH);
    digitalWrite(txPin, LOW);
    delayMicroseconds(ONE_LOW);
};

