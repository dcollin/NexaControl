#ifndef NEXA_SELFLEARNING_RECEIVER_H
#define NEXA_SELFLEARNING_RECEIVER_H

#include "Arduino.h"

/**
* Receiving NEXA signals used by the NEXA devices that support "Self Learning".
*/
class NexaSelfLearningReceiver{

public:

    /**
    * Constructor.
    * @arg pin - The pin used for RX communication.
    * @arg led - The pin used for indicating receiving of data. Preferably a LED. Can be NULL. (default = NULL)
    */
    NexaSelfLearningReceiver(uint8_t pin, uint8_t led = NULL);
    
    /**
    * Receive a signal over air.
    * @arg transmitter - The transmitter ID.
    * @arg group - True if the on parameter applies to the group. False if the on parameter applies to a single channel.
    * @arg on - True to turn on a device. False to turn off a device.
    * @arg channel - The channel/device the on parameter applies to.
    * @arg dim - The absolute dim value to be sent to a device. Will get value -1 if N/A. (******EXPERIMENTAL*****)
    * @arg timeout - The time (in ms) this function will be blocking while looking for a signal preamble (start point). If a signal is found before timeout it will be handled and parsed.
    */
    uint64_t receiveSignal(uint32_t* transmitter, bool* on, bool* group, uint8_t* channel, short* dim, unsigned long timeout = 100);
    
private:

    /**
    * Used internally to detect duplicate data over air.
    */
    inline bool isDuplicateSignal(const uint64_t* receivedData) const;

    uint8_t rxPin;  //the pin connected to the RX device.
    uint8_t rxLED;  //the pin connected to the RX LED. Can be NULL.
    uint64_t prevReceivedData;  //A copy of the last complete signal. Used for duplicate detection.
    unsigned long prevReceivedDataTime; //The time when the last complete signal was received. Used for duplicate detection.
    
};

#endif  //NEXA_SELFLEARNING_RECEIVER_H