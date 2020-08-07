#include <p18F4520.h>
#include <adc.h>
#include <delays.h>
#include <stdio.h>

#include "lcd_4b.h"

unsigned float res_ad;
unsigned float tensao;
char buf_lcd[16];
char buf_lcd_b[16];
char buf_lcd1[16];
unsigned long res_temp;

float KP;
int KPi;

float KI;
int KIi;

float KD;
int KDi;

unsigned float Ton;
unsigned float Toff;

float vm;
int vmi;

float stp;
int stpi;

float erro;
int erroi;

float I=0;
int Ii;

float AP;
int APi;

float AD;
int ADi;

float AI;
int AIi;

int c;

float temp=0;
int tempi;

float temp1=0;
int tempi1;

float temp2=0;
int tempi2;

float temp_old;

int b=0;

// Frequencia do oscilador = 8MHz
// Ciclo de m�quina  = 1/(Fosc/4) = 0,5us
#pragma config OSC = HS        	//Configura o oscilador a cristal 
#pragma config WDT = OFF  		//Desabilita o Watchdog Timer (WDT).
#pragma config PWRT = ON   		//Habilita o Power-up Timer (PWRT).
#pragma config BOREN = ON		//Habilita Brown-out reset 
#pragma config BORV = 1        	//Tens�o do BOR � 4,33V.
#pragma config PBADEN = OFF 	//RB0,1,2,3 e 4 configurado como I/O digital.
#pragma config LVP = OFF       	//Desabilita o Low Voltage Program.


void pwm(){
	
	if(vm < 1){
		vm=1;
	}
	if(vm >99){
		vm = 99;
	}
	Ton = 2*vm;
	Toff = 200 - Ton;			// Liga a ventoinha E LED PWM	
    PORTCbits.RC1=1;
	Delay10KTCYx(Ton);
    PORTCbits.RC1=0;
	Delay10KTCYx(Toff);
}

void main(){


OpenADC(ADC_FOSC_8
		  & ADC_RIGHT_JUST
		  & ADC_4_TAD,
		  ADC_CH0
		  & ADC_INT_OFF
		  & ADC_VREFPLUS_VDD 
		  & ADC_VREFMINUS_VSS,
		   ADC_2ANA);		

	  
PORTCbits.RC1=1;
Delay10KTCYx(40);
PORTCbits.RC1=0;


TRISB=0xff;					//CONFIGURA  PORTA B COMO ENTRADA
TRISCbits.TRISC2 = 0;		//CONFIGURA  PORTA C COMO SA�DA
TRISCbits.TRISC1 = 0;
//ADCON1 = 0x0D;
 
TRISAbits.TRISA0 = 1;
TRISAbits.TRISA1 = 1;
TRISAbits.TRISA3 = 1;

TRISCbits.TRISC0 = 1;

KP=0.2;
KI=0.2;
KD=0.2;

//ADCON0 = 0b00000001; 
//ADCON1 = 0b00001011;
//ADCON2 = 0b10001010; 

lcd_inicio();
temp_old=0;
c=0;
//AP=0;AD=0;AI=0;

while(1){
//prova

while(PORTCbits.RC0 == 1 ){

		KDi=(int)KD;
		KIi=(int)KI;
		KPi=(int)KP;

		sprintf(buf_lcd,"kp=%03i C ki=%03i C",KPi,KIi);
 		lcd_posicao(1,1);
        lcd_escreve_bf(buf_lcd);
		sprintf(buf_lcd,"kd=%03i C ",KDi);
 		lcd_posicao(2,1);
        lcd_escreve_bf(buf_lcd);

if(PORTBbits.RB0 == 0){
		KP+=0.5;
		if(KP>10) KP=0.2;
}
if(PORTBbits.RB1 == 0){
		KI+=0.5;
		if(KI>10) KI=0.2;
}
if(PORTBbits.RB2 == 0){
		KD+=0.5;
		if(KD>10) KD=0.2;
}

		Delay10KTCYx(200);
}

//endprova


if (PORTBbits.RB1 == 0)	//Se o bot�o for pressionado (se B0 for igual a '0')
  c++; // variavel de selecao de menu
if(c>2){c=0;}
    


		//PORTCbits.RC2=1; //AQUECEDOR
		SetChanADC(ADC_CH3);
		Delay10KTCYx(40);
		ConvertADC();
		while(BusyADC());
		res_ad = ReadADC();
		temp2=temp1;
		temp1=temp;

		temp = (res_ad*500) / 1023;
		tempi=(int)temp;



		SetChanADC(ADC_CH0);
		Delay10KTCYx(40);
		ConvertADC();
		while(BusyADC());
		res_temp = ReadADC();
		stp=(res_temp*100)/1023;
		
		if(PORTBbits.RB0==0 && b==0) b=1;
		else if(PORTBbits.RB0 == 0 && b==1) b=0;
					
		stp+=b*5;

		stpi=(int)stp;

		AD=(temp-temp2)*KD;               //Determinacao do AD 
        ADi=(int)AD;

	//	temp_old=temp;
		
		erro=stp-temp;
		erroi=(int)erro;

		AP=KP*erro;               			//Determinacao do AP 
		APi=(int) AP;
		I=I+erro;
		
		if (AI>100)
			AI = 100;
	
		if(AI<-50)
			AI= -50;
		

		AI=(KI*erro)+AI;               				//Determinacao do AI           PROVA
		AIi=(int)AI;
		
//-------------------------------------------------------------------

		vm=AP+AI+AD;
		vmi=(int)vm;
		pwm();

if (PORTBbits.RB2 == 0 || erro<-5)
	PORTCbits.RC2=1;
else PORTCbits.RC2=0;



switch(c){
case 0:

		sprintf(buf_lcd,"s=%03i C t=%03i C",stpi,tempi);
 		lcd_posicao(1,1);
        lcd_escreve_bf(buf_lcd);

	//	sprintf(buf_lcd,"t=%03i C",tempi);
 	//	lcd_posicao(1,9);
    //    lcd_escreve_bf(buf_lcd);

		sprintf(buf_lcd_b,"err=%03i C;vm=%03i C",erroi, vmi);
        lcd_posicao(2,1);
        lcd_escreve_bf(buf_lcd_b);
break;

case 1:
		sprintf(buf_lcd,"P=%03i I=%03i D=%03i",APi,AIi,ADi);
 		lcd_posicao(1,1);
        lcd_escreve_bf(buf_lcd);

		sprintf(buf_lcd_b,"err=%03i C;vm=%03i C",erroi, vmi);
        lcd_posicao(2,1);
        lcd_escreve_bf(buf_lcd_b);
		break;

case 2:
		sprintf(buf_lcd,"s=%03i C t=%03i C",stpi,tempi);
 		lcd_posicao(1,1);
        lcd_escreve_bf(buf_lcd);


		sprintf(buf_lcd_b,"err=%03i% %;vm=%03i %",erroi/stpi, (vmi-stpi)/stpi);
        lcd_posicao(2,1);
        lcd_escreve_bf(buf_lcd_b);
		break;

	
}

    		}  
}