/*
Lakrits.h - Client for transporting data somewhat safely across a serial connection, using the lakrits communications protocol.
The protocol supports basic addressing and error checking, but that's about it. 

For the latest source and documentation, visit:
https://github.com/jpettersson/lakrits

Copyright 2011 Jonathan A Pettersson  

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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