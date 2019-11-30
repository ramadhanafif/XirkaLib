#include "D:\User\Documents\GitHub\XirkaLib\XST_GPIO\XST_GPIO.h"
#include "D:\User\Documents\GitHub\XirkaLib\utility\utility.h"

int main(){
	int i = 0, 
			x = 0;


	XST_GPIO gp(GPIO1);

	gp.pinMode(3,OUTPUT);		
	timer_start();
	

	while(1){
		if (x >= 1000)
		{
			i+=10;
			x = 0;
		}
		x++;
		gp.digitalWrite(3,HIGH);
		delaymicros(10*(i));
		gp.digitalWrite(3,LOW);	
		delaymicros(10*(100-i));
		if (i >=100)
			i = 0;
	}
}