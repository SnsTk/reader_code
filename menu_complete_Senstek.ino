/*


This code is intended to be a menu interface for the Senstek Solid State Reader
which utilizes the Adafruit nRF52840 Feather, as well as the corresponding Adafruit FeatherWing OLED display.

This code creates a scrollable menu, which can be navigated through using the 3 buttons on the OLED display
The top and bottom buttons are used as up and down buttons, while the center button is used as a select button.

Currently the A2 pin is used as an output pin to drive an LED
and the A5 pin is used as an analog input. 

----------------------------------------------------------------------------------------------------------------------------------------------------------
Functions: 
    Sample Setting:
        This option allows uers to adjust how many times the nrf5280 should take a sample
        before taking the average of all samples. 
        
        For example: By setting this value to 30, the nrf5280 will take a sample 30 times,
        sum all 30 samples, and divide that sum by 30 to get the average of all samples.
        It will then use that average value to the calculate the corresponding pH value.

    
    Alarm Setting:
        This option allows users to be notified when the reader senses a pH level below and/or above a certain threshold. 
        
        
        For example: By setting the upper limit at pH 12.0 and the lower limit at 3.0, the code will toggle the 
        A2 pin High and Low 3 times, and then keep it High. 
        This pin can be connected to an auditory or visual element (buzzer, LED, etc)
        to alert the user that the pH level reached the threshold value.

        
    Sample Rate:
        This option allows uers to adjust the time interval between each reading. 
        
        For example: By setting this value to 30, the reader will take samples every 30 seconds. 


    Go:
        This starts the actual sampling. 
        
        This is what the display looks like
        ######################################
        #                                    #
        #  Timer                Sample Count #
        #  --------------------------------  #
        #             pH Value               #
        #                                    #
        ######################################

        User can do two things in this part of the menu:
        
           1.  Hold down middle button (select button): 
                    This allows user to go back to the main menu, and also resets all the settings(Sample Size, Alarm Bounds, Sample Rate)

           2.  Hold down the top and bottom buttoms (up and down buttons):
                    This allows user to view a separate screen which shows all of the setting values chosen (Sample Size, Alarm Bounds, Sample Rate)
                    It also tells user the millivolt input and the corresponding pH value.



----------------------------------------------------------------------------------------------------------------------------------------------------------
              
        

Created 29 September 2020
by Akira Comia

*/



#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);


#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5



int bounce = 0; // cursor, bounces from 0 to 1 repeatedly
int pos = 8; // y position variable used for cursor, increases and decreases by increments of 8
           
int debounce = 1; //debouncing variable, used for button b to make sure button is debounced


//change this part depending on sensors
  float slope = -42.0;
  float yint = 308.9;




int scroller = 0; // variable used to verify that cursor has scrolled down and up


//variables that can change thru menu interface
  int sampleRate = 5;// variable for sample Rate ( how often to take a sample)
  float phValHigh = 14.0; // upper bound for ph alarm
  float phValLow = 0; // lower bound for ph alarm
  int numReadings = 20;



float debugtest = 0; // debugging variable used to print out millivolts when both "A" and "C" button r held down





void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();



  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(A2, OUTPUT);


}






void loop() {
  delay(100);
  digitalWrite(A2, LOW);



//to acheive scrolling effect, variable "scroller" was created. The OLED display will change based on the value of Scroller. 
//scroller = 0 gives you regular menu, 
//scroller = 1 means you scrolled down, and it gives you the more options 
// as you increase menu options, simply add more "else if scroller ==x" statements

  if (scroller == 0) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.println("Menu:");

    display.println("    1: Sample Setting");
    display.println("    2: Alarm Setting");
    display.println("    3: Sample Rate");
    display.display();
  }


  
  else if (scroller == 1) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.println("    1: Sample Setting"); 
    display.println("    2: Alarm Setting"); 
    display.println("    3: Sample Rate");
    display.println("    ++  GO ++");
    display.display();

  }

  
  //displays cursor at position "pos"
  display.setCursor(12, pos); // 12 is the x distance, and pos is the y distance from the top left corner. 
  display.print(bounce); // bounce is interger that will only ever be 1 or 0, and this acts as our cursor
  display.display();

  //cursor
  if (bounce == 0) {
    bounce = 1;
  }
  else if (bounce == 1) {
    bounce = 0;
  }

  if (!digitalRead(BUTTON_A)) // up button
  {
    pos = pos - 8; // change y coordinate of cursor up 1 line. 1 line = 8 vertical units.

    if (pos < 8) { // sets bound for cursor
      pos = 8;
      if (scroller > 0) {
        scroller = scroller - 1;
        if (scroller < 0) {
          scroller = 0;
        }
      }

    }
  }

  
  if (!digitalRead(BUTTON_C)) // down button
  {
    pos = pos + 8; // sets bound for cursor
    if (pos > 24) {
      scroller = scroller + 1;
      if (scroller > 1) { // instead of 1 change to variable as you increase menu options so its easier to changein code
        scroller = 1;

      }
      pos = 24;
    }
  }
  if (!digitalRead(BUTTON_B)) //selector button
  {

    if (scroller != 0) {
      pos = pos + 8 * scroller;
    }
    switch (pos) {



      case 8: {


          int breaker1 = 0;

          //delay(1000);
          while (breaker1 == 0) {
            delay(100);
            display.clearDisplay();
            display.setCursor(0, 0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.println("Samples per average:  ");
            display.setTextSize(2);
            display.print("   ");
            display.println(numReadings);
            display.display();
            //debounce the select button

            if (digitalRead(BUTTON_B) == HIGH)
            {
              debounce = 5;
            }

            if (!digitalRead(BUTTON_A)) // up button
            {
              numReadings = numReadings + 5;
              if (numReadings > 100) {
                numReadings = 100;

              }
            }
            if (!digitalRead(BUTTON_C)) // down button
            {
              numReadings = numReadings - 5;
              if (numReadings < 10) {
                numReadings = 10;

              }
            }

            if ((digitalRead(BUTTON_B) == LOW) && (debounce == 5)) // down button
            {
              debounce = 1; //when clicking button b, set debounce to 1
              display.clearDisplay();
              display.setCursor(0, 0);
              display.setTextSize(1);
              display.println("Sample size set!");
              display.display();
              delay(1000);
              breaker1 = 1;

            }
          }
        } break;

      case 16:
        {
          int breaker2 = 0;
          int pos2 = 16; // position of cursor
          //float phValAlarm=7.0;
          //delay(1000);
          while (breaker2 == 0) {
            delay(100);
            display.clearDisplay();
            display.setCursor(0, 0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.println("Select limit:");
            display.println("");
            //display.println();
            display.println("    1: Lower limit");
            display.println("    2: Upper limit");

            display.display();
            //debounce the select button

            display.setCursor(12, pos2);
            display.print(bounce);
            display.display();

            //cursor
            if (bounce == 0) {
              bounce = 1;
              
            }
            else if (bounce == 1) {
              bounce = 0;
             
            }
            if (digitalRead(BUTTON_B) == HIGH)
            {
              debounce = 5;
            }



            if (!digitalRead(BUTTON_A)) // up button
            {
              pos2 = pos2 - 8;

              if (pos2 < 16) {
                pos2 = 16;
              }
            }
            if (!digitalRead(BUTTON_C))
            {
              pos2 = pos2 + 8;
              if (pos2 > 24) {
                pos2 = 24;
              }
            }
            if ((digitalRead(BUTTON_B) == LOW) && (debounce == 5) && (pos2 == 24)) // Upper limit
            {
              int breaker5 = 0;
              debounce = 1;


              while (breaker5 == 0) {
                if (digitalRead(BUTTON_B) == HIGH)
                {
                  debounce = 5;
                }
                delay(100);
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextSize(1);
                display.println("    Set Alarm if:");
                display.println("---------------------");
                display.setTextSize(2);
                display.print("pH > ");
                display.println(phValHigh);

                display.display();

                if (!digitalRead(BUTTON_A)) // up button
                {
                  phValHigh = phValHigh + 0.5;
                  if (phValHigh > 14) {
                    phValHigh = 14;

                  }


                }
                if (!digitalRead(BUTTON_C)) // down button
                {
                  phValHigh = phValHigh - 0.5;
                  if (phValHigh < 0) {
                    phValHigh = 0;

                  }
                }
                if ((digitalRead(BUTTON_B) == LOW) && (debounce == 5)) // down button
                {
                  debounce = 1;
                  display.clearDisplay();
                  display.setCursor(0, 0);

                  display.println("Alarm set!");
                  display.display();
                  delay(1000);
                  breaker5 = 1;
                  breaker2 = 1;

                }

              } break;



            }
            else if ((digitalRead(BUTTON_B) == LOW) && (debounce == 5) && (pos2 == 16)) // Lower limit
            {
              int breaker5 = 0;
              debounce = 1;


              while (breaker5 == 0) {
                if (digitalRead(BUTTON_B) == HIGH)
                {
                  debounce = 5;
                }
                delay(100);
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextSize(1);
                display.println("    Set Alarm if:");
                display.println("---------------------");
                display.setTextSize(2);
                display.print("pH < ");
                display.println(phValLow);

                display.display();

                if (!digitalRead(BUTTON_A)) // up button
                {
                  phValLow = phValLow + 0.5;
                  if (phValLow > 14) {
                    phValLow = 14;

                  }


                }
                if (!digitalRead(BUTTON_C)) // down button
                {
                  phValLow = phValLow - 0.5;
                  if (phValLow < 0) {
                    phValLow = 0;

                  }
                }
                if ((digitalRead(BUTTON_B) == LOW) && (debounce == 5)) // down button
                {
                  debounce = 1;
                  display.clearDisplay();
                  display.setCursor(0, 0);

                  display.println("Alarm set!");
                  display.display();
                  delay(1000);
                  breaker5 = 1;
                  breaker2 = 1;

                }

              } break;



            }
          }
        } break;

      case 24: //this is for the sample Rate
        {
          int breaker4 = 0;
          //int sampleRate=5;
          //delay(1000);
          while (breaker4 == 0) {
            delay(100);
            display.clearDisplay();
            display.setCursor(0, 0);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.println("Take Sample Every:  ");
            display.println();
            display.setTextSize(2);
            display.print("   ");
            display.print(sampleRate);
            display.println("s");
            display.display();
            //debounce the select button


            if (digitalRead(BUTTON_B) == HIGH)
            {
              debounce = 5;
            }

            if (!digitalRead(BUTTON_A)) // up button
            {
              sampleRate = sampleRate + 5;
              if (sampleRate > 100) {
                sampleRate = 100;

              }
            }
            if (!digitalRead(BUTTON_C)) // down button
            {
              sampleRate = sampleRate - 5;
              if (sampleRate < 5) {
                sampleRate = 5;

              }
            }

            if ((digitalRead(BUTTON_B) == LOW) && (debounce == 5)) // down button
            {
              debounce = 1;
              display.clearDisplay();
              display.setCursor(0, 0);
              display.setTextSize(1);
              display.println("Sample Rate set!");
              display.display();
              delay(1000);

              breaker4 = 1;
            }
          }

        } break;

      
     
      case 32:
        {

          int breaker3 = 0;
          int counter = 1;
          int minutes = 0;
          int seconds = 0;
          while (breaker3 == 0) {
            float sum = 0.0;

            for (int i = 0; i < numReadings; i++) {
              int Value = analogRead(A5);
              int ampVolt = map(Value, 0, 1023, 0, 3600); // translates that value into a number between 0~3600
              float Millivolt = ((ampVolt - 2000) / 3) - 18; // converts that number back into the actual millivolt reading of sensor
              // added -18 bc there seems to be an offset from afe board?
              float phVal = (Millivolt - yint) / (slope); //converts millivolts into ph //

              sum = sum + phVal;
              debugtest = Millivolt;
            }
            float average = sum / numReadings;
            //  should produce the average taken after [numReading] samples
            if (average > phValHigh || average < phValLow) {
              display.clearDisplay();
              display.setCursor(0, 0);
              display.setTextSize(1);
              display.println("Alarm has tripped!");
              display.display();
              digitalWrite(A2, HIGH);
              delay(400);
              digitalWrite(A2, LOW);
              delay(400);
              digitalWrite(A2, HIGH);
              delay(400);
              digitalWrite(A2, LOW);
              delay(400);
              digitalWrite(A2, HIGH);
              delay(400);
              digitalWrite(A2, LOW);
              delay(400);
              digitalWrite(A2, HIGH);

            }

            for (int j = 0; j <= sampleRate; j++) {

              if (digitalRead(BUTTON_B) == HIGH)
              {
                debounce = 5;
              }
              minutes = j / 60;
              seconds = j % 60;

              display.clearDisplay();
              display.setCursor(0, 0);
              display.setTextSize(1);
              // display.println("");
              //display.print("   ");
              display.print(minutes);
              display.print(":");
              
              if (seconds < 10) {
                display.print("0"); 
              }
              display.print(seconds);
              display.print("        sample:");
              display.println(counter);
              //display.println("");
              //display.println("");
              //display.println("+++++++++++++++++++++");
              display.println("---------------------");
              
           


              display.setTextSize(2);
              display.print(" pH");
              display.print(": ");
              display.print(average);
              display.display();


              //press and hold button C and A to see millivolt and ph value
              //diagnosis page
              //shows you mV reading, pH reading, ph alarm bounds, sample rate and sample size 
              
              if ((digitalRead(BUTTON_C) == LOW) && (digitalRead(BUTTON_A) == LOW))
              {
                // for debugging purposes, works
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextSize(1);


                display.print("mV:");
                display.print(debugtest);

                display.print("   pH:");
                display.println(average);

                display.print(phValLow);
                display.print("< pH <");
                display.println(phValHigh);

                display.print("Sample every: ");
                display.print(sampleRate);
                display.println("s");
                display.print("Samples/average: ");
                display.println(numReadings);
                display.display();
                delay(2000);
              }
              //end of diagnosis 
              
              if ((digitalRead(BUTTON_B) == LOW) && (debounce == 5)) // down button
              {
                debounce = 1; // quit sampling

                breaker3 = 1;
                display.clearDisplay();
                display.setCursor(0, 0);
                display.setTextSize(1);
                display.print(" Sampling has stopped");
                display.display();
                delay(1000);
                break;
              }
              delay(1000);

            }
            counter = counter + 1;

            //break;

            //quit sampling
            if (!digitalRead(BUTTON_B)) // quit sampling
            {
              breaker3 = 1;
              display.clearDisplay();
              display.setCursor(0, 0);
              display.setTextSize(1);
              display.print(" Sampling has stopped");
              display.display();
              delay(1000);
            }




          }
        }
        // case 32 end

    }



  }
}
