# working-place-automation (beta)
-this is my school project. It has been exiting project because i have to study a lot about hardware side.

- this device has two microkontrollers. arduino uno and nucleo f303re. arduino keeps the rfid reader(spi) and communication with
nucleo via SPI. Arduino also measures the temperature and adjust the fan speed depending on set temperature. Set temperature you
can adjusment with potentiometer. Arduino has also 20x4 led screen what uses I2C commmunication. It also adjust the fan speed via mosfet
but there i have to make some optical isolator because motor distraction signal.

- Second microcontroller nucleo is the slave device. Arduino gives permission to start and also to shutdown with Spi communication. 
nucleo has second spi communication pins what are controlling spi oled screen and two sn74hc595n chips. one chip is for leds and second
is using relays via uln2804 chip. 
Relays are connecting the power to sockets. There is 5 sockets where 2 of them are because lights, one to soldering station
, one to power unit and one to the main power. 

Near screen is three button where user can select what devices should power up when permission is true. When
soldering is choosed, device keeps watching is the user in the room with ultrasonic sensor and motion sensor. If not its shutdown the soldering station because 
fire security and waits if the user is coming back room. Device has also wlan(working on it) what ask time from udp and print it to screen. 
Device has also lux sensor what prints the value to screen.

Parts:

Arduino uno
Nucleo f303re microcontroller
uln2806 chip
sn74hc595 chip x2pc
welleman rfid reader
20x4 I2C lcd screen
sh1106 oled screen (spi)
mosfet 2amps
resistor 18k 4pc
resistor 800 ohm 6pc
resistor 300 ohm 4pc
switch 3pc
potentiometer 1pc
ultrasonic sensor HCSR04
dht11 sensor
welleman motion sensor
welleman lux sensor
leds 8pc
