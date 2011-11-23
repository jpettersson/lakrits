#  example.rb
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

require '/Users/tfh/Projects/lakrits/ruby/lakrits.rb'

Lakrits::Log.level = Lakrits::Log::WARNING
lakrits = Lakrits.new

lakrits.on_message do |message|
  puts 'Lakrits: Message received: ' + message.inspect
end

sleep 2

#Send one message to the device with id 2000. Set the type to 1 and pass the data [2, 242, 37].
msg = { 
          :recipient_id => 2000, 
          :message_type => 1,
          :data => ["2","242","37"]
       }
       
lakrits.deliver(msg)

loop do
  #Do other more interesting things
  sleep 1
end