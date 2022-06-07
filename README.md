# arduino-speedometer
Arduino speedometer project 

Project Description: An arduino powered bicycle speedometer for a child's bicycle. There are two display settings:
  1. Displays the time elapsed, distance traveled, current speed in mph and maximum speed in mph. 
  2. Displays a series of stars ranging from 1 to 5 stars depending on speed. 

# Hardware
- Arduino Uno
- Hall Effect Sensor
- LCD Display
- 2 switch buttons
- MicroSD Card reader Module
- MicroSD Card
- Magnet

The speedometer works by detecting a magnet fixed to a wheel on the bicycle. Everytime the wheel copletes one full revolution, the speed is calculated and displayed on screen based on the amount of time elapsed between detections. 

![speedometer](https://user-images.githubusercontent.com/90277439/172464379-72cd4166-f597-4585-821f-2ff42708005d.jpg)
