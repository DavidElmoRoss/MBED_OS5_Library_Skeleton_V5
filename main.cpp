/* PROGRAMMER’S BLOCK
Title: Burglar Alarm System
Author: Your NAME, Student ID, Lab Section
Date: Date Project was started
Description: Full description of Project
*/
// INCLUDE SECTION
#include "mbed.h"                       // include mbed library
#include "Grove_LCD_RGB_Backlight.h"    // include Grove LCD library
#include "DigitDisplay.h"               // include Grove DigitDisplay Libary

// HARDWARE PIN CONFIGURATION Section
Grove_LCD_RGB_Backlight LCD(D14,D15);   // LCD is name associated with I2C pins D14,D15
DigitDisplay segs(D2,D3);               // segs is name associated with DigitDisplay D2,D3
InterruptIn PIR_Device(D6);             // PIR_DEVICE is associated with Pin D6
Serial blue(D1,D0);                     // Bluefruit Uart Friend
// GLOBAL VARIABLE DECLARATIONS Section
short Pir_detected;                     // 0 = Motion not deteced, 1= Motion detected
char Bnum;                              // Button Number from Uart Friend
unsigned short counter;                 // used for flashing INTRUDER DETECTED

// FUNCTION PROTOTYPES Section
void Alarm_on(void);                    // Alarm on Message on LCD
void Alarm_off(void);                   // Alarm off Message on LCD
void Alarm_arming(void);                // Alarm arming Message on LCD
void Alarm_armed(void);                 // Alarm armed Message on LCD
void Alarm_mes_white(void);             // Intruder Detected in White
void Alarm_mes_red(void);               // Intruder Detected in RED
void Alarm_disable(void);               // 3 Disable Alarm Message
void Alarm_disarm(void);                // Alarm disarmed message

void seg_counter(void);                 // counts down from 10 to 0 then clears all segment

void INIT(void);                        // clears LCD and 4 digit display

// INTERRUPT SERVICE ROUTINE Section
void IRQ(void)                  
{
    Pir_detected=1;                     // Pir_detected set to 1 every time motion detected
}


int main(void)
{ 
// LOCAL VARIABLE DECLARATIONS Section
short Selector=0;                       // Selector =0  Nothing selected from CONTROL PAD
                                        // Selector =1  Turns on ALarm
                                        // Selector =2  Arms alarm, counts down and Motion Sensing
                                        // Selector =3  Disarms the alarm
                                        // Selector =4  Shuts off burglar alarm
// GLOBAL VARIABLES INITIALIZATION Section
Pir_detected =0;                        // set Pir_detected to 0, no motion detected
Bnum='0';                               // set BlueFuit Control Pad value to '0'

PIR_Device.rise(&IRQ);                  // On rising edge of PIR_Device, will execute IRQ code
PIR_Device.disable_irq();               // starts off with Motion Detector disabled
                                        // default 0 for no motion detected

// Mainline Loop
 for(;;)                                // Endless loop
 {
  INIT();                               // Clears 4 Digit display and LCD
  while(Selector==0)                    // While nothing has been selected
   {
       if(blue.getc()=='!')             // Check for '!' 'B' sequence from CONTROL PAD
       {
           if(blue.getc()=='B')
           {
               Bnum=blue.getc();
               if(Bnum=='1')            // if sequence is '!' 'B' '1'
               {    
                    Selector=1;         // set Selector to 1
               }
           }
       }
   }
   Alarm_on();                          // Put Alarm_on Message on LAC
   while(Selector==1)                   // looking for either '!' 'B' 2 or
   {                                    // '!' 'B' '4' from Bluefruit Control Pad
       if(blue.getc()=='!')
       {
           if(blue.getc()=='B')
           {
               Bnum=blue.getc();
               if((Bnum=='2')||(Bnum=='4'))
               {    
                    if(Bnum=='4')       // if '!' 'B' '4" then Alarm_off and Selector 0
                    {
                        Alarm_off();
                        wait(2.0);
                        Selector=0;
                    }
                    else
                    {
                        Selector=2;     // else set Selector to 2
                    }
 
               }
           }
       }

   }
   while(Selector==2)                   // while Selector is 2
   {
       Alarm_arming();                  // send Alarm_arming message to LCD
       seg_counter();                   // Count from 10 to 0 on 4 digit
       Alarm_armed();                   // Alarm_armed message on LCD
       Selector=3;                      // set Selector to 3
   }
    
   
   while (Selector==3)                  // while Selector is 3
   {
       PIR_Device.enable_irq();         // Enable PIR motion Detector
       Pir_detected=0;                  // Set Pir_detected to 0

       while(Pir_detected==0)           // while Pir_detected is 0
       {
           wait(0.1);                   // give some time for IRQ to be execited
       }
         PIR_Device.disable_irq();      // Disable PIR Motion Detector     
         Pir_detected=0;                // reset Pir_detected to 0

         for(counter=1; counter <=5; ++counter)
         {
           Alarm_mes_white();           // Flash Intruder Detected in RED and
           wait(0.5);                   // WHITE 5 times
           Alarm_mes_red();
           wait(0.5);
         }
        Selector=4;                     // Set Selector to 4
    }
      
    
    while(Selector==4)                  // while Selector is 4
    { 
        Alarm_disable();                // send Alarm_disable message to LCD
        if(blue.getc()=='!')            // check for '!' 'B' '3' from Control Pad
       {
           if(blue.getc()=='B')
           {
               Bnum=blue.getc();
               if(Bnum=='3')
               {                        // if it is then send Alarm Disarm to LCD
                   Alarm_disarm();      // then wait for '!' 'B' '2' or '!' 'B' '4'
                   while(Selector==4)   // and set Selector appropriately
                   {
                        if(blue.getc()=='!')
                        {
                            if(blue.getc()=='B')
                             {
                                Bnum=blue.getc();
                                if (Bnum=='2')
                                {
                                    Selector=2;
                                }
                                else
                                if (Bnum=='4')
                                {
                                    Selector=1;
                                }
                             }
                  
                        }
                   }


               }
           }
       }
       
    
    }
 }

}


// FUNCTION DEFINITIONS Section
// Alarm on Message on LCD
void Alarm_on(void)
{
    LCD.setRGB(0x00,0xFF,00);
    LCD.locate(0 , 0);
    LCD.print("ARM ALARM     2 ");
    LCD.locate(0 , 1);    
    LCD.print("ALARM OFF     4 ");
}
// Alarm off message on LCD
void Alarm_off(void)
{
    LCD.setRGB(0x80,0x00,0x80);
    LCD.locate(0 , 0);
    LCD.print(" ALARM IS about ");
    LCD.locate(0 , 1);    
    LCD.print("  to Turn OFF   ");
}
// Alarm arming message on LCD
void Alarm_arming(void)
{
    LCD.setRGB(0xff,0x00,0x00);
    LCD.locate(0 , 0);
    LCD.print("ALARM is ARMING ");
    LCD.locate(0 , 1);    
    LCD.print("Please Leave now");    
}
// Alarm armed message on LCD
void Alarm_armed(void)
{
    LCD.setRGB(0x00,0xff,0x00);
    LCD.locate(0 , 0);
    LCD.print("  ALARM ARMED   ");
    LCD.locate(0 , 1);    
    LCD.print(" Motion Sensing ");    
}
// Intuder Detected in WHITE on LCD
void Alarm_mes_white(void)
{
    LCD.setRGB(0xff,0xff,0xff);
    LCD.locate(0 , 0);
    LCD.print("   Intruder    ");
    LCD.locate(0 , 1);    
    LCD.print("   DETECTED    ");      
}
// Intruder Detected in RED on LCD
void Alarm_mes_red(void)
{
    LCD.setRGB(0xff,0x00,0x00);
    LCD.locate(0 , 0);
    LCD.print("   Intruder     ");
    LCD.locate(0 , 1);    
    LCD.print("   DETECTED     ");      
}
// 3 Disable alarm in Maroon on LCD
void Alarm_disable(void)
{
    LCD.setRGB(0x80,0x00,0x80);
    LCD.locate(0 , 0);
    LCD.print("Alarm Sounding   ");
    LCD.locate(0 , 1);    
    LCD.print("3  Disable ALarm ");     
}
// Alarm Disamr Message in Green to LCD
void Alarm_disarm(void)
{
    LCD.setRGB(0x00,0xff,0x00);
    LCD.locate(0 , 0);
    LCD.print("  ALARM DISARMED ");
    LCD.locate(0 , 1);    
    LCD.print(" 2- ARM,  4- OFF ");     
}
// Counts from 10 to 0 on 4 digit display then clears
void seg_counter(void)
{
    unsigned short i;
    for(i=10;i>0;--i)
    {
        segs.write(i);
        wait(1.0);
    }
    segs.write(i);
    wait(1.0);
    segs.clear(); 
}
// Clears the LCD and turns off all colours
// Clears the 4 Digit Display
void INIT(void)
{
    LCD.clear();
    LCD.setRGB(0,0,0);
    segs.clear();
}