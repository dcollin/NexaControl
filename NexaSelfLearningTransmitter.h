#ifndef NEXA_SELFLEARNING_TRANSMITTER_H
#define NEXA_SELFLEARNING_TRANSMITTER_H

#include "Arduino.h"
#include "NexaSelfLearningConstants.inc"

/**
* Transmitting NEXA signals used by the NEXA devices that support "Self Learning".
*/
class NexaSelfLearningTransmitter{
    
public:

    /**
    * Constructor
    * @arg pin - The pin used for TX communication
    * @arg led - The pin used for indicating transmission. Preferably a LED. Can be NULL. (default = NULL)
    */
    NexaSelfLearningTransmitter(uint8_t pin, uint8_t led = NULL);
    
    /**
    * Turn a device on
    * @arg transmitter - The transmitter ID to be sent.
    * @arg channel - The channel/device to turn on.
    */
    void deviceOn(const uint32_t transmitter, const uint8_t channel) const;
    
    /**
    * Turn a device off
    * @arg transmitter - The transmitter ID to be sent.
    * @arg channel - The channel/device to turn off.
    */
    void deviceOff(const uint32_t transmitter, const uint8_t channel) const;
    
    /***EXPERIMENTAL***
    * Send a absolute dim value to a device.
    * This function is experimental and should not be seen as stable.
    * @arg transmitter - The transmitter ID to be sent.
    * @arg channel - The channel/device to dim.
    * @arg dim - A absolute dim value (0-16).
    */
    void deviceDim(const uint32_t transmitter, const uint8_t channel, const short dim) const;
    
    /**
    * Turn a group of devices on
    * @arg transmitter - The transmitter ID to be sent.
    */
    void groupOn(const uint32_t transmitter) const;
    
    /**
    * Turn a group of devices off
    * @arg transmitter - The transmitter ID to be sent.
    */
    void groupOff(const uint32_t transmitter) const;
    
    /**
    * Transmit a signal over air.
    * @arg transmitter - The transmitter ID to be sent.
    * @arg group - True if the on parameter applies to the group. False if the on parameter applies to a single channel.
    * @arg on - True to turn on a device. False to turn off a device.
    * @arg channel - The channel/device the on parameter applies to.
    * @arg dim - The absolute dim value to be sent to a device (default = NULL). (******EXPERIMENTAL*****)
    */
    void transmitSignal(const uint32_t transmitter, const bool group, const bool on, const uint8_t channel, const short dim = -1) const;
    
    /**
    * Generate a random 26-bit Transmitter-ID. Note that this function will not generate a true random number.
    */
    static uint32_t generateUniqueTransmitterId(){
        uint32_t seed = random();   //random could contain some entropy, lets keep it.
        for(uint8_t i = 0; i < 13; ++i){
            uint16_t a = analogRead(0);  //collect some entropy from analog pin 0 (has to be unconnected)
            seed <<= 3;
            seed += ( a % 17 );
            delayMicroseconds( 7 + (a % 11) );
        }
        randomSeed(seed);   //use the generated seed
        return random(TRANSMITTER_ID_MIN, TRANSMITTER_ID_MAX);
    };
    
private:
    
    /**
    * Send a 1 "one" over air. This function is used internally by the NEXA protocol.
    */
    inline void sendOne() const;
    
    /**
    * Send a 0 "zero" over air. This function is used internally by the NEXA protocol.
    */
    inline void sendZero() const;
    
    /**
    * Send a preamble indicator over air. This function is used internally by the NEXA protocol.
    */
    inline void sendPreamble() const;
    
    /**
    * Send a end of signal indicator over air. This function is used internally by the NEXA protocol.
    */
    inline void sendEnd() const;
    
    uint8_t txPin;  //the pin connected to the TX device.
    uint8_t txLED;  //the pin connected to the TX LED. Can be NULL.
};

#endif  //NEXA_SELFLEARNING_TRANSMITTER_H