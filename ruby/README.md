#lakrits - ruby server.
Some documentation

##Instantiation
Upon instantiation the server will immediately attempt to open a serial connection to the first serial port. On Mac OS this is /dev/cuaa0. If this is not the name of your port, simply symlink it
example: 
````
sudo ln /dev/tty.usbserial-A800ewIw /dev/cuaa0
````

After the port is opened a new thread is started which uses the blocking method getbyte on the serial port.

##Receive messages
To receive messages, pass a block to the on_message method:
````
salmiak.on_message do |message|
	#do something with the message
end
````
##Exceptions
In the current implementation there's only one exception: 
DataByteSizeExceeded

This is raised when the ASCII representation of a data byte is larger than 254. Larger number needs to be sent as 8 bit chunks.

##Logging
A rudimentary logging engine is included in this version if the library. It's very ugly and will be replaced at some point.