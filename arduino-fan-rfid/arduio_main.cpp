
#include <RFID.h>// wellemans rfid reader library
#include <SPI.h>
//#include <Wire.h> check if needed anymore
#include <LiquidCrystal_I2C.h>
//#include <Adafruit_Sensor.h> // dht sensor library
#include <DHT.h> // dht sensor library
#include <DHT_U.h> // dht sensor library
#define SPI_MOSI        MOSI //spi communication defines
#define SPI_MISO        MISO
#define SPI_SCK        SCK
#define SS_PIN 10  //rfid   
#define RST_PIN 3 //rfid
#define DHTTYPE    DHT11 // the temp sensor type
#define READ  3

//pins
int ssel_to_nucleo = 4;
int pwm_to_fan = 9;
int CS_to_nucleo_controller = 6;
int IR_remote = 0;
int potentionmeter = A1;
int temp_hum = A2;
//temp and hum
DHT_Unified dht(temp_hum, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,20,4);
// rfid pins
RFID rfid(SS_PIN,RST_PIN);
// saved rfid card numbers

int serNum[5];
int cards[][5] = {{22,148,129,241,242},
                 {234, 204, 102, 36, 100},
                 {21, 214, 195, 172, 172}}; // saved rfid card numbers
bool access;

int on_state = 0;
int set_temp = 18; //default temp set
int temp_value_copy = 0;
int fanmode, fanspeed, hum, temp, fan_precent, val, movementcounter, fanpermission = 0; 
int poweroff = 1; //off state
int ledpin = 0;
int dect_sensor = 1;
int movestate = LOW;
int tresholdvalue = 10;
float Rsensor;
uint32_t delayMS;
int potentiometer_position = 0;
void send_to_slave(int permission);



void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  lcd.init();
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  //SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));//testing
  rfid.init();//rfid declare
  pinMode(pwm_to_fan, OUTPUT);
  pinMode(CS_to_nucleo_controller, OUTPUT);
  pinMode(IR_remote, INPUT);
  pinMode(potentionmeter, INPUT);
  pinMode(temp_hum, INPUT);
  pinMode(ssel_to_nucleo, OUTPUT);
  
  //remove this when not needed Just because start of developing
  digitalWrite(CS_to_nucleo_controller, LOW);//high will interrupt!!!
  digitalWrite(ssel_to_nucleo, HIGH); // low will start the frequince
  
  
}



bool rdif_permission(bool access){ //rfid function
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));//testing12000000
  int permission = 2; // 0 is no access, 1 is access 
  if(rfid.isCard()){
      if(rfid.readCardSerial()){
            Serial.print(rfid.serNum[0]);
            Serial.print(" ");
            Serial.print(rfid.serNum[1]);
            Serial.print(" ");
            Serial.print(rfid.serNum[2]);
            Serial.print(" ");
            Serial.print(rfid.serNum[3]);
            Serial.print(" ");
            Serial.print(rfid.serNum[4]);
            Serial.println(" ");
            for(int x = 0; x < sizeof(cards); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != cards[x][i]) {
                      access = false;
                      break;
                  } else { // if rfid tag found, lets change the access value true
                      access = true;
                      if (on_state == 0){ // if device is off state lets change the on_statevalue to 1
                          on_state = 1;
                      }else{
                        on_state = 0; // if it on state,  lets change it to 0 and shut down the device next time when rfid tag is readed
                      }
                  }
              }
              if(access) break;
            }
        }
        
       if(access){ // access granted, now lets check if the on_state value is 0 or 1. if 0 device will shutdown
           if (on_state == 1){
              Serial.println("device has permission");
              delay(1000);
              send_to_slave(1);
           }
           if(on_state == 0){
              access = false;
              Serial.println("device has not permission");
              delay(1000);
              send_to_slave(2);
           }
      } else if (access == false){ // if rfid tag is not on list lets flash the red light
           Serial.println("Not allowed!"); 
       }        
  }
  delay(1000);
  rfid.halt();
  SPI.endTransaction();
  return access;
    
}
  

void print_to_screen(){
  if(access == false){
    lcd.init();
    lcd.setCursor(0,0);
    lcd.print("No access to automation");
    analogWrite(pwm_to_fan, 0);
    
  }
  else{
    lcd.init();
    lcd.setCursor(0,0);
    lcd.print("Welcome to workingpl.");
    lcd.setCursor(0,1);
    lcd.print("temp:");
    lcd.print(temp);
    lcd.print(" C");
    lcd.print(" hum: ");
    lcd.print(hum);
    lcd.print(" %");
    lcd.setCursor(0,2);
    lcd.print("set temp: ");
    lcd.print(set_temp);
    lcd.print(" C");
    lcd.setCursor(0,3);
    lcd.print("fan is: ");
    lcd.print(fan_precent);
    lcd.print(" %");
    measure_temp_hum();
    if (temp > 0){
      measure_temp_hum();
      fanstatus();
      analogWrite(pwm_to_fan, 255);
  }
  }
  
  
}


void measure_temp_hum()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature; // lets read temperature
  
  dht.humidity().getEvent(&event);
  hum = event.relative_humidity; // lets read humidity from sensor
  
}


void fanstatus(){ // here we count what is the temperature difference and what is the fan speed 
  read_potentionmeter();
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


void read_potentionmeter(){
  
  int value = analogRead(potentionmeter);
  set_temp = value / 10;
  
  Serial.println(value);
} 

void send_to_slave(int permission){ // this is a sending function to nucleo mircocontroller. it has defined a slave
  //Serial.println(permission);
  int answer = 0;
  delay(500);
  digitalWrite(CS_to_nucleo_controller, HIGH);
  delay(3000);//test1000 old
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));//testing 12000000
  
  digitalWrite(ssel_to_nucleo, LOW);
  delay(700);//old 500
  SPI.transfer(permission); // here we send nucleo to startup signal 
  answer = SPI.transfer(READ);//off state, 10 == true state, 12 false state
  //Serial.println(answer);
  delay(1500);//testing
  
  digitalWrite(ssel_to_nucleo, HIGH);
  digitalWrite(CS_to_nucleo_controller, LOW);
  delay(500);
  Serial.println("visiting in spi cpmmunication");
  Serial.println(answer);
  SPI.endTransaction();
  permission = 0;
  
}

void loop() {
  
 SPI.endTransaction();
  Serial.println("moi");
  access = rdif_permission(access);
  lcd.backlight();
  print_to_screen();
   delay(1000);
}