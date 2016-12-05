/*
  Ringoes_Powerplant_Script.ino
  V2.1.2.1
  Written by Carlin Greenfield
  This script primarily operates the Sparkfun Bar Graph (Product Number:COM-09938)
  The Bar Graph will go up by 4 every time the "Dump" Button (Attached to A0) is Pressed
  The graph will go down pne bar every 22.5 Seconds, This can be changed by changing int SingleBarValue
  This script Uses a shift Regester (Pins 5-7) to operate the bottom two bars of the Graph
  The top bars are operated by pins 2 and 3.
  The Power button (Attached to A1) prints the time on the clock and the level of the Graph to the Serial Port
  If the Dump Button is HIGH and there isn't enough room ther Error LED will blink and Coal will be set to ten.
  There is an Error LED on Pin 13, this tells us that we hav either 1) tried to do somthing that we cannot (like dump a car without enough Room), or there is a problem with the board. When the Error LED Blinks it will send the Error code to the Serial port
  Things in grey or after a double slash (//) Describe what the code Does

  The script Folder contains 7 Files
  1) The script file (Script.ino) This can only be opened by the arduio IDE (Freware, avalable from arduino.cc/download)
  2) The wiring information in a .fzz file format, can only be opened by fritzing (Free software avalable from ___)
  4) The changelog (Changlog.txt)
  5) Information on the script (README.txt)
  The last two files are Used by the program.
  6) COAL.TXT (Used by the program to save the value of int Clock, DO NOT EDIT THIS.)
  7) CLOCK.TXT (Used by the program to save the value of int Coal, DO NOT EDIT THIS.)*/
//Next two lines Include Libraries used for the SD Card
#include <SPI.h>
#include <SD.h>
//<Declares Pins>
#define GphDataPin 5 //Declares the Data Pin for the IC that will control the Bar Graph
#define GphClockPin 7 //Declares the Clock Pin for the IC that will control the Bar Graph
#define GphLatchPin 6 //Declares the Latch Pin for the IC that will control the Bar Graph
#define ChipSelect 4 //Declares Chipselect, Used by the SD Library.
#define GphNine 2 //The IC only has 8 Outputs, so the top two bars of the graph have their own pins - This is pin 9
#define GphTen 3 //The IC only has 8 Outputs, so the top two bars of the graph have their own pins - This is pin 10
#define PwrBttnPin 15 // This is the input for the Power Button, 15 Tells the Board to use Analog Pin 1 as a Digital Pin
#define DmpBttnPin 14 //This is the input for the Power Button, 14 Tells the Board to use Analog Pin 0 as a Digital Pin
#define UILED 8 //This is the LED that Lights up if somthing goes wrong.
const byte GphNineAndTen[] {GphNine, GphTen}; //this is an array of the 2 pins not controled by the shft Regester. It allows us to use a for loop to use the top 2 LEDs.
//</Declares Pins>
#define SingleBarValue ((1.5 * 60 * 1000) / (4)) //Number is equivelent to (Minutes per Car*60*1000)/(Bars per Car)
byte Coal; //this will be the Bars on the Graph
int Clock; //Milliseconds
boolean DebugMode = false; //If set to true will activate debug mode, Debugging mode can also be activated by pressing power button and dump button simoltaniouslty for 1 seccond
File fileVar;

void setup()
{
  pinMode(UILED, OUTPUT); //Sets the UILED pin as an output, this is put seperate from the rest of the pin declerations so the the UI LED can be on for the whole setup.
  digitalWrite(UILED, HIGH);//Turns on the User Interface LED, this allows us to know that the program is starting.
  //<Sets Pins as inputs and Outputs>
  pinMode(GphDataPin, OUTPUT);
  pinMode(GphClockPin, OUTPUT);
  pinMode(GphLatchPin, OUTPUT);
  pinMode(GphNine, OUTPUT);
  pinMode(GphTen, OUTPUT);
  pinMode(ChipSelect, OUTPUT);
  pinMode(PwrBttnPin, INPUT);
  pinMode(DmpBttnPin, INPUT);
  //</Sets Pins as inputs and Outputs>
  Serial.begin(9600); //Begins Serial Communication for Debugging
  //<Initilaizes SD Card>
  if (!SD.begin(ChipSelect))//if unable to load SD card
  { //Print an error Code to the serial port NOTE: I used NOT initilized as the if statement because that is what the program is checking for, it could be reversed, I didn't
    BlinkUILED();//Blink the User interface LED
    Serial.print("millis=");//Print the Quoted text to the serial port
    Serial.print(millis());//print the return of the function millis to the serial port
    Serial.println(" Error Code=SD_Init_Failed"); //Send the error code to the Serial port
  }
  else//if the SD Card can be loaded
  { //Load States NOTE: this keeps the program from trying to load the Card if there is no SD Card present
    //<Loads stae of int Coal from SD card>
    fileVar = SD.open("COAL.TXT");//Sets File fileVar to SD.open("COAL.TXT")
    if (fileVar) //If the file can be initialized
    { //Loads the variable State
      while (fileVar.available())//While there is something to read from the file
      {
        Coal = Coal + fileVar.read(); //Add the latest charecter from the file to string data
      }
    }
    else //if the file could not be opened
    { //Prints Error Code
      ErrPrint("Cant_Open_COAL.TXT");//Prints the Error code (Quoted Text) and time of Error Code printing to the serial Port and ERRORLOG.TXT
    }
    fileVar.close(); //Close the file
    Serial.print("Coal = ");//Prints the quoted text to the serial port
    Serial.println(Coal);//Prints the value of int coal to the serial port
    //</Loads stae of int Coal from SD card>
    //<Loads stae of int Clock from SD card>
    //NOTE: all the serial communications are used for debugging, I used them to get the code working, and decided not to delete them , incase they are ever needed
    fileVar = SD.open("CLOCK.TXT");//Sets File fileVar to SD.open("CLOCK.TXT")
    if (fileVar) //If the file can be initialized
    { //Loads the variable State
      while (fileVar.available() > 1) //While there is something to read from the file
      { //NOTE: the int variable type isnt large enought to hold the largest possible value of Clock, so the last digit is added outside of the loop.
        Serial.print("fileVar.available() = ");//Prints the quoted text to the serial port
        Serial.println(fileVar.available()); //prints how mant bits stil need to be read
        Clock = (Clock + (fileVar.read() - 48)); //Add the latest charecter from the file to string data
        Serial.print("Clock = ");//Prints the quoted text to the serial port
        Serial.println(Clock);//Prints the value of int Clock to the serial port
        Clock = Clock * 10;
        Serial.print("Clock = ");//Prints the quoted text to the serial port
        Serial.println(Clock);//Prints the value of int Clock to the serial port
      }
      Serial.print("fileVar.available() = "); //Prints the quoted text to the serial port
      Serial.println(fileVar.available()); //prints how mant bits stil need to be read
      Clock = Clock + (fileVar.read() - 48); //Add the latest charecter from the file to string data
      Serial.print("Clock = "); //Prints the quoted text to the serial port
      Serial.println(Clock); //Prints the value of int Clock to the serial port
    }
    else //if the file could not be opened
    { //Prints Error Codes
      Serial.println("Error Code=Cant_Open_CLOCK.TXT"); //Send the error code to the Serial port
    }
    fileVar.close(); //Close the file
    //</Loads stae of int Clock from SD card>
    digitalWrite(UILED, LOW);//Turns off the UILED
  }
}

void loop()
{
  DebugPrint("void loop");// Use the serial Port to print time since startup and Tell debugger that main loop is repeating.
  ClockCount(); //This counts Miliseconds, and decreases Coal by 1 every 1.5 Minutes)
  if ((digitalRead(PwrBttnPin) == HIGH) && !(digitalRead(DmpBttnPin) == HIGH)) //if ((The power button is HIGH) and (the dump button is NOT HIGH))
  { // NOTE: The condition is fales if both buttons are HIGH because that would be ment to engage debug mode.
    PowerDown(); //Saves the state of int Coal and int Clock to the SD Card
    while ((digitalRead(PwrBttnPin) == HIGH) && !(digitalRead(DmpBttnPin) == HIGH)) //while ((The power button is HIGH) and (the dump button is NOT HIGH))
      // NOTE: The condition is fales if both buttons are HIGH because that would be ment to engage debug mode.
    { //this loop wil repeated as long as the condition is true, it keeps the program from detecting the button as being HIGH twice with one long button push
      ClockCount(); // will delay one milisecond and correct the clock as needed
    }
  }
  if ((digitalRead(DmpBttnPin) == HIGH) && !(digitalRead(PwrBttnPin) == HIGH)) //if ((The dump button is HIGH) and (the power button is NOT HIGH))
  { // NOTE: The condition is fales if both buttons are HIGH because that would be ment to engage debug mode.
    Dump(); //Increases int Coal by 4, eventually will play HopperDump.wav sound.
    while ((digitalRead(DmpBttnPin) == HIGH) && !(digitalRead(PwrBttnPin) == HIGH)) //while ((The dump button is HIGH) and (the power button is NOT HIGH))
      // NOTE: The condition is fales if both buttons are HIGH because that would be ment to engage debug mode.
    { //this loop wil repeated as long as the condition is true, it keeps the program from detecting the button as being HIGH twice with one long button push
      ClockCount(); // will delay one milisecond and correct the clock as needed
    }
  }
  if ((digitalRead(DmpBttnPin) == HIGH) && (digitalRead(PwrBttnPin) == HIGH)) //if (the power button is HIGH) and (the dump button is HIGH)
  {
    DebugTrigger();//This Triggers Debug Mode if Power and Dump Buttons are Pressed at the same time for 1 second
    while ((digitalRead(DmpBttnPin) == HIGH) && (digitalRead(PwrBttnPin) == HIGH)) //while ((The power button is HIGH) and (the dump button is  HIGH))
    { //this loop wil repeated as long as the condition is true, it keeps the program from detecting the button as being HIGH twice with one long button push
      ClockCount(); // will delay one milisecond and correct the clock as needed
    }
  }
  GraphLevelSet();//Sets the level of the bar graph to the level of Coal
  ErrorCheck();//Checks variables to make sure they are in peramiters
}

void ClockCount()
{
  DebugPrint("void ClockCount");// Use the serial Port to Tell debugger that ClockCount function hasbeen called.
  delay(1); //this is a 1 milisecond delay, be sure that this only occurs once in loop
  Clock--; // Decrase the clock by one, this counts miliseconds until
  if (Clock <= 0)
  { //resets clock when Time has run out NOTE: there are lines in the code that will take clock below 0 THIS IS NOT AN ERROR!!!
    Clock = SingleBarValue; //Reset Clock
  }
  //Because the debug system cannot print multiple strings in the same line the following 6 lines are needed to print the value of a variable and tag the variable name
  String string = "Coal Level="; //create a string and make it's value "Coal Level="
  string.concat(Coal); //add the ASCII charecters of the value of int Coal to the end of the string
  DebugPrint(string); //print the time since bootup and the string to the debug port
  string = "Clock="; //make the value of variable string "Clock="
  string.concat(Clock); //add the ASCII charecters of the value of int Clock to the end of the string
  DebugPrint(string); //print the time since bootup and the string to the debug port
}

void GraphLevelSet()
{ //This sets the level of the Bar graph
  DebugPrint("void GraphLevelSet"); //Tell us that function has been called via the Serial Port
  byte GphData = 0; //This will be the states for Graph Bars 1-8
  String string; //Used for Debug Print
  for (byte index = 0; index <= 8; index++) //set index to 0 at begining, runs while index is < or = to 8, Changes index by 1 each time
  {
    //Because the debug system cannot print multiple strings in the same line the following 3 lines are needed to print the value of a variable and tag the variable name
    string = "index="; //make the value of variable string "index="
    string.concat(index); //add the ASCII charecters of the value of int index to the end of the string
    DebugPrint(string); //print the time since bootup and the string to the debug port
    if ((Coal - 1) >= index) //Coal-1 is because the index and the bit Write start at 0 and Coal starts at 1
    {
      bitWrite(GphData, index, HIGH); //Change the bit in byte GphData, position "index" to HIGH
    }
    else
    {
      bitWrite(GphData, index, LOW); //Change the bit in byte GphData, position "index" to LOW
    }
  }
  string = "GphData="; //make the value of variable string "GphData="
  string.concat(GphData); //add the ASCII charecters of the value of byte GphData to the end of the string
  DebugPrint(string); //print the time since bootup and the string to the debug port
  shiftOut(GphDataPin, GphClockPin, MSBFIRST, GphData);//Send GphData to the shift Regester.
  //the next two  lines triger the latch pin to tell the shift regester to write the data.
  digitalWrite(GphLatchPin, HIGH);
  digitalWrite(GphLatchPin, LOW);
  //<deal with the 2 pins that are not controled by the shift regester.>
  for (byte index = 0; index <= 1; index++) //set index to 0 at begining, runs while index is < or = to 8, Changes index by 1 each time
  {
    if ((Coal - 9) >= index) //Coal-9 is because the index 0 controls the 9th Line of the Graph
    {
      digitalWrite(GphNineAndTen[index], HIGH); //Send power to Pin "GphNineAndTen[index]"
    }
    else
    {
      digitalWrite(GphNineAndTen[index], LOW); //kill power to Pin "GphNineAndTen[index]"
    }
  }
}

void Dump()
{
  DebugPrint("void Dump"); //Tell us that function has been called via the Serial Port
  if (Coal <= 6) //if there is room for the car to be dumped
  {
    Coal = Coal + 4; //Change Coal by 4
    //Play Hopper_Dump.wav
  }
  else // if there isn't room for the car to be dumped
  {
    Coal = 10; // Set Coal to It's Max Value
    GraphLevelSet();//Sets the Level of the Bar Graph NOTE: this makes the bar graph represent the coal Level Immidiatly, instead of after the LED Blinks
    ErrPrint("Too_Much_Coal");//Prints the Error code (Quoted Text) and time of Error Code printing to the serial Port and ERRORLOG.TXT
    //Save Error Code to Log.txt on SD Card along with time since reset
  }
}

void BlinkUILED()
{ //Blinks the User Interface LED
  int Delay = 500; //The time btween Blinks
  digitalWrite(UILED, HIGH);//Turn On the LED
  delay(Delay);//Wait for Delay Msecs
  digitalWrite(UILED, LOW); //Turn Off LED
  delay(Delay);//Wait for Delay Msecs
  digitalWrite(UILED, HIGH);//Turn On the LED
  delay(Delay);//Wait for Delay Msecs
  digitalWrite(UILED, LOW); //Turn Off LED
  Clock = Clock - (Delay * 3); //Make up for Time Lost
}

void DebugTrigger()
{ //This Triggers Debug Mode if Power and Dump Buttons are Pressed at the same time for 1 second
  DebugPrint("void DebugTrigger()"); //Tell us that function has been called via the Serial Port
  int Delay = 1000; //declare Variable Delay and set it to 1000 - NOTE the delay command uses miliseconds, so take seconds and multiply by 1000
  digitalWrite(UILED, HIGH); //Turn on the User Interface LED
  if (DebugMode == false) //if not in Debug Mode
  {
    Serial.println("Continue holding buttons to enter Debugging Mode.");//Print "Continue holding buttons to enter Debugging Mode." to the serial Port
  }
  else if (DebugMode == true) //if in Debugging Mode - NOTE; else if will only be invoked if above is false
  {
    Serial.println("Continue holding buttons to exit Debugging Mode.");//Print "Continue holding buttons to exit Debugging Mode." to the serial Port
  }
  delay(Delay);//wait for "Delay" msecs
  if ((digitalRead(DmpBttnPin) == HIGH) && (digitalRead(PwrBttnPin) == HIGH)) //if both Power and Dump Buttons are Still HIGH
  {
    if (DebugMode == false) //if not in Debug Mode
    {
      DebugMode = true; //activate Debug Mode
      Serial.println("Debugging Mode Activated");//Tell us that we have activated Debug Mode via the serial Port
    }
    else if (DebugMode == true) //if in Debugging Mode - NOTE; else if will only be invoked if above is false
    {
      DebugMode = false; //deactivate Debug Mode
      Serial.println("Debugging Mode Deactivated");//Tell us that we have deactivated Debug Mode via the serial Port
    }
  }
  if ((digitalRead(DmpBttnPin) != HIGH) || (digitalRead(PwrBttnPin) != HIGH)) //if either Power and Dump Buttons are not Still HIGH
  {
    if (DebugMode == false) //if not in Debug Mode
    {
      Serial.println("Debugging Mode activation Cancled");//Tell us that we have not activated Debug Mode via the serial Port
    }
    else if (DebugMode == true) //if in Debugging Mode - NOTE; else if will only be invoked if above is false
    {
      Serial.println("Debugging Mode Deactivatation Cancelled");//Tell us that we have not deactivated Debug Mode via the serial Port
    }
  }
  digitalWrite(UILED, LOW); //turn of the User interface LED
  Clock = Clock - Delay; //Corect the Clock
}

void DebugPrint(String string)
{
  int Delay = 1000;
  if (DebugMode == true)
  {
    Serial.print(string);//print the info
    delay(Delay);//wait so that the programmer can keep track of what is happening
    Clock = Clock - Delay; //Correct the clock for time lost
  }
}

void ErrorCheck()
{
  //Checks Errors
  //NOTE: None of these should be true, this is an ERROR checker
  if (Coal > 10) //if Coal > 10
  {
    Coal = 10; //Set Coal to 0
    ErrPrint("Coal_>_10");//Prints the Error code (Quoted Text) and time of Error Code printing to the serial Port and ERRORLOG.TXT
  }
  if (Clock > SingleBarValue)
  {
    Clock = SingleBarValue; //Reset Clock
    ErrPrint("Clock_>_SingleBarValue");//Prints the Error code (Quoted Text) and time of Error Code printing to the serial Port and ERRORLOG.TXT
  }
}

void ErrPrint(String ErrCode)
{ //This prints an error code to the serial port and Blinks the UILED
  digitalWrite(UILED, HIGH);
  Serial.print("Millis="); //Print the quoted text to the file
  Serial.print(millis()); //Print the value of millis to the file
  Serial.print("  Error Code="); //print the quoted text to the file
  Serial.println(ErrCode);//print the error code to the file
  if (!SD.begin(ChipSelect))//if unable to load SD card
  { //Print an error Code to the serial port NOTE: I used NOT initilized as the if statement because that is what the program is checking for, it could be reversed, I didn't
    BlinkUILED();//Blink the User interface LED
    Serial.print("millis=");//Print the Quoted text to the serial port
    Serial.print(millis());//print the return of the function millis to the serial port
    Serial.println("  Error Code=SD_Init_Failed"); //Send the error code to the Serial port
  }
  else//if the SD Card can be loaded
  {
    //Next 3 lines print the Value of int Coal
    fileVar = SD.open("ERRORLOG.TXT", FILE_WRITE);//Sets the File fileVar to a command that opens the file for writing
    if (fileVar)//if the file can be opened
    {
      fileVar.print("Millis="); //Print the quoted text to the file
      fileVar.print(millis()); //Print the value of millis to the file
      fileVar.print("  Error Code="); //print the quoted text to the file
      fileVar.println(ErrCode);//print the error code to the file
      fileVar.close();// close the file
    }
    else
    {
      // if the file didn't open, print an error:
      Serial.println("Error Code=Cant_Open_COAL.TXT_Powerdown");//Prints the quoted text to the serial port
    }
    BlinkUILED();
  }
}

void PowerDown()
{ //this saves the value of int Clock and int Coal to the SD card, it also prints them to the serial port
  digitalWrite(UILED, HIGH); //Turns on the Error LED
  DebugPrint("void PowerDown"); //Tell us that function has been called via the Serial Port
  //Next 3 lines print the Value of int Coal
  SD.remove("COAL.TXT");//Delete the old value of int Coal from the txt file
  fileVar = SD.open("COAL.TXT", FILE_WRITE);//Sets the File fileVar to a command that opens the file for writing
  if (fileVar)//if the file can be opened
  {
    Serial.print("Writing to COAL.TXT...");//Prints the quoted text to the serial port
    fileVar.write(Coal);//Write the VALUE of coal to the text file, NOT the the ASCII charecters for it.
    fileVar.close();// close the file
    Serial.println("done.");//Prints the quoted text to the serial port
  }
  else
  {
    // if the file didn't open, print an error:
    ErrPrint("Cant_Open_COAL.TXT_Powerdown");//Prints the Error code (Quoted Text) and time of Error Code printing to the serial Port and ERRORLOG.TXT
  }
  SD.remove("CLOCK.TXT");//Delete the old value of int Clock from the txt file
  fileVar = SD.open("CLOCK.TXT", FILE_WRITE); //Sets the File fileVar to a command that opens the file for writing
  if (fileVar) //if the file can be opened
  {
    Serial.print("Writing to CLOCK.TXT...");//Prints the quoted text to the serial port
    fileVar.print(Clock); //Write the ASCII NUMBERS that MAKE UP the value of the variable to the file
    fileVar.close();// close the file
    Serial.println("done.");//Prints the quoted text to the serial port
  }
  else //If the file couldnt be opened
  {
    Serial.println("Error Code=Cant_Open_CLOCK.TXT_Powerdown");//Prints the quoted text to the serial port
  }

  Serial.println("Power Down Complete");//Prints the quoted text to the serial port
  Serial.print("int Coal = ");//Prints the quoted text to the serial port
  Serial.println(Coal);//Prints the value of int Clock to the serial port
  Serial.print("int Clock = ");//Prints the quoted text to the serial port
  Serial.println(Clock);//Prints the value of int Clock to the serial port
  BlinkUILED(); //Blink the User interface LED
}
