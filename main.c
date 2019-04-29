// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// PA0,1,2,3,4,5 connected to 3x3 Keypad
// PC12,13,14,15 connected to LEDs (or Relays)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
#include "LCD.h"


#define			PIN_GETAR		PA1
#define 		PIR					PA0
#define 		LED					PC12
#define			TOMBOL			PB15
#define 		TRUE				1
#define			FALSE				0

uint32_t input_key = 0;
uint32_t pass_key[6];
uint32_t num_key = 0;
int count_pass = 0;
uint32_t password[6] = {1,2,3,4,5,6};
int check = 0;
int detek = 0;
int timer = 30;


//BUAT KEYPAD
void Init_GPIO(void)
{
	  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	  PC12=1; PC13=1; PC14=1; PC15=1;
}


// display an integer on four 7-segment LEDs
void Display_7seg(uint16_t value)
{
  uint8_t digit;
	digit = value / 1000;
	CloseSevenSegment();
	ShowSevenSegment(3,digit);
	CLK_SysTickDelay(5000);
			
	value = value - digit * 1000;
	digit = value / 100;
	CloseSevenSegment();
	ShowSevenSegment(2,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 100;
	digit = value / 10;
	CloseSevenSegment();
	ShowSevenSegment(1,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 10;
	digit = value;
	CloseSevenSegment();
	ShowSevenSegment(0,digit);
	CLK_SysTickDelay(5000);
}




//INISIALISASI BUZZER
void Init_Buzzer(void)
{
	GPIO_SetMode(PB, BIT11, GPIO_PMD_OUTPUT);
	PB11=1; // turn off Buzzer
}

void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
	PB11 =0; // turn ON Buzzer
	CLK_SysTickDelay(100000); // Delay
	PB11 =1; // turn OFF Buzzer
	CLK_SysTickDelay(100000); // Delay
	}
}



void Init_PWM0(void)
{
	PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  3); // 0.544ms = ~3% duty_cycle
	PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
	PWM_Start(PWM0, PWM_CH_0_MASK);
}

void Servo_Turn(uint8_t pos)
{
	PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50, pos);
	CLK_SysTickDelay(200000); // latency for turning motor
}


void check_pass(void)
{
	for(check = 0; check<6; check++)
	{
		if(pass_key[check] == password[check])
				detek = FALSE;
	}
	count_pass++;
	
}


uint32_t input_pass(void)
{
	if(ScanKey()) 
	{
		input_key = ScanKey();
		pass_key[num_key] = input_key;
		num_key++;
		if(num_key == 5) 
		{
			check_pass();
			num_key = 0;
		}
	}
	
		
		
}








int main(void)
{
	uint32_t i =0;
	
	
	SYS_Init();
	OpenKeyPad();
	OpenSevenSegment();
	Init_GPIO();
	init_LCD();
	Init_PWM0();
	Init_Buzzer();
	clear_LCD();
	GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT); //PIN PIR PA0
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT); //PIN LED
	GPIO_SetMode(PA, BIT1, GPIO_MODE_INPUT); //PIN SENSOR GETAR = PA1
	GPIO_SetMode(PB, BIT15, GPIO_PMD_OUTPUT); //PIN BUAT TOMBOL

	
	//test keypad
 	while(1) 
  {
		i=ScanKey();
		switch(i) {
			case 1 : PC12=0; break;
			case 2 : PC13=0; break;
			case 3 : PC14=0; break;
			case 4 : PC15=0; break;
			default: PC12=1; PC13=1; PC14=1; PC15=1; break;
		}	
		Display_7seg(i);
		CLK_SysTickDelay(100000);
	}
	
	//test LCD
	print_Line(0, "Nu-LB-NUC140    ");
  print_Line(1, "Cortex-M0 @50MHz");
  print_Line(2, "128KB Flash ROM ");
  print_Line(3, "32KB  SRAM      ");	
	
	
	// TEST PIR
	while(1) {
      if (PIR==1) printf("PIR detected!\n");
      else        printf("PIR no detection!\n");			
    }
	
	//TEST SEVEN SEGMENT
	while(1) {
		i=ScanKey();
    Display_7seg(i);
	  }
	
		
	//TEST LED MERAH
	while(1) {
	  LED =0; // turn on LED
	  CLK_SysTickDelay(100000);	 // Delay 
	  LED =1; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		}
	
		
		
	
		
		
		
		
	//PROGRAM UTAMA
	while(1)
	{
		if(PIR == TRUE) 
		{
			detek = TRUE;
			LED 	= FALSE;  			//HIDUPKAN LED
			
			for(timer=30; timer>=0; i--)
			{
				input_pass();
				if(count_pass == 3) break;
				if(PIN_GETAR == TRUE)
				{
					detek = TRUE;
					break;
				}
				
				if(timer < 10) Display_7seg(timer);
				CLK_SysTickDelay(1000000);
			}
		}
		
		if(detek == TRUE) 
		{
			while(1)
			{
				Buzz(10);
				if(TOMBOL == FALSE) 
				{
					detek = FALSE;
					LED		= TRUE; 				//MATIKAN LED
					break;
				}
			}
		}

	}
}
