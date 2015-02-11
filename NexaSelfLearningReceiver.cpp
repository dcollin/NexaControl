#include "NexaSelfLearningReceiver.h"
#include "NexaSelfLearningConstants.inc"
#include "limits.h"

//#define DEBUG

NexaSelfLearningReceiver::NexaSelfLearningReceiver(uint8_t pin, uint8_t led) : rxPin(pin), rxLED(led), prevReceivedData(0), prevReceivedDataTime(0) {
    pinMode(rxPin, INPUT);
    if(rxLED != NULL){
        pinMode(rxLED, OUTPUT);
    }
};

uint64_t NexaSelfLearningReceiver::receiveSignal(uint32_t* transmitter, bool* on, bool* group, uint8_t* channel, short* dim, unsigned long timeout) {
  
  unsigned long startTime = millis();
  unsigned long pulseLength = 0;
    
  //wait for preamble
  while(pulseLength < PREAMBLE_LOW_MIN || PREAMBLE_LOW_MAX < pulseLength) { //allow for some variation in pulse length
    pulseLength = pulseIn(rxPin, LOW, PREAMBLE_LOW_MAX);
    if(millis()-startTime > timeout){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: timed out.");
      #endif
      return 0;
    }
  }
  
  // turn on RX led if specified
  if(rxLED != NULL){digitalWrite(rxLED, HIGH);}

  // receive subsequent data
  bool bit = 0;
  bool prevBit = 0;
  uint8_t bitsReceived = 0;
  uint64_t receivedData = 0;
  while(bitsReceived <= 72){          //0-51=transmitter, 52-53=on, 54-55=group, 56-63=channel, [64=end] or [64-71=dim, 72=END]
    pulseLength = pulseIn(rxPin, LOW, END_LOW_MAX);
    if (ZERO_LOW_MIN < pulseLength && pulseLength < ZERO_LOW_MIN){
        bit = 0;
    }else if (ONE_LOW_MIN < pulseLength && pulseLength < ONE_LOW_MAX){
        bit = 1;
    }else if(END_LOW_MIN < pulseLength && pulseLength < END_LOW_MAX){    //end -> no more data
        break;
    }else if(PREAMBLE_LOW_MIN < pulseLength && pulseLength < PREAMBLE_LOW_MAX){     //preamble -> reset receive buffer
        #ifdef DEBUG
        Serial.println("NEXA Receiver: Found a new sync point. Resetting receive buffer.");
        #endif
        receivedData = 0;
        bitsReceived = 0;
    }
    #ifdef DEBUG
    else if(pulseLength == 0){  //unknown length or pulseIn timeout
        Serial.println("NEXA Receiver: ERROR, pulse timeout.");
        break;
    }
    else{
        Serial.print("NEXA Receiver: ERROR, unknown pulse length: ");
        Serial.print(pulseLength);
        Serial.println(" microsecond(s).");
        break;
    }
    #else
    else{
        if(rxLED != NULL){digitalWrite(rxLED, LOW);}
        return 0;
    }
    #endif
    
    if(bitsReceived == 72){     //no end signal received yet
        #ifdef DEBUG
        Serial.println("NEXA Receiver: ERROR, The 72th pulse is not an END.");
        break;
        #else
        if(rxLED != NULL){digitalWrite(rxLED, LOW);}
        return 0;
        #endif
    }
 
    if (bitsReceived % 2 == 1){    //received the second bit in a pair
      if (prevBit == bit){ // only "01" or "10" ok
        #ifdef DEBUG
        Serial.println("NEXA Receiver: ERROR, received a faulty pulse pair.");
        break;
        #else
        if(rxLED != NULL){digitalWrite(rxLED, LOW);}
        return 0;
        #endif
      }    
      receivedData <<= 1;
      receivedData |= prevBit;
    }
    
    prevBit = bit;
    bitsReceived++;
  }
  
  // turn off RX led if specified
  if(rxLED != NULL){digitalWrite(rxLED, LOW);}
  
  if(isDuplicateSignal(&receivedData)){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: ERROR, Received yet a another copy of the previous message - ignoring it.");
      #endif
      return 0;
  }else if(bitsReceived == 64){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: INFO, Received a signal with 64 pulses (32 bits) of data (no dim).");
      #endif
      if(transmitter != NULL){*transmitter = receivedData >> 6;}
      if(group != NULL){*group = (receivedData >> 5) & 0x01;}
      if(on != NULL){*on = (receivedData >> 4) & 0x01;}
      if(channel != NULL){*channel = receivedData & 0x0F;}
      if(dim != NULL){*dim = -1;}
  }else if(bitsReceived == 72){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: INFO, Received a signal with 72 pulses (36 bits) of data (with dim).");
      #endif
      if(transmitter != NULL){*transmitter = receivedData >> 10;}
      if(group != NULL){*group = (receivedData >> 9) & 0x01;}
      if(on != NULL){*on = (receivedData >> 8) & 0x01;}
      if(channel != NULL){*channel = (receivedData >> 4) & 0x0F;}
      if(dim != NULL){*dim = receivedData & 0x0F;}
  }else{
      #ifdef DEBUG
      Serial.print("NEXA Receiver: ERROR, The signal contains a non supported number of bits (");
      Serial.print(bitsReceived);
      Serial.println("), ignoring signal");
      #endif
      return 0;
  }
  
  #ifdef DEBUG
  Serial.print("Raw = ");
  for(int i = (*dim==-1?32:36); i >= 0; --i){
      Serial.print( (byte)((receivedData>>i) & 0x01), BIN);
  }
  Serial.print(", transmitter_id="); Serial.print(*transmitter);
  Serial.print(", group="); Serial.print(*group);
  Serial.print(", on="); Serial.print(*on);
  Serial.print(", channel="); Serial.print(*channel);
  Serial.print(", dim="); Serial.print(*dim);
  (*dim == -1) ? Serial.println("(not set)") : Serial.println("");
  #endif
  
  prevReceivedData = receivedData;
  prevReceivedDataTime = millis();
  return receivedData;
};

bool NexaSelfLearningReceiver::isDuplicateSignal(const uint64_t* receivedData) const{
    if(prevReceivedData == *receivedData){
        unsigned long currentTime = millis();
        if(currentTime > prevReceivedDataTime){
            return ( currentTime-prevReceivedDataTime < DUPLICATE_SIGNAL_DELAY );
        }else if(currentTime < prevReceivedDataTime){
            return ( (ULONG_MAX - prevReceivedDataTime) + currentTime < DUPLICATE_SIGNAL_DELAY );
        }
    }
    return false;
};