
#include <EEPROM.h>
int addr; // string write start on memory address point 
String str1;
char str2[35]={0};


#include <PS2Keyboard.h>
const int DataPin = 8;  // D8 pin to D- of usb keyboard
const int IRQpin =  3;  // D3 pin to D+ of usb keyboard
PS2Keyboard keyboard;







// include the library code:
#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to

const int rs = 2, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // twelve.ino rs=10 changed
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);










// AUTOMATIC KEYER USING  USB/PS2 KEYBOARD


////////////////////////////// CW sender/////
String mappings[] = {
                     ".-",  // A
                     "-...",
                     "-.-.",  
                     "-..",
                     ".",  
                     "..-.",
                     "--.",  
                     "....",
                     "..",  
                     ".---",
                     "-.-",  
                     ".-..",
                     "--",  
                     "-.",
                     "---",  
                     ".--.",
                     "--.-",  
                     ".-.",
                     "...",  
                     "-",                      
                     "..-",  
                     "...-",
                     ".--",  
                     "-..-",
                     "-.--",  
                     "--..", // Z
                     ".----",  // 1
                     "..---",
                     "...--",  
                     "....-",
                     ".....",  
                     "-....",
                     "--...",  
                     "---..",
                     "----.",  
                     "-----",   // 0
                     "..--..",  // ?
                     ".-.-.-",  // .
                     "--..--",  // ,
                     "-..-.",   // /
                     "-...-",   // ;   // spacing symbol
                     ".-.-."    // -  //  "AR" symbol
                     };

bool dxing_mode = true;

int size1;

char c;
String  morsecode;
String message;
const String  characters = "abcdefghijklmnopqrstuvwxyz1234567890?.,/;-";
int sm_time;

int TIME_UNIT;
//String inputString = "";             // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

String lcdString = ""; 
String lcd_temp_string = ""; 

String bottom_temp_string = ""; 

int type_expert=6;


int time_step=100;  // initial timer, wpm



////////////////////////////








int KEY_PIN=13; // relay drive to transimit cw in port






// tone frequency for speaker
int notes[] = {
  1047, 1319, 1175, 1568
};


int my_tone_frequency=600; // initial side tone frequency
int tone_pin = 12; // connect speaker 

int sidetone_data=6.00;


int tone_only =0; // for no key but tone actions







void setup() {
// front panel indicator leds
pinMode(A1, OUTPUT);// red led - typing skill 
pinMode(A2, OUTPUT);// green led - typing skill 
pinMode(A3, OUTPUT);// green led - dx mode

pinMode(A6, OUTPUT); // for toggle tone pin - mute




// eeprom read for preset wpm data 

if(EEPROM.read(0)){
  time_step = EEPROM.read(0);
}


// eeprom read for preset tone data 

if(EEPROM.read(1)){
  sidetone_data = EEPROM.read(1);
  my_tone_frequency = sidetone_data * 100;
}


// eeprom read for Dx mode ( 0 for off and 1 for ON ) 

  Serial.println(EEPROM.read(2));
 if(EEPROM.read(2)== 0)
 {
   dxing_mode = false;
   analogWrite(A3, 10); //dx mode led off
 }
  else
  {
    dxing_mode = true;
    analogWrite(A3, 150); // dx mode led on
  }




// eeprom read for preset  L - Type skill modes

if(EEPROM.read(3)){


   type_expert=EEPROM.read(3); 



Serial.println(type_expert);

   
  if(type_expert==1){
analogWrite(A1, 10);
analogWrite(A2, 150); 
   }

  if(type_expert==7){
analogWrite(A1, 150);
analogWrite(A2, 150); 
   }

  if(type_expert==64){
analogWrite(A1, 150);
analogWrite(A2, 10); 
   }




}















  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print(" KEYBOARD KEYER");
  delay(1000);
    // clear the screen
    lcd.clear();



  
  keyboard.begin(DataPin, IRQpin);
  Serial.begin(9600);
  Serial.println("Keyboard Test:");







        pinMode(13, OUTPUT);  // connect a buzzer at pin 13 to listen cw
       
    
        // reserve 200 bytes for the lcdString:
       lcdString.reserve(1500);
      lcd_temp_string.reserve(1500);
       message.reserve(1500);
       bottom_temp_string.reserve(1500);

  pinMode(12, OUTPUT); // connect speaker to get tone
  digitalWrite(13, LOW);          // Start with key up
  
pinMode(10, OUTPUT);

digitalWrite(10, LOW); // power the keyboard

 
  // sound welcome tone
  welcome_tone();

// display wpm on screen
 showwpm();
  digitalWrite(10, HIGH); // POWERING USB KEYBOARD VIA D10 PIN
 }


void loop() {





////////-------------------

if(lcdString.length()>=type_expert){
 message = lcdString;

    // clear the screen
    lcd.clear();

lcdString=""; // clearing last string from keyboare entry
int result= messagetocw();

lcd_bottom_display();
  
}

///////--------------------


  if (keyboard.available()) {
    
        // read the next key
        char c = keyboard.read();
    
        // check for some of the special keys
        if (c == PS2_ENTER) {
          // Serial.println();
       message = lcdString;

    // clear the screen
    lcd.clear();

lcdString=""; // clearing last string from keyboare entry
int result= messagetocw();

lcd_bottom_display();

          
        }
        
        
        
        else if (c == PS2_TAB) {
         // Serial.print("[Tab]");

           lcdString += "cq cq cq de vu3otn vu3otn vu3otn k";
           lcd_bottom_display();


          
        } else if (c == PS2_ESC) {
        //  Serial.print("[escape]");
// toggle between pin 12 and A6
// 12 is correct tone pin, switch to A6 which is not connected to any speaker 
if(tone_pin==A6)
{
tone_pin=12;
 tone_up();

}
else
{
tone_down();

tone_pin=A6;
}

          
        } else if (c == PS2_PAGEDOWN) {
          
          
           lcdString += "tnx fer qso  73 tu ee";
           lcd_bottom_display();



      
          
        } else if (c == PS2_PAGEUP) {
          //Serial.print("[PgUp]");

           lcdString += "qrz ? de vu3otn k";
           lcd_bottom_display();




      
          
        } else if (c == PS2_BACKSPACE) {

         
 /// ------////   
size1 = lcdString.length();
size1=size1-1;
//Serial.print("---");
lcdString.remove(size1, 1);
//Serial.print(lcdString);



lcd_bottom_display();
///------////




        } else if (c == PS2_LEFTARROW) {
        //  Serial.print("[Left]");
        
// read string data from memory address 5        
addr=5;        
readEEPROM(addr);
delay(100);
lcdString += str2;
lcd_bottom_display();


        
        
        
        } else if (c == PS2_RIGHTARROW) {
        //  Serial.print("[Right]");

        
// read string data from memory address 145        
addr=145;        
readEEPROM(addr);
delay(100);
lcdString += str2;
lcd_bottom_display();

         
        
        } else if (c == PS2_DELETE) {
       


          
          
        } else if (c == PS2_UPARROW) {
         // Serial.print("[Up]- Speed UP");
         // speedup();
// read string data from memory address 75       
addr=75;        
readEEPROM(addr);
delay(100);
lcdString += str2;
lcd_bottom_display();
         
          
        } else if (c == PS2_DOWNARROW) {
        //  Serial.print("[Down]- Speed DOWN");
       

           lcdString += "vu3otn";
           lcd_bottom_display();
          
          
        } else if (c =='Z') {
          my_tone_frequency = 523;
          message="t";
          tone_only=1;
         int result= messagetocw();
         tone_only=0;
         eeprom_tone_entry();
         


        } else if (c == 'X') {

          my_tone_frequency = 587;
          message="t";
           tone_only=1;
         int result= messagetocw();
          tone_only=0;
         eeprom_tone_entry();

        } else if (c == 'C') {
          my_tone_frequency = 659;
          message="t";
          
         tone_only=1;
         int result= messagetocw();
         tone_only=0;
         
         eeprom_tone_entry();


              } else if (c == 'V') {
                
          my_tone_frequency = 698;
          message="t";
          
         tone_only=1;
         int result= messagetocw();
         tone_only=0;
         
         eeprom_tone_entry();


              } else if (c == 'B') {
                
          my_tone_frequency = 784;
          message="t";
          
         tone_only=1;
         int result= messagetocw();
         tone_only=0;
         
         eeprom_tone_entry();

              } else if (c == 'N') {
                
          my_tone_frequency = 880;
          message="t";
          
         tone_only=1;
         int result= messagetocw();
         tone_only=0;
         
         eeprom_tone_entry();
         
                       } else if (c == 'M') {
                
          my_tone_frequency = 988;
          message="t";
          
         tone_only=1;
         int result= messagetocw();
         tone_only=0;
         
         eeprom_tone_entry();


                      } else if (c == '<') {
                
          speeddown();


                     } else if (c == '>') {
                
          speedup();

        


                      } else if (c == 'H') {
                
         


           lcdString += "hi";
           lcd_bottom_display();



         

//other actions
              } else if (c == 'W') {

          toneup();

         

              

                    } else if (c == 'S') {
                      
           tonedown();



                    } else if (c == '`') {
                      
        
           lcdString +="5nn";
           lcd_bottom_display();
    


                    } else if (c == '~') {
                      
           lcdString +="599";
           lcd_bottom_display();



//////////////skill change by J K L  ///////&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 } else if (c == 'J') {   
  type_expert=64; 

       // EEPROM.write(memmory_locaton, data-to-store);
      EEPROM.write(3, type_expert); // storing eeprom 3 with value 64
            
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("J        [J-K-L]");
  lcd.setCursor(0, 1);
  lcd.print("Type Skill: LOW ");
  tone_up();
  
analogWrite(A1, 150);
analogWrite(A2, 10);  
 

  


 } else if (c == 'K') { 
  type_expert=7; 

       // EEPROM.write(memmory_locaton, data-to-store);
      EEPROM.write(3, type_expert); // storing eeprom 3 with value 7


               
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("K        [J-K-L]");
  lcd.setCursor(0, 1);
  lcd.print("Type Skill: GOOD");
  tone_up();


analogWrite(A1, 150);
analogWrite(A2, 150); 
  
 

 } else if (c == 'L') { 
  
  type_expert=1;   

       // EEPROM.write(memmory_locaton, data-to-store);
      EEPROM.write(3, type_expert); // storing eeprom 3 with value 1
             
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("L        [J-K-L]");
  lcd.setCursor(0, 1);
  lcd.print("Type Skill: BEST");
  tone_up();

analogWrite(A1, 10);
analogWrite(A2, 150); 



//////////////skill change by J K L  ///////

         


        } else if (c =='G') {
         
//dx mode inter chanage
if(dxing_mode){
   dxing_mode = false;
   
    lcd.clear();
    // set the cursor position:
    lcd.setCursor(3, 1);
    // display each character to the LCD
    lcd.print("Dx mode : OFF");

tone_down();

analogWrite(A3, 10); 

       // EEPROM.write(memmory_locaton, data-to-store);
      EEPROM.write(2, 0); // storing eeprom 2 with value 0
    
    
}else{
   dxing_mode = true;

    lcd.clear();
    // set the cursor position:
    lcd.setCursor(3, 1);
    // display each character to the LCD
    lcd.print("Dx mode : ON ");
   

tone_up();

analogWrite(A3, 150);


       // EEPROM.write(memmory_locaton, data-to-store);
      EEPROM.write(2, 1); // storing eeprom 2 with value 1



}//dx mode inter chanage





                    } else if (c == '!') {

// shift + 1 will add memory here at memory address 5
addr=5;
str1=lcdString;
writEEPROM(addr);
// dialog for user
    lcd.setCursor(0, 0);
    lcd.print("Rec1  Left-arrow");


              

                          } else if (c == '@') {
                            
 // shift + 2 will add memory here at memory address 75
addr=75;
str1=lcdString;
writEEPROM(addr);
// dialog for user
    lcd.setCursor(0, 0);
    lcd.print("Rec2    Up-arrow");

  

                          } else if (c == '#') {
 
 // shift + 2 will add memory here at memory address 145
addr=145;
str1=lcdString;
writEEPROM(addr);
// dialog for user
    lcd.setCursor(0, 0);
    lcd.print("Rec3 Right-arrow");


                                   } else if (c == '$') {
 
                        time_step = 85;
                        showwpm();
  

                                            } else if (c == '%') {
 
                        time_step = 75;

                        showwpm();



                                                     } else if (c == '^') {
 
                        time_step = 65;
                        showwpm();



         
                                                     } else if (c == '&') {
 
                        time_step = 55;
                        showwpm();


                                                              } else if (c == '*') {
 
                        time_step = 45;
                        showwpm();
 

                                                                       } else if (c == '(') {
 
                        time_step = 30;
                        showwpm();
 

 








        } else {
          
          // otherwise, just print all normal characters
        //  Serial.print(c);


///Launching
//message=c;




        
Serial.print(c);

        lcdString +=c;
Serial.print(" ::: ");
Serial.print(lcdString);
Serial.print(" ---- ");

lcd_bottom_display();
/////////////////////////////////////////////////
















    
  
//delay(100);

/////////////////////////////

  //   int result= messagetocw(); //  jusst for testing

///Launching








          
        }
 
  }// 'if' is close


 
}// LOOP() close



// -----lcd bottom layer display start-----//

void lcd_bottom_display(){



      if(lcdString.length()<=15)
      {
    // clear the screen
    lcd.clear();
    // set the cursor position:
    lcd.setCursor(0, 1);
    // display each character to the LCD
    lcd.print(lcdString);
    lcd.cursor();
      }
       else
       {
lcd_temp_string ="";        
lcd_temp_string = lcdString;
lcd_temp_string.remove(0, lcdString.length()-15); 
lcd.clear();
lcd.setCursor(0, 1);
lcd.print(lcd_temp_string);
lcd.cursor(); 
       }



  
}// lcd bottom layer display //





void eeprom_tone_entry(){
        // putting in eeprom - new tone 
     // EEPROM.write(memmory_locaton, data-to-store);
      EEPROM.write(1, (my_tone_frequency / 100));
}





void toneup()
{
my_tone_frequency = my_tone_frequency + 50;
          message="t";
         tone_only=1;
         int result= messagetocw();
         tone_only=0;
         
eeprom_tone_entry();


         
  
}

void tonedown()
{
my_tone_frequency = my_tone_frequency - 50;
          message="t";
          
          tone_only=1;
         int result= messagetocw();
         tone_only=0;

 eeprom_tone_entry();
  
}



void showwpm(){


//if(time_step <=50 && time_step >= 40){

 // wpm=33;
//}

//if(time_step ==60){
 //  wpm=28;
//}

//if(time_step ==70){
 //  wpm=26;
//}

//if(time_step ==80){
  // wpm=24;
//}
//if(time_step ==90){
 //  wpm=21;
//}
//if(time_step ==100){
 //  wpm=20;
//}
//if(time_step ==110){
//   wpm=19;
//}
//if(time_step ==120){
 //  wpm=15;
//}
//if(time_step ==130){
//   wpm=14;
//}

    //  lcd.clear();
     // lcd.setCursor(0, 0);
     //  lcd.print("Speed :");
    //  lcd.print(wpm);
     // lcd.print(" wpm   ");

      // putting in eeprom - new wpm
     // EEPROM.write(memmory_locaton, data-to-store);
      EEPROM.write(0, time_step);
      
       // give a sound prefiew of wpm
       message="ok";

       
         tone_only=1;
         int result= messagetocw();
         tone_only=0;


      
      
  //////////////////////////////////////////////////////////////////////////////
}


void speedup()
{
  time_step=time_step-10;






        if(time_step<=10)
  {
            time_step=10;
  }


showwpm();
 

 
}




void speeddown()
{
  time_step=time_step+10;


showwpm();

}



void  speedup_mute(){
time_step=time_step-10;
        if(time_step<=10)
  {
            time_step=10;
  }
  
 }










int messagetocw()
{  

lcd_bottom_display();

  // FUNCTION messagetocw() START


 
  // int size = strlen(message);
  int size = message.length();
  int TIME_UNIT = 50; // default time for DIT
  ///
  if(TIME_UNIT==time_step)
  {
    
  }
  else
  {
        TIME_UNIT=time_step;
  }

  if(TIME_UNIT<=10)
  {
        TIME_UNIT=10;
        time_step=10;
  }

 
  const int DOT = TIME_UNIT;
  const int DASH = 3 * TIME_UNIT;
int ss;
int ls;
int ws;
if(dxing_mode)
{
 
ss=DOT;
ls=0.5 * DOT;
ws=2 * DOT;
}
else
{

 ss=DOT;
ls=2 * DOT;
ws=5 * DOT;
}
 
  const int SYMBOL_SPACE = ss;
  const int LETTER_SPACE = ls;
  const int WORD_SPACE = ws;




  


  
 
  message.toLowerCase();
  for (int i = 0; i < size; i++) {
    
        // const char* ch = strchr(characters, tolower(message[i]));
        char ch = characters.charAt(characters.indexOf(message.charAt(i)));//
        // if (ch != NULL){
   if (ch != '\0'){
          
          // int position = ch-characters;
          int position = characters.indexOf(message.charAt(i));
          
          morsecode = mappings[position];
          
          // int count = strlen(morsecode);
         int count = morsecode.length();
          







          
          
          for( int j=0; j< count; j++)
          {
            
            char symbol = morsecode[j];
            
            if(symbol =='.')
            {
              sm_time = DOT;
            }
            
            if(symbol =='-')
            {
              sm_time = DASH;
            }

            if(tone_only == 0){
            digitalWrite(KEY_PIN, HIGH);
                                }

         tone(tone_pin, my_tone_frequency);
        




            
            delay(sm_time);
           if(tone_only == 0){
            digitalWrite(KEY_PIN, LOW);
                                }
            
             noTone(tone_pin);
            delay(SYMBOL_SPACE);





////////////////////////////////////////
  if (keyboard.available()) {
    
        // read the next key
        char c = keyboard.read();


 if (c == PS2_ENTER) {

//message =lcdString;

    // clear the screen
  //  lcd.clear();

//lcdString=""; // clearing last string from keyboare entry
//int result= messagetocw();
//lcd_bottom_display();


        
  } else if (c == PS2_BACKSPACE) {

// if (c == PS2_BACKSPACE) {
 
size1 = lcdString.length();
size1=size1-1;
Serial.print("########");
lcdString.remove(size1, 1);

}else{

        lcdString +=c;

      }

       // lcd_top_instant_display();
        lcd_bottom_display();  
  }
////////////////////////////////////////////



//// bottom lcd live scroll/////
if(message.length()<=15){
  bottom_temp_string = message; 

      lcd.setCursor(0, 0);
    lcd.print(bottom_temp_string);
    bottom_temp_string ="";
}else{

  bottom_temp_string = message; 
  bottom_temp_string.remove(0, (bottom_temp_string.length()-15));
      lcd.setCursor(0, 0);
      
      lcd.print(bottom_temp_string);
      bottom_temp_string ="";
     
}

//// bottom lcd live scroll/////











            
          }// for loop close2

          




          
          delay(LETTER_SPACE);
          
        }













        
        delay(WORD_SPACE);
    
  }// for loop close1


   
 
} // FUNCTION messagetocw() close





void tone_up(){

         tone(tone_pin, 1000);
         delay(200);
         tone(tone_pin, 1173);
         delay(100);
         noTone(tone_pin);
}

void tone_down(){
  
         tone(tone_pin, 1173);
         delay(200);
         tone(tone_pin, 1000);
         delay(100);
         noTone(tone_pin);
  
}




void welcome_tone(){
  

  for (int thisSensor = 0; thisSensor < 4; thisSensor++) {
         tone(12, notes[thisSensor]);
         delay(time_step);
         noTone(12);
  }
  
  for (int thisSensor = 3; thisSensor >= 0; thisSensor--) {
         tone(12, notes[thisSensor]);
         delay(time_step);
         noTone(12);
  }

  
}









//////////////////////////////////////////////////////
void writEEPROM(int addr)
{
int _size = str1.length();
Serial.println("\n");
Serial.println(_size);

EEPROM.write(addr, _size); // put size of string at starting 'addr'

int i = 0;
for(i = 0; ( str1[i] != '\0' && i < _size  && addr < EEPROM.length() ); i++)
  {
EEPROM.put(addr + 1 + i, str1[i]);
  }

EEPROM.put(addr + 1 + i, '\0');
}


void readEEPROM(int addr)
{

int k;
k=EEPROM.read(addr);
  
Serial.println("value of k \n");
Serial.println(k);

int i = 0;
for( i=0; (i< k && addr + 1 < EEPROM.length()); i++ )

   {
      EEPROM.get(addr + 1 + i, str2[i]);
   }
   
       str2[i] = '\0';

}
////////////////////////////////////////////////////////
