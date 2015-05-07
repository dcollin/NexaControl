#ifndef NEXA_SELF_LEARNING_CONSTANTS
#define NEXA_SELF_LEARNING_CONSTANTS

//timings (in microseconds) for preamble pulses over air.
#define PREAMBLE_HIGH       250
#define PREAMBLE_LOW        2500
#define PREAMBLE_LOW_MIN    2200    //allowed lower variation when detecting the low pulse length
#define PREAMBLE_LOW_MAX    2800    //allowed higher variation when detecting the low pulse length

//timings (in microseconds) for binary one pulses over air.
#define ONE_HIGH            250
#define ONE_LOW             1250
#define ONE_LOW_MIN         1100    //allowed lower variation when detecting the low pulse length
#define ONE_LOW_MAX         1550    //allowed higher variation when detecting the low pulse length

//timings (in microseconds) for binary zero pulses over air.
#define ZERO_HIGH           250
#define ZERO_LOW            250
#define ZERO_LOW_MIN        200    //allowed lower variation when detecting the low pulse length
#define ZERO_LOW_MAX        400    //allowed higher variation when detecting the low pulse length

//timings (in microseconds) for end pulses over air.
#define END_HIGH            250
#define END_LOW             10000
#define END_LOW_MIN         8000    //allowed lower variation when detecting the low pulse length
#define END_LOW_MAX         12000    //allowed higher variation when detecting the low pulse length

//the time (in ms) during which a duplicate signal is ignored.
#define DUPLICATE_SIGNAL_DELAY   250

//value boundaries for valid transmitter IDs.
#define TRANSMITTER_ID_MIN  1
#define TRANSMITTER_ID_MAX  67108863   //((2^26)-1)

#endif  //NEXA_SELF_LEARNING_CONSTANTS