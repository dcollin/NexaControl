#include "NexaSelfLearningReceiver.h"
#include "limits.h"

//#define DEBUG
#define DUPLICATE_SIGNAL_DELAY 1000

NexaSelfLearningReceiver::NexaSelfLearningReceiver(uint8_t pin, uint8_t led) : rxPin(pin), rxLED(led), prevReceivedSignal(0), prevReceivedSignalTime(0) {
    pinMode(rxPin, INPUT);
    pinMode(rxLED, OUTPUT);
};

uint64_t NexaSelfLearningReceiver::receiveSignal(uint32_t* sender, bool* on, bool* group, uint8_t* channel, short* dim, unsigned long timeout) {
  
  unsigned long startTime = millis();
  unsigned long pulseLength = 0;
  bool prevBit = 0;
  bool bit = 0;
  uint64_t receivedData = 0;
  
  //wait for preamble
  pulseLength = 0;
  while(pulseLength < 2200 || pulseLength > 2800) { //allow for some variation in pulse length
    pulseLength = pulseIn(rxPin, LOW, 2900);
    if(millis()-startTime > timeout){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: timed out.");
      #endif
      return 0;
    }
  }
  
  digitalWrite(rxLED, HIGH);

  // receive subsequent data
  uint8_t bitsReceived = 0;
  bool error = false;
  while(bitsReceived <= 72){          //0-51=sender, 52-53=on, 54-55=group, 56-63=channel, [64=end] or [64-71=dim, 72=END]
    pulseLength = pulseIn(rxPin, LOW, 12000);
    if (pulseLength > 200 && pulseLength < 400){
        bit = 0;
    }else if (pulseLength > 1100 && pulseLength < 1550){
        bit = 1;
    }else if(pulseLength > 8000 && pulseLength < 12000){    //end
        break;
    }else{  //unknown length or pulseIn timeout
        error = true;
        break;
    }
    if(bitsReceived == 72){     //no end signal received yet
        Serial.println("NEXA Receiver: ERROR, The 72th pulse is not a END.");
        error = true;
        break;
    }
 
    if (bitsReceived % 2 == 1){    //received the second bit in a pair
      if (prevBit == bit){ // only "01" or "10" ok
        error = true;
        break;
      }    
      receivedData <<= 1;
      receivedData |= prevBit;
    }
    
    prevBit = bit;
    bitsReceived++;
  }
  
  digitalWrite(rxLED, LOW);
  

  if(error){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: ERROR, Received corrupt signal over air:");
      for(int i = bitsReceived-1; i >= 0; --i){
          Serial.print( (receivedData >> i)&&0x0F, BIN);
      }
      Serial.println("");
      Serial.print("length = ");
      Serial.println(bitsReceived);
      #endif
      return 0;
  }else if(isDuplicateSinal(&receivedData)){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: ERROR, Received yet a another copy of the previous message - ignoring it.");
      #endif
      return 0;
  }else if(bitsReceived == 64){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: INFO, Received a signal with 64 pulses (32 bits) of data (no dim).");
      #endif
      *sender = receivedData >> 6;
      *group = (receivedData >> 5) & 0x01;
      *on = (receivedData >> 4) & 0x01;
      *channel = receivedData & 0x0F;
      *dim = -1;
  }else if(bitsReceived == 72){
      #ifdef DEBUG
      Serial.println("NEXA Receiver: INFO, Received a signal with 72 pulses (36 bits) of data (with dim).");
      #endif
      *sender = receivedData >> 10;
      *group = (receivedData >> 9) & 0x01;
      *on = (receivedData >> 8) & 0x01;
      *channel = (receivedData >> 4) & 0x0F;
      *dim = receivedData & 0x0F;
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
  Serial.print(", transmitter_id="); Serial.print(*sender);
  Serial.print(", group="); Serial.print(*group);
  Serial.print(", on="); Serial.print(*on);
  Serial.print(", channel="); Serial.print(*channel);
  Serial.print(", dim="); Serial.print(*dim);
  if(*dim == -1){
      Serial.println("(not set)");
  }else{
      Serial.println("");
  }
  #endif
  
  prevReceivedSignal = receivedData;
  prevReceivedSignalTime = millis();
  return receivedData;
};

bool NexaSelfLearningReceiver::isDuplicateSinal(const uint64_t* receivedData) const{
    if(prevReceivedSignal == *receivedData){
        unsigned long currentTime = millis();
        if(currentTime > prevReceivedSignalTime){
            return ( currentTime-prevReceivedSignalTime < DUPLICATE_SIGNAL_DELAY );
        }else if(currentTime > prevReceivedSignalTime){
            return ( (ULONG_MAX - prevReceivedSignalTime) + currentTime < DUPLICATE_SIGNAL_DELAY );
        }
    }
    return false;
};