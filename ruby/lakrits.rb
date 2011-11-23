#!/usr/bin/ruby 

#  lakrits.rb - Server for transporting data somewhat safely across a serial connection, using the lakrits communications protocol.
#  The protocol supports basic addressing and error checking, but that's about it.
#
#  Copyright 2011 Jonathan A Pettersson  
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.  
#
#  For the latest source and documentation, visit:
#  https://github.com/jpettersson/lakrits

require 'rubygems'
require 'serialport'
    
class Lakrits

  class DataByteSizeExceeded < RuntimeError; end

  STX = 0x02;
  ETX = 0x03;

  HEADER_LENGTH = 8;
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

  # Message packet structure

  # [STX               ]
  # [recipient id low  ]
  # [recipient id high ]
  # [sender id low     ]
  # [sender id high    ]
  # [message type low  ]
  # [message type high ]
  # [data length       ]
  # [data ...          ]
  # [checksum          ]
  # [ETX               ]
  
  attr_accessor :on_packet_callback
  
  def initialize
    
    self.on_packet_callback = nil
    
    @sp_r = SerialPort.new(0, 9600, 8, 1, SerialPort::NONE)
    @sp_w = SerialPort.new(0, 9600, 8, 1, SerialPort::NONE)
    
    @packets_o = 0
    @packets_i = 0
    
    @buffer = []
    
    spt = Thread.new() { 
      
      while true
        s = @sp_r.getbyte
        @buffer.push s 
        process
      end
    }
    spt.priority = 10
    
    Log.i "Lakrits", "Listening"
  end
  
  def on_message &block
    self.on_packet_callback = block
  end
  
  def process
    
    while @buffer.length > 0
      
      Log.i "Lakrits", @buffer.inspect
      
      # look for a potential start of packet
      if @buffer[0] == STX
      
        # wait for at least status and length
        if @buffer.length > HEADER_LENGTH
            length = @buffer[PAYLOAD_LENGTH_OFFSET]
            
            Log.i "Lakrits", "Status and length"
            #puts @buffer.length.to_s + " >= " + HEADER_LENGTH.to_s + " " + length.to_s + " " + TRAILER_LENGTH.to_s
            # if whole packet is available
            if(@buffer.length >= (HEADER_LENGTH + length + TRAILER_LENGTH))
              Log.i "Lakrits", "Potential whole package"
              if(@buffer[HEADER_LENGTH + length + TRAILER_LENGTH-1] == ETX)
                # found potential pakcet
                # test checksum
                
                lrc = 0
                
                Log.i "Lakrits", 'whole packet available'
                
                HEADER_OFFSET.upto(PAYLOAD_OFFSET+length-1) do |i|
                  lrc = lrc ^ @buffer[i]
                end
                
                if lrc == @buffer[@buffer.length-CHECKSUM_NEGATIVE_OFFSET]
                  # found packet!
                  Log.i 'Lakrits', 'Found packet! ' + @buffer.inspect                
                  
                  @packets_i += 1
                  
                  payload = []
                  PAYLOAD_OFFSET.upto(PAYLOAD_OFFSET+length-1) do |i|
                    payload << @buffer[i]
                  end
                  
                  id_low = @buffer[SENDER_ID_LOW_OFFSET].to_i
                  id_high = @buffer[SENDER_ID_HIGH_OFFSET].to_i
                  id = ( id_high << 8 | id_low )
                  
                  type_low = @buffer[MESSAGE_TYPE_LOW_OFFSET].to_i
                  type_high = @buffer[MESSAGE_TYPE_HIGH_OFFSET].to_i
                  type = ( type_high << 8 | type_low)
                  
                  message = {:sender_id => id.to_s, :message_type => type.to_s, :data => payload}
                  
                  #### Invoke callback? 
                  unless self.on_packet_callback.nil?
                    self.on_packet_callback.call(message)
                  end
                  
                  @buffer.slice! 0, HEADER_LENGTH +  length + TRAILER_LENGTH
                  
                else
                  # checksum failed
                  Log.e 'Lakrits', 'Checksum failed. ' + @buffer.inspect
                  @buffer.shift
                  
                end
                
              else
                # no ETX or ETX in wrong position
                # dump this STX
                Log.e 'Lakrits', 'Corrupt ETX. ' + @buffer.inspect
                @buffer.shift
                
              end
            else
              #puts 'waiting for moar'
              break
            end
        else
          #puts 'waiting for morr'
          break
        end
        
      else
        # must begin with STX, throw away
        Log.e 'Lakrits', 'Corrupt STX ' + @buffer.inspect
        @buffer.shift
        
      end

    end
    
  end
  
  def deliver m
    
    payload = m[:data]
    
    payload.each do |c|
      if c.to_i > 254
        raise DataByteSizeExceeded
      end
    end
    
    id_high = m[:recipient_id].to_i & 0xFF
    id_low = (m[:recipient_id].to_i >> 8) & 0xFF
    
    type_high = m[:message_type].to_i & 0xFF
    type_low = (m[:message_type].to_i >> 8) & 0xFF
    
    pkt = []
    pkt.push STX
    pkt.push id_high
    pkt.push id_low
    pkt.push 0x00
    pkt.push 0x00
    pkt.push type_high
    pkt.push type_low
    pkt.push payload.length
    
    payload.each do |c| 
      pkt.push c.to_i
    end
    
    lrc = 0
    HEADER_OFFSET.upto(PAYLOAD_OFFSET + payload.length-1) do |i|
      lrc = lrc ^ pkt[i]
    end
    
    pkt.push lrc
    pkt.push ETX
    
    Log.i 'Lakrits', 'Sending. ' + pkt.inspect
    
    @packets_o += 1
    
    pkt.each do |b|
      @sp_w.write b.chr
    end
    
  end
  
  class Log

    INFO = 3
    WARNING = 2
    ERROR = 1
    
    @@lvl = 0
    
    def self.level=val
      @@lvl = val
    end
    
    def Log.i who, str
      if @@lvl > 2
        write Log.now + ': ' + who + ': ' + str
      end
    end 

    def Log.e who, str
      if @@lvl > 0
        write Log.red( Log.now + ' [E]: ' + who + ': ' + str )
      end
    end

    def Log.w who, str
      if @@lvl > 1
        write Log.yellow( Log.now + ' [W]: ' + who + ': ' + str )
      end
    end

    def Log.now
      time = Time.new

      return time.year.to_s + '-' + time.month.to_s + '-' + time.day.to_s + ' ' + time.hour.to_s + ':' + time.min.to_s + ':' + time.sec.to_s

    end

    def Log.yellow text
      Log.colorize text, (27.chr) + "[33m"
    end

    def Log.red text
      Log.colorize text, (27.chr) + "[31m"
    end

    def Log.colorize text, color
      "#{color}#{text}" + (27.chr) + "[0m"
    end

    private

    def self.write str
      puts str
    end

    #27
  end
  
end

