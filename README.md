# working-place-automation (beta)
-this is my school project. It has been exiting project because i have to study a lot about hardware side.


## Short instruction about the project

- this device has two microkontrollers. arduino uno and nucleo f303re. arduino keeps the rfid reader(spi) and communication with
- nucleo via SPI. Arduino also measures the temperature and adjust the fan speed depending on set temperature. Set temperature you
- can adjusment with potentiometer. Arduino has also 20x4 led screen what uses I2C commmunication. It also adjust the fan speed via - - mosfet but there i have to make some optical isolator because motor distraction signal.

- Second microcontroller nucleo is the slave device. Arduino gives permission to start and also to shutdown with Spi communication. 
- nucleo has second spi communication pins what are controlling spi oled screen and two sn74hc595n chips. one chip is for leds and - second
- is using relays via uln2804 chip. 
- Relays are connecting the power to sockets. There is 5 sockets where 2 of them are because lights, one to soldering station
- one to power unit and one to the main power. 

- Near screen is three button where user can select what devices should power up when permission is true. When
- soldering is choosed, device keeps watching is the user in the room with ultrasonic sensor and motion sensor. If not its shutdown - - the soldering station because 
- fire security and waits if the user is coming back room. Device has also wlan(working on it) what ask time from udp and print it to screen. 
- Device has also lux sensor what prints the value to screen.



### Parts:

- Arduino uno

- Nucleo f303re microcontroller

- uln2806 chip

- sn74hc595 chip x2pc

- welleman rfid reader

- 20x4 I2C lcd screen

- sh1106 oled screen (spi)

- mosfet 2amps

- resistor 18k 4pc

- resistor 800 ohm 6pc

- resistor 300 ohm 4pc

- switch 3pc

- potentiometer 1pc

- ultrasonic sensor HCSR04

- dht11 sensor

- welleman motion sensor

- welleman lux sensor

- leds 8pc


## version 100
- i have build up the hardware side and there was also lots of not so easy problems. arduino has one broken digital pin
- and nucleo has also one pin broken. nucleo causes problem with usb serial communication and arduino didnt set the pin to zero at all.
- I was define that pin to SSEL to nucleo. 

## 101 version.
### bugs  and figure out these:
- devices are communicating some how and sensors are working. in spi therre is some problem because sometimes old messages leave to - - memory
- and device might send old message. in spi the small delays after and before communication are important but i have adjust those. so when
- i build up the software i have to figure out what causes that old message problem.Also fan are causing some disturb signal to serial communication.

#### working now.
-sensors, screens in arduino and nucleo. serial communication chips are working and rfid reader. 

## 102 version.
### bugs. network still not working, spi is sending sometimes old messages, i cant flash a new firmware to esp8266 wlan card.
- maybe i keep little time off of this project. lots of hours behind with this project.

## 103 version. 10.3.2022
- now motion sensor is working.i add person in room boolean when it goes false if permission is true, it shutdown the soldering.
- if motion detected, it power on the soldering station. normally when device dont have permission its light up the lights if movement detected. 
- i make a function what keeps checking ultrasonic sensor and motion sensor. if they noticed both that person is room then this value is setting false.
- littlebit problems with wlan. I update the firmware to 8266 and add rst pin to nucleo. Not working, gives device error. and sometimes spi keeps old
message. perhaps delays need to adjusment before and after send message arduino->nucleo and nucleo->arduino functions.

- I also noticed that arduino D2 pin was broken. It wont go 0 state at all. this causes me lots of problems because i was using this pin
- to ssel to nucleo and connection didnt work because this pin..
some how nucleo has earlier problems about serial communication(usb print) and i search the fault a long time and final try i flash the empty code
- with usb setting and print clause and it work. when i define the pins one by time i noticed that D1 pin causes this problem. its not connected now.

## version 104. 2.4.2022
- i add some timerules that lights will stay on if perso is not in room for 20 seconds. i also made a connections to high voltage side and now i testing the
relays. powerstation it wont shut down, there is some problems with buttons i think.

## version 107.
- now its working quite well. i add relays between high voltage and microkontroller side. now i have to figure out why network is not working. there is also
smallbug that every device will shut down with delay. 



## version 108

- Program works in the microcontroller quite well and also controls now the power outlets. It reads Rfid tag and allows the user to choose, is he/she using the soldering station or not. The time out rules work. If now one is not near of workingpace, this device will power off the pulp.

### plan to next version

- Next step is to debug, why the wlan card is not working. I think and remember, that this was some kind of hardware issue and also my microcontroller does not support the objects, so perhaps I need to build different program, what search the wlan devices from network.

- I have to plan, what features this project needs more, because this project has been in pause mode a long time so there is still lots of work to do.

- Figure out the bugs, what needs to be fixed.

- 