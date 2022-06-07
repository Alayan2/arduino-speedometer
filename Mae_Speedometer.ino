


#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 8, 7, 6, 5);

// Circumference of bicycle wheel expressed in meters
float bicycleWheelCircumference = 50.2655;  

const int pauseButton = 2;
boolean lastPauseButton = LOW;
boolean currentPauseButton = LOW;

const int displayModeButton = 3;
boolean lastDisplayModeButton = LOW;
boolean currentDisplayModeButton = LOW;

const int revolutionButton = 4;
boolean lastRevolutionButton = LOW;
boolean currentRevolutionButton = LOW;

boolean greeting = HIGH;

boolean paused = LOW;
boolean pausedShown = LOW;
unsigned long pausedStartTime = 0;

boolean wheelTurningShown = LOW;
unsigned long wheelTurningStartTime = 0;

boolean motivatorShown = LOW;
unsigned long motivatorStartTime = 0;

unsigned long lastRevolutionStartTime = 0;
unsigned long revolutionTime = 0;

int currentDisplayMode = 0;
int showLap = 0;
int lapCurrentlyShown = 100;
int currentLap = 0;

float currentDistance;
unsigned long currentDuration;
int currentMaximumMPH;
int currentAverageMPH;
int currentMPH;

float arrayDistance[100];
unsigned long arrayDuration[100];
int arrayMaximumMPH[100];
int arrayAverageMPH[100];

unsigned long revolutionCount = 0;
unsigned long currentTime = 0;
unsigned long lapStartTime = 0;

float mi = 0.00;
float mph = 0.00;
int intHours;
int intMinutes;
int intSeconds;

unsigned long milliSecondsInSecond = 1000;
unsigned long milliSecondsInMinute = 60000;
unsigned long milliSecondsInHour = 3600000;

void setup()
{

  // Configure digital input pins for push buttons and Hall sensor
  pinMode (revolutionButton, INPUT);
  pinMode (pauseButton, INPUT);
  pinMode (displayModeButton, INPUT);

  // Initialize maximum MPH in totals as this may not be calculated if no maximum was computed for laps
  // and there may be random data in memory location
  arrayMaximumMPH[0] = 0;

  // Initialize LCD screen & show "PRESS BUTTON TO START"
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("HI LADY MAE");
  lcd.setCursor(4, 1);
  lcd.print("PRESS BUTTON TO START");
  
}

void loop() {

  // Get current time in millis
  currentTime = millis();

  // Read revolution Hall sensor
  currentRevolutionButton = debounce(lastRevolutionButton, revolutionButton);
  if (lastRevolutionButton == HIGH && currentRevolutionButton == LOW) {
    
    // If initial "PRESS BUTTON TO START" is not displayed and not currently paused then this 
    // status is not at start up.
    if (!greeting && !paused) {

      // Increase wheel revolution count
      revolutionCount++;

      // Display "+" to show that one revolution was recorded
      lcd.setCursor(0, 0);
      lcd.print("+");
      wheelTurningShown = HIGH;
      wheelTurningStartTime = currentTime;

      // Compute millis it took for this latest revolution
      if (lastRevolutionStartTime > 0) {

        revolutionTime = currentTime - lastRevolutionStartTime;

        // Compute current speed in miles per hour based on time it took to complete last wheel revolution
        mph = (3600000 / revolutionTime) * bicycleWheelCircumference / 63360;
        currentMPH = mph;

        // If current speed is new maximum speed for this lap then store it
        if (currentMaximumMPH < currentMPH) {
          currentMaximumMPH = currentMPH;
        }
      }
      lastRevolutionStartTime = currentTime;
    }
  }
  lastRevolutionButton = currentRevolutionButton;

    // Read PAUSE/RESUME push button
  currentPauseButton = debounce(lastPauseButton, pauseButton);
  if (lastPauseButton == LOW && currentPauseButton == HIGH) {

    // If "PRESS BUTTON TO START" message has been showing then we now need to start 1st lap/period
    if (greeting) {

      greeting = LOW;  

      // Show motivational message
      showMotivator();
      motivatorShown = HIGH;
      motivatorStartTime = currentTime;

      currentLap = 1;
      resetLapVariables();
      currentDisplayMode = 1;

    }
    else {
      
      // Otherwise if pause is active then we need to take it out of pause and start new lap/period
      if (paused) {

        paused = LOW;

        // Show "CYCLE SAFELY!" message
        showMotivator();
        motivatorShown = HIGH;
        motivatorStartTime = currentTime;

        // Increment lap counter
        currentLap++;

        resetLapVariables();
        currentDisplayMode = 1;
      }
      // Otherwise pause is not currently active so we need to save lap/period data and activate pause
      else {

        paused = HIGH;

        // Calculate duration
        currentDuration = currentTime - lapStartTime;

        // If lap duration is less than 2 seconds (which means user pressed the pause button while "CYCLE SAFELY!" message
        // was shown) then do not store the lap/ignore it
        if (currentDuration < 2000) {
          currentLap--;
        }
        // Otherwise store the lap
        else {

          // Compute distance and average miles per hour if bicycle moved
          if (revolutionCount > 0) {
            currentDistance = revolutionCount * bicycleWheelCircumference / 63360;
            currentAverageMPH = currentDistance * 3600000 / currentDuration;
          }
          
          // Store data for lap/period into array
          arrayDistance[currentLap] = currentDistance;
          arrayDuration[currentLap] = currentDuration;
          arrayAverageMPH[currentLap] = currentAverageMPH;
          arrayMaximumMPH[currentLap] = currentMaximumMPH;
  
          // Update totals for all laps/periods
          arrayDistance[0] = arrayDistance[0] + currentDistance;
          arrayDuration[0] = arrayDuration[0] + currentDuration;
          arrayAverageMPH[0] = arrayDistance[0] * 3600000 / arrayDuration[0];  
          if (currentMaximumMPH > arrayMaximumMPH[0]) {
            arrayMaximumMPH[0] = currentMaximumMPH;
          }        
        }

        // In case "Great Work Lady Mae!" has been showing, turn it off now since we want to show "PAUSED!" message
        // and we don't want it to be removed when "Great Work Lady Mae!" times out
        motivatorShown = LOW;
        
        // Show "PAUSED!" message
        showPaused();
        pausedShown = HIGH;
        pausedStartTime = currentTime;

        // We will need to show data for lap which was just finished
        showLap = currentLap;
        currentDisplayMode = 3;
      }
    }
  }

    lastPauseButton = currentPauseButton;


  // Read DISPLAY MODE push button
  currentDisplayModeButton = debounce(lastDisplayModeButton, displayModeButton);
  if (lastDisplayModeButton == LOW && currentDisplayModeButton == HIGH) {

    // If "PRESS BUTTON TO START" message has been showing then we now need to start 1st lap/period
    if (greeting) {

      greeting = LOW;  

      // Show motivational message
      showMotivator();
      motivatorShown = HIGH;
      motivatorStartTime = currentTime;

      currentLap = 1;
      resetLapVariables();
      currentDisplayMode = 1;

    }
    else {
      
      // Otherwise if motivational message is not shown nor is "PAUSED!" message shown...
      if (!motivatorShown && !pausedShown) {

        // If not currently paused (so lap is ongoing)...
        if (!paused) {

          // Flip between the two different display modes available
          if (currentDisplayMode == 1) {
            currentDisplayMode = 2;
          }
          else {
            currentDisplayMode = 1;
          }
          
          // Clear display and show appropriate labels
          showLabels(currentDisplayMode);
        }
        
        // Otherwise we are in paused mode so cycle through lap data available, including totals page
        else {
          currentDisplayMode = 3;
          showLap++;
          if (showLap > currentLap) {
            showLap = 0; // Show totals
          }
        }
      }
    }
  }
  lastDisplayModeButton = currentDisplayModeButton;   

  // If wheel revolution indicator has been showing, take if off if it has been 250 millis or more
  if (wheelTurningShown && !greeting && !paused && (currentTime >= (wheelTurningStartTime + 250))) {
    wheelTurningShown = LOW;
    lcd.setCursor(0, 0);
    lcd.print(" ");
  }

    // If wheel revolution indicator has been showing, take if off if it has been 250 millis or more
  if (!greeting && !paused && (currentTime >= (lastRevolutionStartTime + 10000)) && currentMPH > 0) {
    currentMPH = 0;
  }

  // If "Great Job Lady Mae!" has been showing, take it off if it has been 2 seconds or more
  if (motivatorShown && (currentTime >= (motivatorStartTime + 2000))) {
    motivatorShown = LOW;
    showLabels(currentDisplayMode);
  }

    // If "Paused!" has been showing, take it off if it has been 2 seconds or more
  if (pausedShown && (currentTime >= (pausedStartTime + 2000))) {
    pausedShown = LOW;
    showLabels(currentDisplayMode);
  }

  // If "PUSH BUTTON TO START" is not showing and not currently paused...
  if (!greeting && !paused) {

    // Compute milliseconds since start of lap
    currentDuration = currentTime - lapStartTime;

    // Compute distance and average miles per hour if bicycle has moved
    if (revolutionCount > 0) {
      // Compute miles traveled
      // Circumference of wheel is in meters
      currentDistance = revolutionCount * bicycleWheelCircumference / 63360;

      // Compute average miles per hour since start of lap
      currentAverageMPH = currentDistance * 3600000 / currentDuration;
    }
  }

  
  // If no messages are currently showing then update data on display
  if (!greeting && !motivatorShown && !pausedShown) {


    if (currentDisplayMode == 2) {

      starLoop(currentMPH);
      
    } 
    
      if (currentDisplayMode == 1) {
  
        lcd.setCursor(1, 0);
        lcd.print(currentDistance);
        lcd.print(" mi");
  
        lcd.setCursor(14, 0);
        if (currentMPH < 10) {
          lcd.print(" ");
        }
        lcd.print(currentMPH);
  
        computeHMS(currentDuration);
        lcd.setCursor(1, 1);
        if (intHours < 10) {
          lcd.print("0");
        }
        lcd.print(intHours);
        
        lcd.print(":");
        if (intMinutes < 10) {
          lcd.print("0");
        }
        lcd.print(intMinutes);
        
        lcd.print(":");
        if (intSeconds < 10) {
          lcd.print("0");
        }
        lcd.print(intSeconds);
  
//        lcd.setCursor(12, 1);
//        lcd.print("A");
  
        if (currentDisplayMode == 1) {
//          lcd.setCursor(12, 1);
//          lcd.print("A");
          lcd.setCursor(14, 1);
          if (currentMaximumMPH < 10) {
            lcd.print(" ");
          }
          lcd.print(currentMaximumMPH);
        }
        else {
          lcd.setCursor(10, 1);
          lcd.print("M");
          lcd.setCursor(11, 1);
          lcd.print("A");
          lcd.setCursor(12, 1);
          lcd.print("X");
          lcd.setCursor(14, 1);
          if (currentMaximumMPH < 10) {
            lcd.print(" ");
          }
          lcd.print(currentMaximumMPH);
        }
      }
    
    // Otherwise device is paused so show historical lap information
    else {

      // Update display only if we need to show data for different lap to that currently shown
      // this way display is not constantly cleared and refreshed with same data which would
      // cause display to flicker and is not needed anyway as data is not changing
      if (lapCurrentlyShown != showLap) {

        lapCurrentlyShown = showLap;
        
        lcd.clear();

        lcd.setCursor(0, 0);
        if (showLap == 0) {
          lcd.print("T ");
        } 
        else {
          lcd.print(showLap);
        }

        lcd.setCursor(3, 0);
        lcd.print("Avg");
        lcd.setCursor(7, 0);
        lcd.print(arrayAverageMPH[showLap]);
        if (arrayAverageMPH[showLap] < 10) {
          lcd.print(" ");
        }

        lcd.setCursor(10, 0);
        lcd.print("Max");
        lcd.setCursor(14, 0);
        lcd.print(arrayMaximumMPH[showLap]);
        if (arrayMaximumMPH[showLap] < 10) {
          lcd.print(" ");
        }
        
        lcd.setCursor(0, 1);
        lcd.print("        ");
        lcd.setCursor(0, 1);
        lcd.print(arrayDistance[showLap]);
//        lcd.print(" mi");
        
        computeHMS(arrayDuration[showLap]);
        lcd.setCursor(8, 1);
        if (intHours < 10) {
          lcd.print("0");
        }
        lcd.print(intHours);
        
        lcd.print(":");
        
        if (intMinutes < 10) {
          lcd.print("0");
        }
        lcd.print(intMinutes);
        
        lcd.print(":");
        
        if (intSeconds < 10) {
          lcd.print("0");
        }
        lcd.print(intSeconds);
      }        
    }
  }
}





//serial plotter: lap, distance, date and time, speed

// Compute hours, minutes and seconds for given duration expressed in milliseconds
void computeHMS(unsigned long duration) {

  float floatHours;
  float floatMinutes;
  float floatSeconds;

  intHours = 0;
  intMinutes = 0;
  intSeconds = 0;

  if (duration >= 1000) {
      floatSeconds = duration / milliSecondsInSecond % 60;
      intSeconds = floatSeconds;
      
      floatMinutes = duration / milliSecondsInMinute % 60;
      intMinutes = floatMinutes;
      
      floatHours = duration / milliSecondsInHour % 24;
      intHours = floatHours;
  }
}

// Show motivator "Great Work Lady Mae!"
void showMotivator() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("GREAT WORK");
  lcd.setCursor(4, 1);
  lcd.print("LADY MAE!");
}

// Show "PAUSED!"
void showPaused() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("PAUSED!");
}

// Show appropriate labels for current mode
void showLabels(int currentDisplayMode) {

  lcd.clear();
  switch (currentDisplayMode)     {
  case 1:
    lcd.setCursor(10, 0);
    lcd.print("S");
    lcd.setCursor(11, 0);
    lcd.print("P");
    lcd.setCursor(12, 0);
    lcd.print("D");
    lcd.setCursor(10, 1);
    lcd.print("M");
    lcd.setCursor(11, 1);
    lcd.print("A");
    lcd.setCursor(12, 1);
    lcd.print("x");   
    break;
  case 2:
//    lcd.setCursor(10, 0);
//    lcd.print("S");
//    lcd.setCursor(11, 0);
//    lcd.print("P");
//    lcd.setCursor(12, 0);
//    lcd.print("D");
//    lcd.setCursor(10, 1);
//    lcd.print("M");
//    lcd.setCursor(11, 1);
//    lcd.print("A");
//    lcd.setCursor(12, 1);
//    lcd.print("x");        
    break;
  }
}


//A debouncing function that can be used for any button
boolean debounce(boolean last, int pin)
{
  boolean current = digitalRead(pin);
  if (last != current) {
    delay(5);
    current = digitalRead(pin);
  }
  return current;
}

void starLoop(int speed) {

  delay(250);
  lcd.clear();
  int roundedSpeed = round(speed);
  
    byte image02[8] = {B00000, B00000, B00100, B00100, B01110, B01110, B01110, B11111};
    byte image01[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B01111};
    byte image17[8] = {B00011, B00000, B00001, B00001, B00011, B00011, B00100, B00000};
    byte image18[8] = {B11111, B11111, B11111, B11011, B10001, B00000, B00000, B00000};
    byte image19[8] = {B11000, B00000, B10000, B10000, B11000, B11000, B00100, B00000};
    byte image03[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11110};
    
    lcd.createChar(0, image02);
    lcd.createChar(1, image01);
    lcd.createChar(2, image17);
    lcd.createChar(3, image18);
    lcd.createChar(4, image19);
    lcd.createChar(5, image03);

    int count = 0;

    for(int i = 0; count <= roundedSpeed; i = i + 3) {

        lcd.setCursor(1+i, 0);
        lcd.write(byte(1));
        
        lcd.setCursor(2+i, 0);
        lcd.write(byte(0));
        
        lcd.setCursor(3+i, 0);
        lcd.write(byte(5));
        
        lcd.setCursor(1+i, 1);
        lcd.write(byte(2));
        
        lcd.setCursor(2+i, 1);
        lcd.write(byte(3));
        
        lcd.setCursor(3+i, 1);
        lcd.write(byte(4));
    

      count++;
//      delay(5000);
//      lcd.clear();
    }

//    delay(1000);
//    lcd.clear();
//    delay(500);

}

// Reset all variables used for calculating current/ongoing lap
void resetLapVariables() {
  revolutionCount = 0;

  lapStartTime = currentTime;

  currentDistance = 0;
  currentDuration = 0;
  currentMaximumMPH = 0;
  currentAverageMPH = 0;
}
