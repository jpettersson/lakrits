/*
example.pde

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


#include <Lakrits.h>;

//A unique 2 byte integer as an identifier.
int id = 2000;

//Instantiate the lakrits object and pass it the unique id.
Lakrits lk = Lakrits(id);

void setup() {
  Serial.begin(9600);
  //Use the Serial class for communication.
  lk.setPrinter(Serial);

  //Set a callback for when new messages arrive. 
  lk.setOnMessageListener(onMessage);
}

void onMessage(int type, byte* data, byte length) {
  //A message was recieved
  //Let's wait a while and send it right back to the server.
  delay(1000);
  lk.sendMessage(type, data, length);
}

//In order for lakrits to properly process incoming data it might need a full clock cycle. 
//Only perform other tasks when processIO is false, this will ensure that lakrits has enough time to complete.
void loop() {
  if(!lk.processIO()) {
    //Liquorice is not doing work on this cycle, do something else.
  }
}
