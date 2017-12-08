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
unsigned int countDownNum = 7;
unsigned int prevTime;

unsigned int dir;//1 = cw, -1 = ccw
bit update;//0 = stay same,1 = update

unsigned int time = 50; //  Default:50 (99-7)/2 = 46, rounded up
unsigned int delayVal = 0;

//bit variables to keep track of state
bit ARMED = 0;//countdown finishes
bit timer;
bit Laser_Break = 0;

//Interrupt functions
void timer1(void) interrupt 3{//50ms 
	TR1 = 0;//turn timer off
	delayVal = delayVal - 1;//decrease delay value
	TH1 = 0x4B;//initial values
	TL1 = 0x92;
}

void breakBeam() interrupt 2//senses if beam is broken
{
	Laser_Break = 1;//flag to check if beam is broken
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
void delayOneTen()
{
	delayVal = 3;
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
void updateLCD(unsigned int time);//updates numbers
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


void encoder() interrupt 0//
{	
	if(EncB == 1)//cw
	{
		dir = 1;
	}
	else//ccw
	{
		dir = -1;
	}
	update = 1;//update flag, interrupt will always show what direction
}
/**************************************************************/
enum states//5 states
{
	disarmed_state,
	update_timer_state,
	armed_state,
	countdown_state,
	intruder_state
};
enum states state = disarmed_state;
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
	IT1 = 1;
	IT0 = 1;
	//Delcare inputs and outputs
	LED0 = 0;
	LED1 = 0;
	//turn off LEDS
	GREEN = 1;
	YELLOW = 1;
	RED = 1;
	//Turn off laser
	LaserSwitch = 0;
	
	init_lcd();
	//countDownTimer(12);
	//displayDisarmed();
	//countDownTimer(7);
	//intruderState();
	displayTime(countDownNum);
		while(1)
		{
			switch(state)
			{
				case disarmed_state:
					//call function
					disarmedState();
					//next state logic
					if(update == 1)
					{
						state = update_timer_state;
					}
					if(ARM == 1)
					{
						prevTime = countDownNum;//save previous value
						state = countdown_state;
					}
					break;
				case update_timer_state:
					//call function
					updateTimerState();
					//next state logic
					state = disarmed_state;
					break;
				case armed_state:
					//call function
					armedState();
					//next state logic
					if(ARM == 0)
					{
						state = disarmed_state;
					}
					if(ARMED == 1 && Laser_Break == 1)
					{
						state = countdown_state;
					}
					break;
				case countdown_state:
					//call function
					countdownState();
					//next state logic
					if(ARM == 0)
					{
						state = disarmed_state;
					}
					if(ARMED == 0 && timer == 0)
					{
						state = armed_state;
					}
					if(ARMED == 1 && timer == 0)//already armed and countdown is zero
					{
						state = intruder_state;
					}
					break;
				case intruder_state:
					//call function
					intruderState();
					//next state logic
					if(ARM == 0)
					{
						state = disarmed_state;
					}
					break;
				default:
					state = disarmed_state;
			}
		}
		
	
	


}
/**************************************************************/
////////////////////////////////////////////////////////////////
//                     State Functions                        //
////////////////////////////////////////////////////////////////

void disarmedState()
{
	//displayTime(countDownNum);
	delayOneTen();
	displayDisarmed();
	turnOffLaser();
	ARMED = 0;
	//turn off intruder LEDs
	LED0 = 0;
	LED1 = 0;
	//update timer LEDs
	GREEN = 0;
	RED = 1;
	YELLOW = 1;
	
}
void updateTimerState() //Fixme: No need for this state? Will wait for John's lecture
{
	//FIXME: will the encoder just take it to this state?
	update = 0;//recognize that it is updating
	countDownNum = countDownNum + dir;//inc or decrease, always
	
	//check if in bounds
	if(countDownNum > 99)
	{
		countDownNum = 99;
	}
	if(countDownNum < 7)
	{
		countDownNum = 7;
	}
	updateLCD(countDownNum);//updateLCD with new num
	delayOneTen();
}
void armedState()
{
	displayArmed();
	turnOnLaser();
	timer = 1;//not zero
	ARMED = 1;//armed flag
	updateLCD(prevTime);
}
void countdownState()
{
	countDownNum = countDownNum - 1;//decrease counter
	updateLCD(countDownNum);
	//update timer flag
	if(countDownNum == 0)
	{
		timer = 0;
	}
	//update LEDs
	if(countDownNum >= 7)//green
	{
		GREEN = 0;
		YELLOW = 1;
		RED = 1;
	}
	else if(countDownNum >= 4 && countDownNum <= 6)//yellow
	{
		GREEN = 1;
		YELLOW = 0;
		RED = 1;
	}
	else//RED
	{
		GREEN = 1;
		YELLOW = 1;
		RED = 0;
	}
	
}
void intruderState()
{
	displayIntruder();
	while(ARM == 1)//alternate LEDs and don't change states
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

void updateLCD(unsigned int time)//updates numbers
{
	char msb = (time/10)+48;
	char lsb = (time%10)+48;
		//Write to start of number on first line from left to right
	write_to_lcd(0x86,COMMAND); 
	write_to_lcd(msb,LCD_DATA); 
	write_to_lcd(lsb,LCD_DATA); 
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
	LaserSwitch = 1;
}
void turnOffLaser()
{
	LaserSwitch = 0;
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
			GREEN = 0;
			YELLOW = 1;
			RED = 1;
		}
		else
		{
			if(lsb >= 0x34 && lsb <= 0x36)//4-6 yellow
			{
				GREEN = 1;
				YELLOW = 0;
				RED = 1;
			}
			else if(lsb >= 0x30 && lsb <= 0x33)//0-3 red
			{
				GREEN = 1;
				YELLOW = 1;
				RED = 0;
			}
			else//7-9 green
			{
				GREEN = 0;
				YELLOW = 1;
				RED = 1;
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