#  example.rb - Example ruby implementation of the lakrits communication protocol.
#
#  For the latest source and documentation, visit:
#  https://github.com/jpettersson/lakrits

#  Copyright 2011 Jonathan A Pettersson  
#  You may use this software under the terms of the MIT License.


require '/Users/tfh/Projects/lakrits/ruby/lakrits.rb'

Lakrits::Log.level = Lakrits::Log::WARNING
lakrits = Lakrits.new

# Wait for the serial port to initalize properly
sleep 2


lakrits.on_message do |message|
  puts 'Lakrits: Message received: ' + message.inspect
end

#Send one message to the device with id 2000. Set the type to 1 and pass the data [2, 242, 37].
msg = { 
          :recipient_id => 2000, 
          :message_type => 1,
          :data => [2,242,37]
       }

puts 'Lakrits: Send message: ' + msg.inspect       
lakrits.deliver(msg)

loop do
  #Do other more interesting things
  sleep 1
end