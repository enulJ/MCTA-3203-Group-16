//Define segment
const int segmentA = 4;
const int segmentB = 3;
const int segmentC = 12;
const int segmentD = 10;
const int segmentE = 9;
const int segmentF = 6;
const int segmentG = 8;
 
 
//Define button
const int buttonIncrement = 0;   // Button to increment the count
const int buttonReset = 5;       // Button to reset the count
int count = 0;                    // Current count
 
// Button states
bool lastButtonStateIncrement = LOW;
bool lastButtonStateReset = LOW;
 
void setup() {
// Initialize the digital pins as OUTPUTS and INPUTS
pinMode (segmentA, OUTPUT);
pinMode (segmentB, OUTPUT);
pinMode (segmentC, OUTPUT);
pinMode (segmentD, OUTPUT);
pinMode (segmentE, OUTPUT);
pinMode (segmentF, OUTPUT);
pinMode (segmentG, OUTPUT);
pinMode (buttonIncrement, INPUT);
pinMode (buttonReset, INPUT);
}
 
void loop() {
bool currentButtonStateIncrement = digitalRead (buttonIncrement);
bool currentButtonStateReset = digitalRead (buttonReset);
// Detect button press (LOW means pressed due to pull-up)
 
if (currentButtonStateIncrement == LOW && lastButtonStateIncrement == HIGH)  //---------------------------Increment count
{
count++;
if (count > 9)
{
count = 0;// Wrap around if count exceeds 9
}
delay (200);
}
 
if (currentButtonStateReset == LOW && lastButtonStateReset == HIGH)         //---------------------------Reset Button
{
count = 0; // Reset count to 0
delay (200);
}
 
// Update last button state
lastButtonStateIncrement = currentButtonStateIncrement;
lastButtonStateReset = currentButtonStateReset;
// Display the current count on the 7-segment display
 
displayNumber (count); //Function to display number
}
 
void displayNumber (int count)
{
 
digitalWrite (segmentA, HIGH);
digitalWrite (segmentB, HIGH);
digitalWrite (segmentC, HIGH);
digitalWrite (segmentD, HIGH);  //--------------------------- Reset all segments first
digitalWrite (segmentE, HIGH);
digitalWrite (segmentF, HIGH);
digitalWrite (segmentG, HIGH);
 
switch (count)
 
{
case 0:
digitalWrite (segmentA, LOW);
digitalWrite (segmentB, LOW);
digitalWrite (segmentC, LOW);  //--------------------------- Display 0
digitalWrite (segmentD, LOW);
digitalWrite (segmentE, LOW);
digitalWrite (segmentF, LOW);
break;
 
case 1:
digitalWrite(segmentB, LOW);   //--------------------------- Display 1
digitalWrite(segmentC, LOW);
break;
 
case 2:
digitalWrite (segmentA, LOW);
digitalWrite (segmentB, LOW);
digitalWrite (segmentD, LOW);   //--------------------------- Display 2
digitalWrite (segmentE, LOW);
digitalWrite (segmentG, LOW);
break;
 
case 3:
digitalWrite (segmentA, LOW);
digitalWrite (segmentB, LOW);
digitalWrite (segmentC, LOW);    //--------------------------- Display 3
digitalWrite (segmentD, LOW);
digitalWrite (segmentG, LOW);
break;
 
case 4:
digitalWrite (segmentB, LOW);
digitalWrite (segmentC, LOW);    //--------------------------- Display 4
digitalWrite (segmentF, LOW);
digitalWrite (segmentG, LOW);
break;
 
case 5:
digitalWrite (segmentA, LOW);
digitalWrite (segmentC, LOW);
digitalWrite (segmentD, LOW);    //--------------------------- Display 5
digitalWrite (segmentF, LOW);
digitalWrite (segmentG, LOW);
break;
 
case 6:
digitalWrite (segmentA, LOW);
digitalWrite (segmentC, LOW),
digitalWrite (segmentD, LOW);    //--------------------------- Display 6
digitalWrite (segmentE, LOW);
digitalWrite (segmentF, LOW);
digitalWrite (segmentG, LOW);
break;
 
case 7:
digitalWrite (segmentA, LOW);
digitalWrite (segmentB, LOW);    //--------------------------- Display 7
digitalWrite (segmentC, LOW);
break;
 
case 8:
digitalWrite (segmentA, LOW);
digitalWrite (segmentB, LOW);
digitalWrite (segmentC, LOW);
digitalWrite (segmentD, LOW);    //--------------------------- Display 8
digitalWrite (segmentE, LOW);
digitalWrite (segmentF, LOW);
digitalWrite (segmentG, LOW);
break;
 
case 9:
digitalWrite (segmentA, LOW);
digitalWrite (segmentB, LOW);
digitalWrite (segmentC, LOW);    //--------------------------- Display 9
digitalWrite (segmentD, LOW);
digitalWrite (segmentF, LOW);
digitalWrite (segmentG, LOW);
break;
}
 
}
 
