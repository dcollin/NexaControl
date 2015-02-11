#ifndef NEXA_SELFLEARNING_TRANSMITTER_H
#define NEXA_SELFLEARNING_TRANSMITTER_H

#include "Arduino.h"

class NexaSelfLearningTransmitter{
    
public:
    NexaSelfLearningTransmitter(uint8_t pin, uint8_t led);
    
    void deviceOn(const uint32_t transmitter, const uint8_t channel) const;
    void deviceOff(const uint32_t transmitter, const uint8_t channel) const;
    void deviceDim(const uint32_t transmitter, const uint8_t channel, const short dim) const;
    void groupOn(const uint32_t transmitter) const;
    void groupOff(const uint32_t transmitter) const;
    
private:
    void transmitSignal(const uint32_t transmitter, const bool group, const bool on, const uint8_t channel, const short dim = -1) const;
    inline void sendOne() const;
    inline void sendZero() const;
    inline void sendPreamble() const;
    inline void sendEnd() const;
    
    uint8_t txPin;
    uint8_t txLED;
};

#endif  //NEXA_SELFLEARNING_TRANSMITTER_H