#ifndef NEXA_SELFLEARNING_RECEIVER_H
#define NEXA_SELFLEARNING_RECEIVER_H

#include "Arduino.h"

class NexaSelfLearningReceiver{

public:
    NexaSelfLearningReceiver(uint8_t pin, uint8_t led);
    uint64_t receiveSignal(uint32_t* sender, bool* on, bool* group, uint8_t* channel, uint8_t* dim, unsigned long timeout = 100);
    
private:
    bool isDuplicateSinal(const uint64_t* receivedData) const;

    uint8_t rxPin;
    uint8_t rxLED;
    uint64_t prevReceivedSignal;
    unsigned long prevReceivedSignalTime;
    
};

#endif  //NEXA_SELFLEARNING_RECEIVER_H