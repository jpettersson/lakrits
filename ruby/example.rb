Lakrits::Log.level = Lakrits::Log::WARNING
lakrits = Lakrits.new

lakrits.on_message do |message|
  puts 'Lakrits: Message received: ' + message.inspect
end