/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
 // here is libraries

#include "mbed.h"
#include <SPI.h>
#include "SSH1106.h" //screen oled
#include "ESP8266Interface.h" // wlan 
#include "ntp-client/NTPClient.h"

#define ntpAddress "time.mikes.fi"  // The VTT Mikes in Helsinki
#define ntpPort 123     // Typically 123 for every NTP server


//wlan setup
ESP8266Interface esp(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);
//pin setups
SPI spi(D11, D12, D13); // mosi, miso, sck
SPI oledspi(PA_7, PA_6, PA_5); // mosi, miso, clk to screen

DigitalIn motiondetector(D10);
DigitalOut alsCS(D7);
DigitalIn switch_yes(D2);
DigitalIn switch_no(D3);
AnalogIn benchswitch(A0);
DigitalOut sn74hc595CS(D9);
PwmOut speaker(A2);
DigitalOut oledCS(D8);
DigitalOut rst_oled(D5);
DigitalOut dc_oled(D6);
AnalogIn rfid_permission(A3);

//global values
char solder[15];
char alsByte0 = 0;           // 8 bit data from sensor board, char is the unsigned 8 bit
char alsByte1 = 0;           // 8 bit data from sensor board
char alsByteSh0 = 0;
char alsByteSh1 = 0;
char als8bit = 0;
unsigned short alsRaw = 0;   // unsigned 16 bit
float alsScaledF = 0.0;   
char udp_timestamp[30];
int alsScaledI = 0; // 32 bit integer
int getmovement = 0;
char out8 = 0x69;
int usr_choice = 1;
bool loop_rule = false;
bool asking_loop = true;
int no_soldering = 2;
int switch_position = 0;
int no_switch = 0;
int what_state = 0;
int  bench = 0;
int soldermovement = 0;

// functions
int getALS(); // function for the Digilent PmodALS sensor
int getPhotoDiode(); // function for the photo diode circuit
void speakercall();
void write_to_595(int what_state);
void screen();
void locking();
int benchswitch_state();
void get_time_from_web();
const char *sec2str(nsapi_security_t sec);//wlan setups
void scan_demo(WiFiInterface *wifi);//wlan demo

SSH1106 ssh1106(oledspi, oledCS, dc_oled, rst_oled); // screen library call




int main(){ 
    // SPI for the ALS 
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 12MHz clock rate
    
    int timer = 0;
    spi.format(8,0); 
    spi.frequency(12000000);
    oledspi.format(8,0); 
    oledspi.frequency(12000000);
    // ready to wait the conversion start
    rst_oled.write(0);
    dc_oled.write(0);
    alsCS.write(1);
    oledCS.write(1);
    sn74hc595CS.write(1);
    write_to_595(8);
    oledCS.write(1);
    screen();
    speakercall();
    get_time_from_web();
    ssh1106.init();
    locking();
    
    while (loop_rule) {
        float per = rfid_permission.read(); // here we read if user shutdown the system, lets go the locking function
        if (per < 0.50f){
            usr_choice = 1; // this is for the screen
            no_soldering = 1; // lets change the soldering rule
            asking_loop = true; // for screen loop
            locking();
            
            }
        if (no_soldering == 0){ //if user choose to solder today lets check the benchswitch. if it not true then movement detector start calculate time and shut down the power from soldering station
            bench = benchswitch_state();
            if (bench == 0){
                getmovement = motiondetector.read();
                if (getmovement== 0 ){ // if not movement lets count and later power of the soldering station if nobody is in room
                    soldermovement = soldermovement + 1;
                    }
                if (soldermovement == 50){
                    write_to_595(11); // if time rule is full, power off soldering station
                    strcpy(solder, "off");
                    }
                if (getmovement == 1){
                    write_to_595(10);// and if noticed movement then power up the soldering station
                    soldermovement = 0;
                    strcpy(solder, "on");
                    }
                }
            }
        alsScaledI = getALS(); // lets check the lux value
        alsScaledF = getPhotoDiode();
        ThisThread::sleep_for(1000);
        screen();    
        timer = timer +1;
        if (timer == 40){
            get_time_from_web();
            timer = 0;
            }
        }
}


int getALS(){ // this not work correct, figure it out!!
    
    alsCS = 0; 
    ThisThread::sleep_for(500);
    alsByte0 = spi.write(0x00);
    alsByte1 = spi.write(0x00);
    ThisThread::sleep_for(500);
    alsCS = 1;
   
    alsByteSh0 = alsByte0 << 4;
    alsByteSh1 = alsByte1 >> 4;
    
    als8bit =( alsByteSh0 | alsByteSh1 );
    
    alsRaw = als8bit; // 
    alsScaledF = (float(alsRaw))*(float(4.45));                                      
    ThisThread::sleep_for(100);
    return (int)alsScaledF; 
}


int getPhotoDiode() {
    AnalogIn ainPD(A0);
    unsigned short pd12bit = 0;
    float pdScaledF = 0.0;
    pd12bit = ainPD.read_u16() >>4; // leftmost 12 bits moved 4 bits to right.
    pdScaledF = (float(pd12bit))*(float(0.1)); 
    return (int)pdScaledF;
}


void speakercall(){ // to pwm speaker/buzzer
    speaker.period(1.0/500.0);
    ThisThread::sleep_for(10000);
    speaker=0.0;
    
}

void write_to_595(int what_state){
    char out8 = 0x69;
    char dataArray[] = { // here is output setups to sn74hc595n chip
        0b10000000, //relay1
        0b01000000, // relay2
        0b00100000, // relay3
        0b00010000, // relay 4
        0b00001000, // relay 5
        0b00000100, // led red
        0b00000010,// led green
        0b00000001, //NC
        0b00000000, // all off
        0b00001100, // red light and lights
        0b11111010, // green led and all on
        0b10111010, // soldering off
        
        };
        sn74hc595CS.write(0);
        spi.write(dataArray[what_state]); // this writes the setup what user choose
        ThisThread::sleep_for(500);
        sn74hc595CS.write(1);
        ThisThread::sleep_for(500);
        
    }
    
    
void screen(){
    //SPI lcd(p11, NC, p13);      // mosi, miso (nc), sclk /// nää täytyy modata omaa käyttöö
    //DigitalOut cs(p15);         // chip select  (active low)
    //DigitalOut cd(p14);         // command/data (0=command, 1=data)
    //DigitalOut rst(p16);        // Reset (active low)
    ssh1106.init();
    // here is screen variables
    char strquestion[40];
    char lux[30];
    char strwelcome[20];
    char choice[10];
    char working_text[30];
    char str_time[30];
    char soldering[30];
    int question = 8;
    int welcome= 8;
    int lux_value = 8;
    int choice_1 = 8;
    int working_number = 8;
    int int_time = 8;
    int soldering_ = 8;
    //here is the screen text setups
    question = sprintf(strquestion, "are you using soldering station");
    welcome = sprintf(strwelcome, "Welcome!!");
    lux_value = sprintf(lux, "lux value: %0.2f", alsScaledF);
    choice_1 = sprintf(choice, "Yes Or no");
    working_number = sprintf(working_text, "Hi! and have nice day!");
    int_time = sprintf(str_time, "%s", udp_timestamp);
    soldering_ = sprintf(soldering, "soldering station is: %s", solder);
    //ssh1106.init();
    ssh1106.clear();
    ssh1106.clearBuffer();
    
    while(asking_loop){ // here we ask that if user want to solder or not
        ThisThread::sleep_for(500);
        ssh1106.writeText(0,2, font_4x5, strwelcome, welcome); // here we print out the text to screen
        ssh1106.writeText(0,3, font_4x5, strquestion, question);
        ssh1106.writeText(0,4, font_4x5, choice, choice_1);
        ssh1106.writeText(0,5, font_4x5, lux, lux_value);
        ssh1106.writeText(0,1, font_4x5, str_time, int_time);
        ThisThread::sleep_for(100);
        //ssh1106.update();
        ThisThread::sleep_for(100);
        switch_position = switch_yes.read();
        no_switch = switch_no.read();
        if (switch_position == 1){ // if user wants to soldering this if clause pull up the outputs and put no_soldering to 0 state
            usr_choice = 0;
            no_soldering = 0;
            strcpy(solder, "on"); // lets copy to screen
            write_to_595(10); // all outputs are on
            asking_loop = false;
            break;
                
            }
        
            
        if(no_switch == 1){ // if user dont want to solder then no_solder state is 1
            usr_choice = 0;
            no_soldering = 1;
            strcpy(solder, "off");// lets copy to screen
            asking_loop = false;
            break;
                
                
            }
        if (usr_choice == 1){
            break;
            }                
         }
            
            
    if (no_soldering == 1 && usr_choice == 0){ //user choose to not soldering. screen text setup
        //ssh1106.init();
        ssh1106.clear();
        ssh1106.clearBuffer();
        ThisThread::sleep_for(500);
        ssh1106.writeText(0,2, font_4x5, strwelcome, welcome);// here we print out the text to screen
        ssh1106.writeText(0,3, font_4x5, working_text, working_number);
        ssh1106.writeText(0,1, font_4x5, str_time, int_time);
        ssh1106.writeText(0,4, font_4x5, lux, lux_value);
        ssh1106.writeText(0,5, font_4x5, soldering, soldering_);
        ThisThread::sleep_for(100);
        
        }
        
    if (no_soldering == 0 && usr_choice == 0){ //user choose to soldering screen text setup
        //ssh1106.init();
        ssh1106.clear();
        ssh1106.clearBuffer();
        ThisThread::sleep_for(500);
        ssh1106.writeText(0,2, font_4x5, strwelcome, welcome);// here we print out the text to screen
        ssh1106.writeText(0,3, font_4x5, working_text, working_number);
        ssh1106.writeText(0,1, font_4x5, str_time, int_time);
        ssh1106.writeText(0,4, font_4x5, lux, lux_value);
        ssh1106.writeText(0,5, font_4x5, soldering, soldering_);
        ThisThread::sleep_for(100);
        }
    
 }


void locking(){ // program starts from this function and waits user
    loop_rule = false; // mainloop rule
    int counter = 0;
    int local_rule = 0;
    write_to_595(5); // only red led
    for (int i= 0; i <=8;){ // here we wait user. if movement detected, powerr on the lights
      counter = counter +1;
      switch_position = 0;
      no_switch = 0;
      float per = rfid_permission.read();  // rfid reader
      getmovement = motiondetector.read(); // motion detection to lights
      alsScaledI = getALS();
      alsScaledF = getPhotoDiode();
      screen();//screen update
      if (getmovement == 1 && local_rule == 0 ){ // keeps light on some time
          if (getmovement == 1 && alsScaledF < 150.00f){
            write_to_595(9); // lights and red led
            local_rule = 1;
            counter = 0;
            }
          }
      bench = benchswitch_state();
      if (per <= 1.00f && per > 0.50f && bench == 1){ // locking opened. lets power up all execpt soldering station
          loop_rule = true;
          asking_loop = true;
          write_to_595(11); // all on , green light, soldering off
          i = 8;
          break;
          }
      if (counter == 25){
          counter = 0;
          local_rule = 0;
          write_to_595(5);// only red led
          }  
    ThisThread::sleep_for(1000);
    
    }
        
}


int benchswitch_state(){ //here we check the benchswitch state
    float pos = 0.0;
    int state = 0;
    pos = benchswitch.read();
    if ( pos <= 1.00f && pos > 0.55f){
        state = 1;
    }else{
        state = 0;
    }
    return state;
    } 
  
  
void get_time_from_web(){ // in this function we try to get timestamp from net
  // Setting up WLAN
 
    printf("WiFi example\r\n\r\n");
    
    //Store device IP
    SocketAddress deviceIP;
    ThisThread::sleep_for(500); // waiting for the ESP8266 to wake up.
      
    scan_demo(&esp);
 
    printf("\r\nConnecting...\r\n");
     int ret = esp.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\r\nConnection error\r\n");
        //return -1;
    }
 
    
    printf("Success\n\n");
    printf("MAC: %s\n", esp.get_mac_address());
    esp.get_ip_address();//&deviceIP
    printf("IP: %s\n", deviceIP.get_ip_address());
    printf("Netmask: %s\n", esp.get_netmask());
    printf("Gateway: %s\n", esp.get_gateway());
    printf("RSSI: %d\n\n", esp.get_rssi());
 
// --- WLAN
    
// NTP demo    
    printf("\nNTP Client example (using WLAN)\r\n");
    
    
    NTPClient ntp(&esp);
    
    ntp.set_server(ntpAddress, ntpPort);
      
    
    time_t timestamp = ntp.get_timestamp(); //lets get the time stamp from net
        
    if (timestamp < 0) {
        printf("An error occurred when getting the time. Code: %u\r\n", timestamp);
    } else {
        printf("The timestamp seconds from the NTP server in\r\n  32 bit hexadecimal number is %X\r\n", timestamp);
        printf("  decimal number is %u\r\n", timestamp);
        timestamp += (60*60*3);  //  GMT +3  for Finland for the summer time.
        printf("Current time is %s\r\n", ctime(&timestamp));
        }
        strcpy (udp_timestamp, ctime(&timestamp));


}   
      
      
// WLAN security
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
} 

void scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;
 
    printf("Scan:\r\n");
 
    int count = wifi->scan(NULL, 0);
 
    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;
 
    ap = new WiFiAccessPoint[count];
 
    count = wifi->scan(ap, count);
    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\r\n", count);
 
    delete[] ap;
}