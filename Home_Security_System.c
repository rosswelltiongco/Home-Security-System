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
bit dir;



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
void updateTimer(int time);
void updateLCD();
void updateStatus();
void turnOnLaser();
void turnOffLaser();
void countDownTimer(int num);
void updateTimerLEDs();
void turnOnAlarm();
void turnOffAlarm();
void flashIntruder();
void soundAlarm();
void resetTimer();


/**************************************************************/
void main(){
		while(1){
			disarmedState();
			updateTimerState();
			armedState();
			countdownState();
			intruderState();
		}
		
	
	


}
/**************************************************************/


void disarmedState()
{
}
void updateTimerState()
{
}
void armedState()
{
}
void countdownState()
{
	countDownTimer(12);
}
void intruderState()
{
}




////////////////////////////////////////////////////////////////
//               LCD  Helper Functions                        //
////////////////////////////////////////////////////////////////
void updateTimer(int time)
{
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
void updateStatus()
{
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
	
  init_lcd();                                                                                       
	
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
		
		write_to_lcd(0x86,COMMAND); //Write to second line
		write_to_lcd(msb,LCD_DATA); //Writes left to right
		write_to_lcd(lsb,LCD_DATA);
		
		if (msb == 0x30 && lsb == 0x30)
		{
			break;
		}
		
		lsb--; //Deccrementing least significant bit
		
		MSDelay(250); //Replace with internal timer delay		 
	}
}

void updateTimerLEDs()
{
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