#include "NexaSelfLearningReceiver.h"

#define TX_PIN          10
#define RX_PIN          4
#define RX_LED          13

//Radio RX
NexaSelfLearningReceiver receiver = NexaSelfLearningReceiver(RX_PIN, RX_LED);
short dim = 0;
uint64_t receivedSignal = 0;

//Serial RX
byte rxBuffer[79];

void setup(){
    Serial.begin(9600);
    pinMode(8, OUTPUT); digitalWrite(8, LOW);
    pinMode(9, OUTPUT); digitalWrite(9, HIGH);
    pinMode(6, OUTPUT); digitalWrite(6, LOW);
    pinMode(3, OUTPUT); digitalWrite(3, HIGH);
};

void loop(){
  
    //Receive and execute command over serial
    if(Serial.available() > 0){
        uint8_t rxDataSize = Serial.readBytesUntil('+', &rxBuffer[0], 79);
        if(rxDataSize > 0){
          if(parseRxBuffer(&rxBuffer[0], 0, rxDataSize, false, 3, 0)){
            //Serial.println(F("DEBUG:PARSE OK"));
          }else{
            //Serial.println(F("DEBUG:PARSE ERROR"));            
          }
        }
    }
    
    //Receive signal over air and send it over serial
    receivedSignal = receiver.receiveSignal(NULL, NULL, NULL, NULL, &dim, 100);
    if(receivedSignal > 0){
        Serial.print(F("+Wclass:command;protocol:arctech;model:selflearning;data:0x"));
        uint8_t hexToSend = (dim == NULL ? 8 : 9);
        for(int8_t i = hexToSend-1; i >= 0; --i){
            Serial.print( (byte)((receivedSignal>>(4*i))&0x0F), HEX);
        }
        Serial.println(F(";"));
    }
    
};

bool parseRxBuffer(byte* buffer, uint8_t startIndex, uint8_t endIndex, bool debug, uint8_t repeat, uint8_t pause){
    if(startIndex > endIndex){
        return false;
    }
    char c = buffer[startIndex];
    //Serial.print("DEBUG: char:"); Serial.println(c, DEC);
    switch(c){
        case 'S':
            return handleS(buffer, startIndex+1, endIndex, debug, repeat, pause);
        case 'T':
            return handleT(buffer, startIndex+1, endIndex, debug, repeat, pause);
        case 'V':
            Serial.println(F("+V2"));
            return parseRxBuffer(buffer, startIndex+1, endIndex, debug, repeat, pause);
        case 'D':
            return parseRxBuffer(buffer, startIndex+1, endIndex, !debug, repeat, pause);
        case 'P':
            if(endIndex-startIndex+1 < 3){return false;} //at least {'P',[p-value],'+'} must be left in the buffer
            return parseRxBuffer(buffer, startIndex+2, endIndex, debug, repeat, buffer[startIndex+1]);
        case 'R':
            if(endIndex-startIndex+1 < 3){return false;} //at least {'R',[r-value],'+'} must be left in the buffer
            return parseRxBuffer(buffer, startIndex+2, endIndex, debug, buffer[startIndex+1], pause);
        case '+':
            return true;
        default:
            //Serial.print("DEBUG: unknown char: '"); Serial.print(c, BIN); Serial.println("'");
            return false;
    }
};

bool handleS(byte* buffer, uint8_t startIndex, uint8_t endIndex, bool debug, uint8_t repeat, uint8_t pause){
    //Parse message received from serial
    uint8_t S_data[78]; //78 pulses
    uint8_t pulseCount = 0;
    for(uint8_t i = startIndex; i <= endIndex; ++i){
        if(buffer[i] == '+'){
            break;
        }else if(i == endIndex){
            return false;
        }else{
            S_data[pulseCount++] = buffer[i];
        }
    }
    //Send message
    for(uint8_t rep = 0; rep < repeat; ++rep){
        bool nextPinState = HIGH;
        for(int i = 0; i < pulseCount; ++i){
            if(S_data[i] > 0 || i == pulseCount-1){
                digitalWrite(TX_PIN, nextPinState);
                delayMicroseconds(S_data[i]*10);
            }
            nextPinState = !nextPinState;
        }
        delay(pause);
    }
    return true;
};

bool handleT(byte* buffer, uint8_t startIndex, uint8_t endIndex, bool debug, uint8_t repeat, uint8_t pause){
    //Parse message received from serial
    uint8_t T_data[72]; //0-188 pulses
    if(endIndex - startIndex < 5){
        Serial.println("DEBUG: wrong size!");
        return false;
    }
    uint8_t buff_p = startIndex;
    uint8_t T_times[4] = {buffer[buff_p++], buffer[buff_p++], buffer[buff_p++], buffer[buff_p++]};
    uint8_t T_long = buffer[buff_p++];
    uint8_t T_bytes = 0;
    if( (T_long/4.0) > (float)(T_long/4) ){
        T_bytes = T_long/4 + 1;
    }else{
        T_bytes = T_long/4;
    }
    uint8_t j = 0;
    while(j < T_bytes){
        if(buffer[buff_p] == '+'){
            break;
        }else if(buff_p >= endIndex){
            return false;
        }else{
            T_data[j++] = buffer[buff_p++];
        }
    }
    if( j != T_bytes ){
        return false;
    }
    //Send message
    for(uint8_t rep = 0; rep < repeat; ++rep){
        bool nextPinState = HIGH;
        for(int i = 0; i < T_long; ++i){
            uint8_t timeIndex = (T_data[i/4]>>(6-(2*(i%4))))&0x03;
            if(T_times[timeIndex] > 0 || i == T_long-1){
                digitalWrite(TX_PIN, nextPinState);
                delayMicroseconds(10*T_times[timeIndex]);
            }
            nextPinState = !nextPinState;
        }
        digitalWrite(TX_PIN, LOW);
        if(rep < repeat-1){
            delay(pause);
        }
    }
    return parseRxBuffer(buffer, buff_p, endIndex, debug, repeat, pause);
};

