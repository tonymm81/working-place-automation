#include <IRremote.h> // remote control library
#include <Adafruit_Sensor.h> // dht sensor library
#include <DHT.h> // dht sensor library
#include <DHT_U.h> // dht sensor library
#define dht_pin A1 //sensor pin
#define lux_sensor A2 // lux meter sensor pin
#define DHTTYPE    DHT11 // sensor type
#include <U8glib.h> // oled display library
#include <math.h> //math library
int RECV_PIN = 10; // ir remote pin variables
IRrecv irrecv(RECV_PIN); // ir remote setups
//oled and dht global variables
U8GLIB_SH1106_128X64 u8g(4, 5, 6, 7); // oled setup pins
DHT_Unified dht(dht_pin, DHTTYPE); // dht type setup
decode_results signals;
//here is global variables
uint32_t delayMS;
int set_temp = 20; //default temp set
word HEXcode;
int fanmode, fanspeed, hum, temp, fan_precent, val, movementcounter, fanpermission = 0; 
int poweroff = 1; //off state
int pwm_pin = 2;
int ledpin = 22;
int dect_sensor = 24;
int movestate = LOW;
int tresholdvalue = 10;
float Rsensor;

void u8g_prepare(void) { // oled setups
 u8g.setFont(u8g_font_6x10);
 u8g.setFontRefHeightExtendedText();
 u8g.setDefaultForegroundColor();
 u8g.setFontPosTop();
}



void u8g_string(uint8_t a) { // oled settings
 u8g.drawStr(30+a,31, " 0");
 u8g.drawStr90(30,31+a, " 90");
 u8g.drawStr180(30-a,31, " 180");
 u8g.drawStr270(30,31-a, " 270");
}


uint8_t draw_state = 0;


void draw(void) { // oled draw function
 fanmode = "some def";
 u8g_prepare();
 sensors_event_t event;
 dht.temperature().getEvent(&event);
 temp = event.temperature; // lets read temperature
 dht.humidity().getEvent(&event);
 
 hum = event.relative_humidity; // lets read humidity from sensor
 char tempc[9];
 sprintf (tempc, "%d", temp);// temperature value has to convert to string
 char humc[9];
 sprintf (humc, "%d", hum); // humidity value has to convert string
 char set_tempc[9];
 sprintf (set_tempc, "%d", set_temp);
 //int temp = DHT.read11(dht_pin);
 u8g.drawStr(0, 12, "temperature is:"); // printing on screen
 u8g.drawStr( 90, 12, tempc );
 u8g.drawStr( 110, 12, "C" );
 u8g.drawStr(0, 24, "humidity is:");
 u8g.drawStr( 80, 24, humc);
 u8g.drawStr( 110, 24, "%");
 u8g.drawStr(0, 36, "set temp is:");
 u8g.drawStr( 75, 36, set_tempc);
 u8g.drawStr( 110, 36, "C");
 u8g.drawStr(0, 48, "fan is now:");
 char fan_precentc[9]; // fanprecent has to convert to string
 sprintf (fan_precentc, "%d", fan_precent);
 if (fanspeed == 0){
  u8g.drawStr( 65, 48, "off");
  u8g.drawStr( 90, 48, fan_precentc);
  u8g.drawStr( 110, 48, "%");
 }else{
  u8g.drawStr( 65, 48, "on");
  u8g.drawStr( 80, 48, fan_precentc);
  u8g.drawStr( 110, 48, "%");
 }
// 
 
 
}

void setup(void) {
 pinMode(ledpin, OUTPUT); // led control
 pinMode(pwm_pin, OUTPUT); // fan control with mosfet
 pinMode(dect_sensor, INPUT); // detection sensor
 
 
 dht.begin();
 Serial.begin(9600);
 sensor_t sensor;
 dht.temperature().getSensor(&sensor);
 dht.humidity().getSensor(&sensor);
 delayMS = sensor.min_delay / 1000;
#if defined(ARDUINO)
 pinMode(13, OUTPUT); 
 digitalWrite(13, HIGH); 
 irrecv.enableIRIn();
 irrecv.blink13(true);
#endif
}


void fanstatus(){ // here we count what is the temperature difference and what is the fan speed 
  
  int difference = set_temp - temp; // here we count the difference between temperatures
  if(difference == 0){
    fanspeed = 190; // this value controls pwm out 
    fan_precent = 74; // this value is for the screen.
  }
  else if (difference == 1){
    fanspeed = 140;
    fan_precent = 54;
  }
  else if (difference == 2){
    fanspeed = 120;
    fan_precent = 47;
  }
  else if (difference >= 3){
    fanspeed = 0;
    fan_precent = 0;
  }
  else if(difference == -1){
    fanspeed = 210;
    fan_precent = 82;
  }
  else if(difference == -2){
    fanspeed = 230;
    fan_precent = 90;
  }
  else if (difference <= -3){
    fanspeed =255;
    fan_precent = 100;
  }
}

void check_mov_and_light(){ //  this luxsensor need really much light before works and movement sensor needs a time rule.
 int lux = analogRead(lux_sensor);
 movestate = digitalRead(dect_sensor);
 
 Serial.println(digitalRead(dect_sensor));
 if (lux > 1 ){ // lux has to be more than 1
    movementcounter++; // this is counter what shutdown the fan when nothing moves on the room
    Serial.println(movementcounter);
    Serial.println(lux);
    if (movestate == HIGH && poweroff == 1){ // this power on the fan if lux and movement are true
      poweroff = 0; // power flag
      fanpermission = 1; //power permission
      movementcounter = 0;
      fanstatus();
      Serial.println("movement detected");
      
    }
    
    else if (poweroff ==1){
      movementcounter = 0;
      Serial.println("shutdown");
    }
    else if (poweroff == 0 && fanspeed > 0 && fanpermission == 1){ // if movement detector is not high but fan permission is 1, then check the temp and fans
      fanstatus(); 
      Serial.println("checking speed");

      if (movestate == HIGH){
        movementcounter = 0;
        Serial.println("movement high");
        fanstatus();
       
      }
      
      else if (movementcounter == 800){ // if not movement happend a while, this shutdown the devie and waits movement detector
        poweroff = 1;
        fanspeed =0;
        fan_precent =0;
        movementcounter =0;
        fanpermission  = 0;
        
      }
      
        
      else if (movementcounter > 800){
        poweroff = 1;
        fanspeed =0;
        fan_precent=0;
        movementcounter =0;
        fanpermission = 0;
       
      }
    }

  if (movestate == HIGH && lux > 1 && fanspeed == 0  ){ // sometimes device notice movement but not starting fan. this if clause fixes it.
    fanstatus();
    poweroff = 0;
    fanpermission = 1;
  }
    
 
 }else if (lux == 0){ // if not light on the room, device is offline
    Serial.println("device offline");
    poweroff = 1;
    fanspeed = 0;
    fan_precent =0; 
    movementcounter = 0;
    fanpermission  = 0;
 }
 
  
  
 
}


void ir_remote(){ // if button pressed here we make a temp set or power off
  if (irrecv.decode(&signals)) { // ir remote buttons
        
        HEXcode = (signals.value); // this not hexcode but some number series this ir remote gives
        if (HEXcode == 36975)
        {
          set_temp = set_temp+1; // + button pressed
          digitalWrite(ledpin, LOW); // when button pressed led is blinking
          delay(500);
          digitalWrite(ledpin, HIGH);
          fanstatus();
        }
        if (HEXcode == 43095){ // - button pressed
          set_temp = set_temp-1;
          digitalWrite(ledpin, LOW); // when button pressed led is blinking
          delay(500);
          digitalWrite(ledpin, HIGH);
          fanstatus();
        }
        if (HEXcode == 41565){ // power button pressed
          digitalWrite(ledpin, LOW); // when button pressed led is blinking
          delay(500);
          digitalWrite(ledpin, HIGH);
          poweroff = 1;
           // if the button is pressed, we change poweroff value and therefore we dont roll the fan and dont check the temp ant set_temp values.
          if (poweroff == 1){
            
            poweroff = 2;
            fanspeed, fan_precent, fanpermission  = 0;
            
            
          }
          else if(poweroff == 2){
            poweroff = 0;
          }
          
          
        }
        irrecv.resume(); // get the next signal
    }
}

void loop(void) {
 analogWrite(pwm_pin, fanspeed); // here we adjust the fan speed
 digitalWrite(ledpin, HIGH); // power led
 check_mov_and_light(); // here we check devices permission and control the fans
 ir_remote();
 u8g.firstPage(); //graphical function call
 
 do {
 draw();
 } while( u8g.nextPage() );
 
 // increase the state
 draw_state++;
 if ( draw_state >= 9*8 )
 draw_state = 0;
 
 
 delay(300);
}