# Smart **Classroom** using LPC1768

## Overview

**Smart Classroom** is an embedded system project developed using the **LPC1768** microcontroller. The system integrates multiple functionalities to automate classroom management, including attendance tracking, quiz handling, display controls, environmental controls, and emergency alerts.

## Team Members

- **Ayan Ayush**
- **Kapish Choudary**
- **Arpita Magaraj**
- **Nirmal Kushna Pati**

## Features

The project includes six major features controlled via keypad input:

1. **Attendance System** - Handles student attendance tracking using authentication mechanisms.
2. **Quiz System** - Enables teachers to conduct quizzes for students.
3. **Display Control** - Manages classroom displays for announcements and notifications.
4. **Environmental Controls** - Controls devices like fans and lights manually and automatically based on sensor data.
5. **Emergency Alert System** - Sends emergency alerts for safety protocols.
6. **Exit Functionality** - Provides a safe way to terminate the system.

## Environmental Control Features

The **environmental control module** allows for manual and automatic operation of fans and lights based on temperature and ambient light conditions. The key functionalities include:

- **Fan and Light Control:**
  - Manual on/off control using keypad input.
  - Automatic control based on real-time sensor data (temperature and light intensity).
  - Uses **hysteresis logic** to prevent frequent toggling of fans and lights.

- **Sensor-based Auto Mode:**
  - If the temperature exceeds a predefined threshold, the fan turns on automatically.
  - If the ambient light falls below a threshold, lights are turned on automatically.
  - Users can override auto mode using keypad inputs.

## Technologies Used

- **Microcontroller:** LPC1768
- **Programming Language:** C
- **Display Module:** LCD
- **Input Interface:** Keypad
- **Sensors:** Temperature sensor, LDR (Light Dependent Resistor)
- **Actuators:** Fan, Light

## How to Run the Project

1. **Set up the LPC1768 development board.**
2. **Flash the firmware onto the microcontroller.**
3. **Connect the required peripherals (LCD, Keypad, Sensors).**
4. **Power on the system and interact using the keypad.**

## Future Improvements

- Implement IoT integration for remote monitoring.
- Enhance the user interface with a touchscreen.
- Expand features for automated scheduling and AI-based classroom management.

