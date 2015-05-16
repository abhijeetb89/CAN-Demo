Aim of this project is to develop small demo or prototype for CAN similar to CAN in vehicle.

Four microcontrollers have been used as CAN nodes.

1.STM32F407VGT6 (Present on STM32F4 Discovery Board)
http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF252419

2.STM32F091RC6 (Present on STM32 Nucleo-F091 Board)
https://developer.mbed.org/platforms/ST-Nucleo-F091RC/

3.LPC1768 (Present on Android Open Accessory Application Kit)
http://www.embeddedartists.com/products/app/aoa_kit.php

4.LPC11C24 Present on Android Open Accessory Application Kit)
http://www.embeddedartists.com/products/app/aoa_kit.php

Each of these CAN nodes sends and receives CAN messages as defined in can_database.xls file 
which is at 20_Requirements folder.

Sensors connected to nodes - 

1. Accelerometer - detect crash 

2. Light Sensor - measure ambient light 

3. Temperature Sensor - measure temperature inside vehicle 

4. Roraty POT - measures speed 

5. Ultrasonic Sensor - detect obstacle while parking 

Using push buttons interior and exterior lights of vehicle are controlled.
    
To observe messages sent by node, STM32F4 Discovery Board is connected to PC via USB. This board reads
all messages on CAN and sends it over USB. Windows based software reads these messages and displays
on GUI. Trace can be stored.
