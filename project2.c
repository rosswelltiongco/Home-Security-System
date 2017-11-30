#include <reg51.h>

#define COMMAND 0
#define LCD_DATA 1

//give constant names to our ports
//P0
sbit ARM P0^0;
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
//P3
sbit EncA = P3^2;
sbit BREAK = P3^3;
sbit rw = P3^5;
sbit rs = P3^6;
sbit en = P3^7;
//variables
bit dir

//function prototypes
void init_ldc();//initialize LCD
void write_to_lcd(unsigned char value, bit mode);//write data or command
void MSDelay(unsigned int itime);//delay 1 ms
void lcdready();//check if lcd is ready to write to





/**************************************************************/
void main(){
		
		
	
	


}
/**************************************************************/

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

