# working-place-automation
this device will lockup your working place power and lights. using rfid tags.

parts:

1: nucleo st432kc microkontroller

2: Esp 2806 Wlan adapter

3: Pmodals lux sensor (spi controlled)

4: motion sensor

5: uln 2803 chip

6: sn74hc595n chip (spi controlled)

7: leds

8: 12v relays

9: oled screen sh1106(spi)

10: buttons


short describe Mbed-os-working-place-automation project:
 I make a device what automatic the working place. It control 5 different powersource. It protecting the user to not forget soldering station on state too long. It also lock up your working place electrics. When you arrive you working place, the device light up the lights if the lux value is under  100. this device has also benchswitch. When you show the rfid tag and power on the device. On screen you can se date time from udp and also asking are you soldering today. If you put yes, the soldering is powering on. When user is not sitting behind the desk and not moving in the room device shutdown the soldering machine, and power on it if motion is detected. If you show the rfid tag again, the device shutdown. 


arduino fan controller:
 i also made a arduino based fan controller:
 
 parts:
 
 1: fans 2 pc
 
 2: oled screen sh1106 (spi)
 
 3: mosfet
 
 4: diode
 
 5: motion detector
 
 6: lux sensor
 
 7: dht 11 temperature and humidity sensor
 
 8: ir remote control
 
 9: leds and resistors
 

 short describe:
 this is simple device. you can setup the set point to temp and arduino adjust the fan speed based on that. If room is dark then device is shut down. if there is light
 enough, it will power on if motion is detected. You can adjust the setpoint with remote control.

 there is schematics also in devices folder

 
