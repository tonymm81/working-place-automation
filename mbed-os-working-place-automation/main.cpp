/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "platform/mbed_thread.h"
#include "SSH1106.h" //screen oled
#include "ESP8266Interface.h"
#include "ntp-client/NTPClient.h"
#include <SPI.h>
#include <SPISlave.h>
#include "hcsr04.h" // ultrasonic distance meaure library
#define ntpAddress "time.mikes.fi"  // The VTT Mikes in Helsinki
#define ntpPort 123     // Typically 123 for every NTP server
#include "string" //no need use char anymore
//#pragma-import( __use_two_region_memory)//test

//testing
DigitalOut rst_wlan(D2);
SPI oledspi(PA_7, PA_6, PA_5); // mosi, miso, clk to screen, sn74hc595 chip x 2                                         
Serial pc(USBTX, USBRX, 9600);
//USBSerial pc;
//Serial pc(PC_12, PD_2, 9600);
ESP8266Interface esp(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);
//SPISlave(spiArduino);

  
  

AnalogIn Lux_sensor(PC_0);
DigitalOut ultasonic_Trigger(D14);
DigitalIn ultrasonic_echo(D15);
DigitalIn motion_sensor(D8);
DigitalOut CS_to_hc595chip(D7);
DigitalOut CS_to_screen(D0);// solder this wire D1 is broken
DigitalOut RST_to_screen(D10);
DigitalIn from_arduino_spi(D9);
DigitalIn switch_no(D5);
DigitalIn switch_yes(D6);
DigitalOut CS_to_hc595_2(D4);
DigitalOut DC_to_screen(D3);
//DigitalIn back_to_menu_button(D2); // solder wires and resistor here!!!
DigitalIn ssel(PB_12);
//spi communication setup
SSH1106 ssh1106(oledspi, CS_to_screen, DC_to_screen, RST_to_screen); //screen
SPISlave spiArduino(PB_15, PB_14, PB_13, PB_12); // mosi, miso, sck , ssel PB_12
HCSR04  usensor(D14,D15);
//SPISlave(spiArduino);
//here are the global variables
float lux = 0.0;
char udp_timestamp[35]; // temporary, using to print out the values because usb did not work
int counter, mov = 0;
bool permission = false;
bool person_in_room = false;
int choice, device = 0;
int msgback = 8;// return message to arduino
float distance_measured = 0.0;
// here is functions
int wait_buttons(int btn1, int btn2);
void screen(int line1, int line2, int line3);
int message_from_master(int message);//spi comminication
void my_spi_write(int choice,int device);//spi communication function to serial chips and oled screen
bool check_if_person_is_room(bool person_in_room); // this function checks if the person is in the room with ultrasonic sensor and movement sensor
int control_devices(bool soldering, bool power_machine, int write_flag, int send_to_595_1);
int timerule = 0;



int main()
{
    bool power_machine = false;
    bool soldering = false;
    int send_to_595_1 = 0;
    int send_to_secone_595 = 0;
    int btn1, btn2, asking_menu = 0;
    Thread threadSample(osPriorityNormal, 2000);//memory error
    ssh1106.init();
    ThisThread::sleep_for(200);
    from_arduino_spi.mode(PullDown);
    
    int message, write_flag = 0;            
    //oledspi.select();
    //ssh1106.init();
    oledspi.format(8, 0);//3 old setup
    oledspi.frequency(8000000); // old was 12000000
    spiArduino.reply(0x00);
    CS_to_hc595chip.write(1);
    CS_to_screen.write(1);
    RST_to_screen.write(1);
    CS_to_hc595_2.write(1);
    DC_to_screen.write(0);
    pc.printf("success");    
    ThisThread::sleep_for(200);
    CS_to_hc595chip.write(1);
    char number[15];
    char number1[15];
    my_spi_write(8,0);//all off
    my_spi_write(13,1); // standby led on
    //get_time_from_web();// throw error about new operator is out of memory
    //get_time_from_web(); // gives a device error
    pc.printf("success\n");
    ssh1106.init();
    ThisThread::sleep_for(200);
    while(1){
        timerule = timerule+1;
        person_in_room = check_if_person_is_room(person_in_room); // this function call is to measure with two sensors are the person in the room
        printf(" person in room %d\n", person_in_room);
        lux = Lux_sensor.read() * 1000;
        ThisThread::sleep_for(300);
        printf("lux  is: %0.2f\n", lux);
        
        
        if(from_arduino_spi.read() == 1){ // if arduino raises interrupt pin lets go to message function ja take commands from it.
            message = 0;
            message = message_from_master(message);
            printf("%d",  message);
            ThisThread::sleep_for(1000);
            
            
        }
        if (permission == true){ // here we wait user buttons state
            write_flag = control_devices(soldering, power_machine, write_flag, send_to_595_1);
            // do here menubutton
            
            if (asking_menu == 0){ // we dont know the user choice
                screen(6,1,3);// ask soldering
                btn1, btn2 = wait_buttons(btn1, btn2);
                
                if (btn1 >= 1){ soldering = true;} else{soldering==false;}
                
                screen(6,5,3); // ask power machine
                btn1, btn2 = wait_buttons(btn1, btn2);
                if (btn1 >= 1){ power_machine = true;} else {power_machine == false;}
                
                printf("yes %d, no %d\n", btn1, btn2);
                asking_menu = 1; // now we no users choice and now raise the flag
                }else{
                   screen(6,8,2);// back to menu button (solder this analog in or digital if avaible
                   send_to_595_1=2; // this is passing the timrule
                    }    
                
                
            
                }
            else{
                soldering = false;
                power_machine = false;
                asking_menu = 0;
                screen(6,1,3);
                write_flag = 0;
                
                write_flag = control_devices(soldering, power_machine, write_flag, send_to_595_1);
                }   
                     
        if ( person_in_room == false && permission == true && timerule > 20){
                if (write_flag == 2 || write_flag == 1){
                    write_flag = 4; // if person is not in room lets  shutdown th soldering
                    timerule = 0;
                }
                write_flag = control_devices(soldering, power_machine, write_flag, send_to_595_1);
                
        }
                
        if (person_in_room == true && send_to_595_1 == 0 && permission == false){
           my_spi_write(2,0); // lights on
            timerule = 0;
            send_to_595_1=1;
               
           } 
        else if (person_in_room == false && send_to_595_1 == 1 && permission == false && timerule > 20){
             my_spi_write(8,0);// lights off
             send_to_595_1=0;
             timerule = 0;  
                
                }    
         }
         
         if (timerule > 30){
                     
                timerule = 0;
                     }
                     
    
}


int control_devices(bool soldering, bool power_machine, int write_flag, int send_to_595_1){ // depends on user choice here we switch devices on and also shutdown the soldering station in person in room is false
    if (soldering == true && person_in_room == true && write_flag == 0){
        my_spi_write(15, 0); // test 2 last bit zero
        write_flag = 1;
        printf("soldering on\n");
        timerule = 0;
        }
    if ( power_machine == true && write_flag == 1){
        my_spi_write(14, 0); // test all bits up
        write_flag = 2;
        printf("power on\n");
        }
        
    if ( power_machine == false && write_flag == 1){
        my_spi_write(16, 0);
        printf("lets test tis");
        }
    if ( person_in_room == false && write_flag == 4){
        my_spi_write(16, 0); // 3 last bit zero
        write_flag = 0;
        printf("solder off\n");
        timerule = 0;
        
        }
    if (permission == false && send_to_595_1==2){ // this is because if user shut down the device, timerule is not slow the shutting down
        my_spi_write(2,0); // lights on
        }    
    
    printf("control_devices sol:%d pow %d flag %d\n", soldering, power_machine, write_flag);
    return write_flag;
    }


bool check_if_person_is_room(bool person_in_room){ // here we check is the person in room with ultrasonic sensor and motion sensor
    usensor.start();
    ThisThread::sleep_for(500); 
    distance_measured = usensor.get_dist_cm();
    ThisThread::sleep_for(500); 
    printf("sensor distance %02.f\n", distance_measured);
    mov = motion_sensor.read();
    printf("movement is: %d\n", mov);
    if ( distance_measured > 200 ){
        if ( mov == 0){
            person_in_room = false;
            }
        }else{
            person_in_room = true;
            }
    return person_in_room;
    }


void my_spi_write(int choice,int device){
    char bytesArray[] = { // here is output setups to both sn74hc595n chip
        0b10000000, //
        0b01000000, // main power, contactor here
        0b00100000, // 
        0b00010000, // 
        0b00001000, // 
        0b00000100, // 
        0b00000010,// 
        0b00000001, //
        0b00000000, // all off
        0b00001100, // 
        0b11111010, // 
        0b10111010, // soldering off
        0b01111100, //second sn74hc595n chip tree leds on
        0b01000000, // second sn74hc595n chip one led?
        0b11111111, // all on
        0b11111100,
        0b11111000, // test
        
        };
        
     
    if (device ==0){
        CS_to_hc595chip.write(0);
        oledspi.write(bytesArray[choice]);
        ThisThread::sleep_for(200);
        CS_to_hc595chip.write(1);
        }
    if (device == 1){
        CS_to_hc595_2.write(0);
        oledspi.write(bytesArray[choice]);
        ThisThread::sleep_for(200);
        CS_to_hc595_2.write(1);
    }

}



int wait_buttons(int btn1, int btn2){ // here we wait user to chooce soldering and power usage.
    btn1 = 0; 
    btn2 = 0;
    while(1){
        btn1 = switch_yes.read();
        btn2 = switch_no.read();
        ThisThread::sleep_for(200);
        if(btn1 == 1 || btn2 == 1){ 
           break;
           }
        
        }
    return btn1,btn2;
    }

void screen(int line1, int line2, int line3){ // this is printout function.  here we print the oled screen some information
    if (counter == 10)//this is because sometimes screen is messed up so lets call .init() command to reset the screen
    {
        ssh1106.init();
        ssh1106.clearBuffer();
        ThisThread::sleep_for(500);
        counter = 0;
    }
    //ssh1106.init();
    ssh1106.clear();
    ThisThread::sleep_for(600);// this is good solution. now screen works better than ever.
    char solder[10];
    char strquestion[40];
    char str_lux[20];
    char strwelcome[20];
    char choice[10];
    char working_text[30];
    char str_time[30];
    char soldering[30];
    char distance[20];
    int question = 8;
    int welcome= 8;
    int lux_value = 8;
    int choice_1 = 8;
    int working_number = 8;
    int int_time = 8;
    int soldering_ = 8;
   
    int int_distance = 8;
    char text_array[30][30] = {
        "welcome!",
        "Are you soldering to day",
        "back to menu",
        "Yes or no",
        "time is: ",
        "are you using power?",
        "Hi and have nice day",
        "No permission. Show rfid!!",
        "Devices are on",
        
        };
    if(permission == false){ // if arduino message told to device keep shut down
        strcpy(udp_timestamp, text_array[7]);
        int_time = sprintf(str_time, "%s", udp_timestamp);
        ssh1106.writeText(0,1, font_4x5, str_time, int_time);
        counter = counter + 1;
        
        
        }else{ // arduino gives permission to start
        strcpy(udp_timestamp, "here comes time ");
        question = sprintf(strquestion, text_array[line2]);
        welcome = sprintf(strwelcome, text_array[0]);
        lux_value = sprintf(str_lux, "Lux value: %0.2f", lux);
        choice_1 = sprintf(choice, text_array[line3]);
        working_number = sprintf(working_text, text_array[line1]);
        int_time = sprintf(str_time, "%s", udp_timestamp);
        int_distance = sprintf(distance, "Distance is: %0.2f", distance_measured);
        soldering_ = sprintf(soldering, "soldering station is: %s", solder);
        ssh1106.writeText(0,1, font_4x5, str_time, int_time);
        ssh1106.writeText(0,2, font_4x5, strwelcome, welcome); // here we print out the text to screen
        ssh1106.writeText(0,6, font_4x5, strquestion, question);
        ssh1106.writeText(0,7, font_4x5, choice, choice_1);
        ssh1106.writeText(0,5, font_4x5, str_lux, lux_value);
        ssh1106.writeText(0,3, font_4x5, working_text, working_number);
        ssh1106.writeText(0,4, font_4x5, distance, int_distance);
        counter = counter + 1;
        //ssh1106.update();
        }
    }


int message_from_master(int message)// here we go to spi connection slave mode and wait message from master. This gives the permission to start up the system.
{
    spiArduino.format(8, 0);
    spiArduino.reply(msgback);
    while(1){
        
        
        int counter = 0;
        
        ThisThread::sleep_for(500);
        if (spiArduino.receive()) {
            printf("meggages sending and receive\n");
            message = spiArduino.read();   // Read byte from master
            spiArduino.reply(msgback);         // Make this the next reply
            ThisThread::sleep_for(1600); // 1500 worked // sometimes visit two times in function
            // lets test if this fix the old message problem Sometimes spi sends back old value 2 or 3 times. have to find what causes this problem.
            break;
                
            }
       counter = counter +1;
       //ThisThread::sleep_for(500);
       if (counter == 25){
           break;
           } 
    }
    printf("here is message from master %d\n", message);
    
   printf("out of message function\n");
   if(message == 1){
       
        my_spi_write(12,1);
        permission = true;
        person_in_room = true;
        msgback=10; // permission
        
    }
    if (message == 3 || message == 2){
        my_spi_write(13,1);
        permission = false;
        person_in_room = false;
        
        msgback=12; // no permission
        
        }
   return message; 
}


