Ringoes_Powerplant_Script.ino
V2.1.2.0
This script primarily operates the Sparkfun Bar Graph (Product Number:COM-09938)
The Bar Graph will go up by 4 every time the "Dump" Button (Attached to A0) is Pressed
The graph will go down pne bar every 22.5 Seconds, This can be changed by changing int SingleBarValue
This script Uses a shift Regester (Pins 5-7) to operate the bottom two bars of the Graph
The top bars are operated by pins 2 and 3.
The Power button (Attached to A1) prints the time on the clock and the level of the Graph to the Serial Port
If the Dump Button is pushed and there isn't enough room ther Error LED will blink and Coal will be set to ten.
There is an Error LED on Pin 13, this tells us that we hav either 1) tried to do somthing that we cannot (like dump a car without enough Room), or there is a problem with the board. When the Error LED Blinks it will send the Error code to the Serial port
Things in grey or after a double slash (//) Describe what the code Does

The script Folder contains 7 Files
1) The script file (Script.ino) This can only be opened by the arduio IDE (Freware, avalable from arduino.cc/download)
2) The wiring information in a .fzz file format, can only be opened by fritzing (Free software avalable from ___)
4) The changelog (Changlog.txt)
5) Information on the script (README.txt)
The last two files are Used by the program.
6) COAL.TXT (Used by the program to save the value of int Clock, DO NOT EDIT THIS.)
7) CLOCK.TXT (Used by the program to save the value of int Coal, DO NOT EDIT THIS.)