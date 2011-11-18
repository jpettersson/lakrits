/*
  Lakrits.h - Client for transporting data somewhat safely across a serial connection.
  The protocol supports basic addressing and error checking, but that's about it. 
  
  Created by Jonathan A Pettersson, Nov 14, 2011.
  Released into the public domain.
*/

//** Message packet structure

//** [STX               ]
//** [recipient id low  ]
//** [recipient id high ]
//** [sender id low     ]
//** [sender id high    ]
//** [message type low  ]
//** [message type high ]
//** [data length       ]
//** [data ...          ]
//** [checksum          ]
//** [ETX               ]

#ifndef Lakrits_h
#define Lakrits_h

#include "WProgram.h"

class Lakrits
{
  public:
	
	byte STX;
	byte ETX;
	
	int HEADER_LENGTH;
	int TRAILER_LENGTH;
	
	int HEADER_OFFSET;
	int RECIPIENT_ID_LOW_OFFSET;
	int RECIPIENT_ID_HIGH_OFFSET;
	int SENDER_ID_LOW_OFFSET;
	int SENDER_ID_HIGH_OFFSET;
	int MESSAGE_TYPE_LOW_OFFSET;
	int MESSAGE_TYPE_HIGH_OFFSET;

	int PAYLOAD_LENGTH_OFFSET;
	int PAYLOAD_OFFSET;
	
	int CHECKSUM_NEGATIVE_OFFSET;
	
	int MAX_PACKET_DATA;
	
	void onMessage(int, byte*, byte);
	
    Lakrits(int id);
	
	void setPrinter(HardwareSerial &print);
	boolean processIO();
	
	void setOnMessageListener(void (*listener)(int, byte*, byte));	
	void sendMessage(int type, byte* data, int len);
	
	byte calculateLRC(byte* bfr, int len);
	
	//Serial interface
	void appendByte(byte data);
	boolean processIBuffer();
	boolean processOBuffer();

  private:
	HardwareSerial* printer;
	
	byte id_low;
	byte id_high;
	
	byte *iShiftBuffer;
	int iBufferLen;
	byte *iBuffer;
	
	byte *oShiftBuffer;
	int oBufferLen;
	byte *oBuffer;
	
	void (*onMessageListener)(int, byte*, byte);
	
	void clearOBuffer();
	void setTrailer(int len);
	void setHeader(int type, int len);
	
	void shiftOBuffer(int n);
	void shiftIBuffer(int n);
	
	byte low(int id);
	byte high(int id);
	
	int combine(byte low, byte high);
};

#endif