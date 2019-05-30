#include <p18F4520.h>
#include <adc.h>
#include <delays.h>
#include <stdio.h>

#include "lcd_4b.h"

unsigned long res_ad;
unsigned long tensao;
char buf_lcd[16];
unsigned long res_temp;


// Frequencia do oscilador = 8MHz
// Ciclo de máquina  = 1/(Fosc/4) = 0,5us
#pragma config OSC = HS        	//Configura o oscilador a cristal 
#pragma config WDT = OFF  		//Desabilita o Watchdog Timer (WDT).
#pragma config PWRT = ON   		//Habilita o Power-up Timer (PWRT).
#pragma config BOREN = ON		//Habilita Brown-out reset 
#pragma config BORV = 1        	//Tensão do BOR é 4,33V.
#pragma config PBADEN = OFF 	//RB0,1,2,3 e 4 configurado como I/O digital.
#pragma config LVP = OFF       	//Desabilita o Low Voltage Program.

void main()
{

ADCON2 = 0b10001010;/*  			ADFM1 = 1 -> Resultado da conversão AD
                                    			... justificado à direita
                                                 -
                                                 ****Velocidade de aquisição em 8TAD
                                                 ACQT2 = 1
                                                 ACQT1 = 0
                                                 ACQT0 = 0
                                                 ****Fonte de clock em Fosc/8
                                                 ADCS2 = 0
                                                 ADCS1 = 0
                                                 ADCS0 = 1*/

ADCON1 = 0b00001011;/*                                               	-
                                               	VCFG1 = 0 -> Vref- = terra
                                               	VCFG0 = 1 -> Vref+ = VDD
                                               	*****Seleciona os canais AN0 até AN3 como analógicos
                                               	PCFG3 = 1
                                               	PCFG2 = 1
                                               	PCFG1 = 0
                                               	PCFG0 = 1*/
ADCON0 = 0b00000001;		/* Seleciona Canal 0 para conversão e habilita o conversor AD */

TRISAbits.TRISA0 = 1;
TRISAbits.TRISA1 = 1;
TRISAbits.TRISA3 = 1;
TRISB=0xff;


lcd_inicio();
Delay10KTCYx(40);  //200ms
Delay10KTCYx(40);  //200ms

	lcd_posicao(1,1);
			   //12345678901234567890
  	lcd_escreve("LAB AUTOMAÇÃO   ");

Delay10KTCYx(200);  //200ms
Delay10KTCYx(200);  //200ms
Delay10KTCYx(200);  //200ms
Delay10KTCYx(200);  //200ms
Delay10KTCYx(200);  //200ms

		  		
while(1)
		{
           SetChanADC(ADC_CH0);
		Delay10KTCYx(40);  //200ms

		ConvertADC();
		while(BusyADC());
		res_ad = ReadADC();

		tensao = (res_ad*5000) / 1023;

		sprintf(buf_lcd,"AN0 = %04ld mv",tensao);
           lcd_posicao(1,1);
           lcd_escreve_bf(buf_lcd);



        SetChanADC(ADC_CH1);
		Delay10KTCYx(40);  //200ms
		ConvertADC();
		while(BusyADC());
		res_temp = ReadADC();

		tensao = (res_temp*5000) / 1023;

		sprintf(buf_lcd,"AN1 = %04ld mv",tensao);
           lcd_posicao(2,1);
           lcd_escreve_bf(buf_lcd);




    		}         

}