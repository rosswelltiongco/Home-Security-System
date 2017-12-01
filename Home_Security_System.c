//////////////////////////////////
//     Home Security System     //
//////////////////////////////////

#include <reg51.h>


#define COMMAND 0
#define LCD_DATA 1

//Declaring Port Pins
//P0
sbit ARM = P0^0;
//P1
sbit LED0 = P1^0;
sbit LED1 = P1^1;
sbit LaserSwitch = P1^2;
sbit EncB = P1^3;
sbit GREEN = P1^5;
sbit YELLOW = P1^6;
sbit RED = P1^7;
//P2
#define ldata P2
sbit busy = P2^7; // connected to DB7 on the LCD
//P3
sbit EncA = P3^2;
sbit BREAK = P3^3;
sbit rw = P3^5;
sbit rs = P3^6;
sbit en = P3^7;

//variables
bit dir;//1 = cw, 0 = ccw
unsigned int time = 50; //  Default:50 (99-7)/2 = 46, rounded up
unsigned int delayVal = 0;

//Interrupt functions
void timer1(void) interrupt 3{//50ms 
	TR1 = 0;//turn timer off
	delayVal = delayVal - 1;//decrease delay value
	TH1 = 0x4B;//initial values
	TL1 = 0x92;
}
void encoder() interrupt 0//
{
	if(EncB == 1)
	{
		dir = 1;
		time++;
	}
	else
	{
		dir = 0;
		time--;
	}
}
void breakBeam() interrupt 2
{
	
}
void delay()//delay 1 second
{
	delayVal = 20;
	while(delayVal > 0)
	{
		TR1 = 1;
	}
	TR1 = 0;
}
void delayHalf()//delay 0.5 second
{
	delayVal = 10;
	while(delayVal > 0)
	{
		TR1 = 1;
	}
	TR1 = 0;
}

//State Functions
void disarmedState();
void updateTimerState();
void armedState();
void countdownState();
void intruderState();

//Function Prototypes
void init_lcd();//initialize LCD
void write_to_lcd(unsigned char value, bit mode);//write data or command
void MSDelay(unsigned int itime);//delay 1 ms
void lcdready(void);//check if lcd is ready to write to
void displayTime(unsigned int time);
void updateLCD();
void displayArmed();
void displayDisarmed();
void displayIntruder();
void turnOnLaser();
void turnOffLaser();
void countDownTimer(int num);
void turnOnAlarm();
void turnOffAlarm();
void flashIntruder();
void soundAlarm();
void resetTimer();


/**************************************************************/
void main(){
	//Interrupt enable
	EA = 1;
	ET1 = 1;
	//Timer enable
	TMOD = 0x10;//timer 1 mode 1
	TH1 = 0x4B;//high bit value
	TL1 = 0x92;//low bit value
	TR1 = 0;//turn off timer 0
	
	//enable external interrupts
	EX1 = 1;
	EX0 = 1;
	//Delcare inputs and outputs
	//P0 = 0xFF;
	//P1 = 0x08;
	//P3 = 0x0C;
	LED0 = 0;
	LED1 = 0;
	//turn off LEDS
	GREEN = 1;
	YELLOW = 1;
	RED = 1;
	
	init_lcd();
	//countDownTimer(12);
	//displayDisarmed();
	//countDownTimer(7);
	//intruderState();
		while(1){
				countDownTimer(12);
				intruderState();
			//disarmedState();
			//updateTimerState();
			//armedState();
			//countdownState();
			//intruderState();
		}
		
	
	


}
/**************************************************************/


void disarmedState()
{
	displayTime(time);
}
void updateTimerState() //Fixme: No need for this state? Will wait for John's lecture
{
	//FIXME: will the encoder just take it to this state?
	displayTime(time);
	delay(); //Short delay so that 
}
void armedState()
{
	displayTime(time);
}
void countdownState()
{
	countDownTimer(time);
}
void intruderState()
{
	displayIntruder();
	delay();
	//displayArmed();
	for(;;)
	{
		LED0 = 1;
		LED1 = 0;
		delayHalf();
		LED0 = 0;
		LED1 = 1;
		delayHalf();
	}
}




////////////////////////////////////////////////////////////////
//               LCD  Helper Functions                        //
////////////////////////////////////////////////////////////////
void displayTime(unsigned int time)
{
	//Converting and separating to what was passed in
	char msb = (time/10)+48;
	char lsb = (time%10)+48;
	
  unsigned char code timer[]="Timer:NA seconds";
  unsigned char t = 0;                                                                            
	
	//Writing first line
	write_to_lcd(0x80,COMMAND); //Move to start of first line
	while (timer[t]!='\0') 
   write_to_lcd(timer[t++],LCD_DATA);

	//Write to start of number on first line from left to right
	write_to_lcd(0x86,COMMAND); 
	write_to_lcd(msb,LCD_DATA); 
	write_to_lcd(lsb,LCD_DATA); 
}


//setup LCD for required display
void init_lcd(){
	//all instructions here are commands, not data
	
	write_to_lcd(0x38,COMMAND);

	write_to_lcd(0x0E,COMMAND);
	
	write_to_lcd(0x06,COMMAND);
	//clear display and return cursor to home position
	write_to_lcd(0x01,COMMAND);
}

void write_to_lcd(unsigned char value, bit mode){
	lcdready();
	ldata = value;
	rs = mode; //1 for data, 0 for command
	rw = 0;
	en = 1;
	MSDelay(1);
	en = 0;
}
void MSDelay(unsigned int itime){
	unsigned i, j;
	for(i=0;i<itime;i++)
		for(j=0;j<1275;j++);

}
void lcdready(){
	busy = 1;
	en = 1;
	rs = 0;//command
	rw = 1;//read command
	while(busy){
		en = 0;
		MSDelay(1);
		en = 1;
	}
	en = 0;
	rw = 0;
}

void updateLCD()
{
}

void displayArmed()
{
	unsigned char code msg[]="ARMED";
  unsigned char i=0;                                                                                 

	//Writing to second line
	write_to_lcd(0xC0,COMMAND);
	
	while (msg[i]!='\0') 
   write_to_lcd(msg[i++],LCD_DATA);
}
void displayDisarmed()
{
	unsigned char code msg[]="DISARMED";
  unsigned char i=0;                                                                                      

	//Writing to second line
	write_to_lcd(0xC0,COMMAND);
	
	while (msg[i]!='\0') 
   write_to_lcd(msg[i++],LCD_DATA);
}
void displayIntruder()
{
	unsigned char code msg[]="INTRUDER!";
  unsigned char i=0;                                                                                    

	//Writing to second line
	write_to_lcd(0xC0,COMMAND);
	
	while (msg[i]!='\0') 
   write_to_lcd(msg[i++],LCD_DATA);
}

void turnOnLaser()
{
}
void turnOffLaser()
{
}
void countDownTimer(int time)
{
	unsigned int count;
	
	//Converting and separating to what was passed in
	char msb = (time/10)+48;
	char lsb = (time%10)+48;
	
  unsigned char code timer[]="Timer:NA seconds";
  unsigned char t = 0;                                                                            
	
	//Writing first line
	write_to_lcd(0x80,COMMAND); //Move to start of first line
	while (timer[t]!='\0') 
   write_to_lcd(timer[t++],LCD_DATA);
	
	
	for (count = time; count >= 0; count--){		
		if (lsb < 0x30){
			lsb = 0x39;
			msb--;
			if (msb < 0x30){
				msb = 0x39;
			}
		}
		
		//Write to start of number on first line from left to right
		write_to_lcd(0x86,COMMAND); 
		write_to_lcd(msb,LCD_DATA);
		write_to_lcd(lsb,LCD_DATA);
		
		//turn on and off LEDS
		if(msb > 0x30 )//greater than 10, green
		{
			GREEN = 1;
			YELLOW = 0;
			RED = 0;
		}
		else
		{
			if(lsb >= 0x34 && lsb <= 0x36)//4-6 yellow
			{
				GREEN = 0;
				YELLOW = 1;
				RED = 0;
			}
			else if(lsb >= 0x30 && lsb <= 0x33)//0-3 red
			{
				GREEN = 0;
				YELLOW = 0;
				RED = 1;
			}
			else//7-9 green
			{
				GREEN = 1;
				YELLOW = 0;
				RED = 0;
			}
		}
		
		//Keeps both numbers at 00
		if (msb == 0x30 && lsb == 0x30)
		{
			break;
		}
		
		lsb--; //Deccrementing least significant bit
		
		delay(); //1 second delay	 
	}
}

void turnOnAlarm()
{
}
void turnOffAlarm()
{
}
void flashIntruder()
{
}
void soundAlarm()
{
}
void resetTimer()
{
}