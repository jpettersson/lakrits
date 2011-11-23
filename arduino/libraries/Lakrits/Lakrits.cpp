/*
Lakrits.cpp - Client for transporting data somewhat safely across a serial connection, using the lakrits communications protocol.
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

#include "WProgram.h"
#include "Lakrits.h"

Lakrits::Lakrits(int id)
{	
	id_low = low(id);
	id_high = high(id);

	STX = 0x02;
	ETX = 0x03;
	
	HEADER_LENGTH = 7;
	TRAILER_LENGTH = 2;
	
	HEADER_OFFSET = 1;
	RECIPIENT_ID_LOW_OFFSET = 1;
	RECIPIENT_ID_HIGH_OFFSET = 2;
	SENDER_ID_LOW_OFFSET = 3;
	SENDER_ID_HIGH_OFFSET = 4;
	MESSAGE_TYPE_LOW_OFFSET = 5;
	MESSAGE_TYPE_HIGH_OFFSET = 6;
	
	PAYLOAD_LENGTH_OFFSET = 7;
	PAYLOAD_OFFSET = 8;
	
	CHECKSUM_NEGATIVE_OFFSET = 2;
	
	MAX_PACKET_DATA = 64;
	
	iBufferLen = 0;
	iBuffer = (byte *) calloc(MAX_PACKET_DATA, 1);
	iShiftBuffer = (byte *) calloc(MAX_PACKET_DATA, 1);
	
	oBufferLen = 0;
	oBuffer = (byte *) calloc(MAX_PACKET_DATA, 1);
	oShiftBuffer = (byte *) calloc(MAX_PACKET_DATA, 1);
	
	onMessageListener = 0;
}

void Lakrits::setPrinter(HardwareSerial &print) {
	printer = &print; //operate on the adress of print
}

boolean Lakrits::processIO() {
 if(!processOBuffer()) {  
   if(printer->available() > 0) {
     byte b = (byte) printer->read();
     appendByte(b); 
   }else{
     if(!processIBuffer()) {
	  return false;
     } 
   }
 }

 return true;
}

void Lakrits::sendMessage(int type, byte* data, int len) {
	
	clearOBuffer();
	setHeader(type, len);
	
	for(int i=0;i<len;i++) {
		oBuffer[PAYLOAD_OFFSET + i] = data[i];
	}

	setTrailer(len);
}

void Lakrits::setTrailer(int dataLen) {
	oBuffer[PAYLOAD_OFFSET + dataLen] = calculateLRC(oBuffer, dataLen);
	oBuffer[PAYLOAD_OFFSET + dataLen + 1] = ETX;	
	oBufferLen = HEADER_LENGTH + dataLen + TRAILER_LENGTH + 1;
}

void Lakrits::setHeader(int type, int dataLen) {
	oBuffer[0] = STX;
	oBuffer[RECIPIENT_ID_LOW_OFFSET] = 0;
	oBuffer[RECIPIENT_ID_HIGH_OFFSET] = 0;
	oBuffer[SENDER_ID_LOW_OFFSET] = id_low;
	oBuffer[SENDER_ID_HIGH_OFFSET] = id_high;
	oBuffer[MESSAGE_TYPE_LOW_OFFSET] = low(type);
	oBuffer[MESSAGE_TYPE_HIGH_OFFSET] = high(type);
	oBuffer[PAYLOAD_LENGTH_OFFSET] = dataLen;
}

void Lakrits::clearOBuffer() {
	oBufferLen = 0;
	for(int i=0;i<MAX_PACKET_DATA;i++) {
		oBuffer[i] = 0;
	}
}

boolean Lakrits::processOBuffer()
{
	if(oBufferLen > 0) {
		Serial.print(oBuffer[0], BYTE);
		shiftOBuffer(1);
		return true;
	}
	
	return false;
}

void Lakrits::appendByte(byte b) {
	iBuffer[iBufferLen] = b;
	iBufferLen ++;
}

boolean Lakrits::processIBuffer() {
	byte dataLen = 0;
	
	if(iBufferLen > 0) {
	
		// look for a potential start of packet
		if(iBuffer[0] == STX) {
			// wait for at least length
			if(iBufferLen >= HEADER_LENGTH) {
				dataLen = iBuffer[PAYLOAD_LENGTH_OFFSET];
			
				//if whole packet is available
				if(iBufferLen > HEADER_LENGTH + dataLen + TRAILER_LENGTH) {
					if(iBuffer[HEADER_LENGTH + dataLen + TRAILER_LENGTH] == ETX) {
					
						// found potential packet
						// test checksum
				
						byte lrc = calculateLRC(iBuffer, dataLen);
					
						if(lrc == iBuffer[PAYLOAD_OFFSET + dataLen]) {
							// found packet!
							
							//Is it for me?
							if(iBuffer[RECIPIENT_ID_LOW_OFFSET] == id_low && iBuffer[RECIPIENT_ID_HIGH_OFFSET] == id_high) {
								
								///Serial.print(5, BYTE);	
								
								byte type_low = iBuffer[MESSAGE_TYPE_LOW_OFFSET];
								byte type_high = iBuffer[MESSAGE_TYPE_HIGH_OFFSET];
																
								int type = combine(type_low, type_high);
								//Remove the header
								shiftIBuffer(PAYLOAD_OFFSET);
								
								onMessage(type, iBuffer, dataLen);
								
							}else{
								///Serial.print(4, BYTE);
							}
							
							shiftIBuffer(iBufferLen);
						}else{
							///Serial.print(3, BYTE);
							shiftIBuffer(1);
						}
					}else{
						///Serial.print(2, BYTE);
						shiftIBuffer(1);
					}
				}else{
					//wait for more
				}
			}else{
				//wait for more
			}
		}else{
			///Serial.print(1, BYTE);
			shiftIBuffer(1);
		}
	
	}else{
		return false;
	}
	
	return true;
	
}

void Lakrits::onMessage(int type, byte* data, byte dataLen) {
	onMessageListener(type, data, dataLen);
}

void Lakrits::shiftIBuffer(int n) {
	
	for(int i=n;i<iBufferLen;i++) {
		iBuffer[i-n] = iBuffer[i];
	}
	
	iBufferLen-=n;
	if(iBufferLen < 0) {
		iBufferLen = 0;
	}
	
}

void Lakrits::shiftOBuffer(int n) {
	
	for(int i=1;i<oBufferLen;i++) {
		oBuffer[i-1] = oBuffer[i];
	}
	
	oBufferLen-=n;
	if(oBufferLen < 0) {
		oBufferLen = 0;
	}
	
}

void Lakrits::setOnMessageListener(void (*listener)(int, byte*, byte))
{
	onMessageListener = listener;
}

byte Lakrits::calculateLRC(byte* data, int len) {
    byte lrc = 0;
	
    for( int x=HEADER_OFFSET ; x < PAYLOAD_OFFSET+len ; x++ ){
      lrc = (byte) lrc ^ (byte) data[x];
    }
	
	return lrc;		
}

byte Lakrits::low(int value) {
	return value & 0xFF;
}

byte Lakrits::high(int value) {
	return (value >> 8) & 0xFF;
}

int Lakrits::combine(byte low_byte, byte high_byte) {
	unsigned int word = high_byte * 256 + low_byte;
	return word;
}

