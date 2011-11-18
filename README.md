#lakrits - a tiny communications protocol.
Suitable for AVR and other micro controllers with limited memory.

##Features
- 16 bit addressing
- 16 bit message type
- LRC error checking
- 64 byte payload per message

Example implementations are available for Arduino and Ruby.


##Specification

Several lakrits devices can be connected simultaneously over a wireless serial port like XBee (Series 1 and 2). In theory the devices could be connected electrically, but more effort would have to go into avoiding data collision and better error handling since the XBee does a lot of this for you. The current implementation of the protocol assumes a star topology and can only send messages between the server and the devices.

A lakrits device send and receive messages. A message is transferred in the form of a packet. A packet consist of a header, a payload and a trailer. 
The header is divided into 8 bytes and contains the "start of transmission" byte, addressing, message type and payload length. The payload begins directly after the header and continues as defined by the payload length value. After the payload, the trailer follows with a LRC checksum that is calculated over all the bytes between STX and the end of [payload]. The LRC checksum is followed by a "end of transmission" byte.

<pre>
[STX 0x02			]	Start of transmission.
[recipient id low	]	The low 8 bits of the receiving device id.
[recipient id high	]	The high 8 bits of the receiving device id.
[sender id low		]	The low 8 bits of the transmitting device id.
[sender id high		]	The high 8 bits of the transmitting device id.
[message type low	]	The low 8 bits of the message type.
[message type high	]	The high 8 bits of the message type.
[payload length		]	Indicates the number of bytes in the payload
[payload			]
...
...
...
[LRC Checksum		]
[ETX 0x03			]	End of transmission.
</pre>

##Why?

I created lakrits to power my home automation system. I needed a reliable way of transmitting tiny messages between a server and a lot of sensors and actuators. At the time I needed to concentrate on the harder problem of designing the automation system and various ways of controlling it, so I decided to stick to the minimal requirements and create the messaging protocol myself. 

This was over a year ago and I've since decided to switch my system to MQTTs, the smaller implementation intended for situations where TCP/IP is overkill (planning work on C++ and Ruby libraries). I learnt a lot about serial communication while building this and the system has been running almost flawlessly for over a year, so I figured I'll make it available in case anyone is on the same journey as I was. 

##TODO:
- Implement message numbering
- ACK
- (better) error handling.