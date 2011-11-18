/*
Lakrits.h - Client for transporting data somewhat safely across a serial connection, using the lakrits communications protocol.
The protocol supports basic addressing and error checking, but that's about it. 

Copyright 2011 Jonathan A Pettersson  

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  

For the latest source and documentation, visit:
https://github.com/jpettersson/lakrits
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