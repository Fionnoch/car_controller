# car_controller
Microcontroller code designed control when a car gear changes based on user inputs.

**Car**

    * ECU - ?
    
    + 4 speed gear box.
        - 1st gear - High, Low
        - 2nd gear - Low, Low
        - 3rd gear - Low, High
        - 4th gear - High, High 
        
    * Turbo


**Microcontroller - ESP32**

    * Canbus module which could be used to communicate with the car.
    
    * 4 inputs that can be used to control: Gear Up, Gear Down, Turbo & ECU (used to control when the microcontroller is on or not).
        - GPIO_NUM_34
        - GPIO_NUM_35
        - GPIO_NUM_36
        - GPIO_NUM_39
        
    * 4 relay outputs which can be used to control: gear choice, Turbo, ECU change over (whether the gear selection is controlled by the microcontroller or the default car).
        - GPIO_NUM_16
        - GPIO_NUM_17
        - GPIO_NUM_18
        - GPIO_NUM_19
        
    * Variable 12v signal used to control the clutch. 
        - GPIO_NUM_26
        
    * i2c bus for an oled display 
        - SDA - GPIO_NUM_32
        - SCL - GPIO_NUM_33

To Do List:
- [x] Control when the microcontroller is controlling the car (i.e when ECU input is high enable microcontroller).
- [x] Update display screen with current gear.
- [x] Prevent gears from skipping. 
- [ ] Read gear state when car is not being controlled by the microcontroller to ensure seemless transitioning.
- [ ] Tuning of clutch.
