#include <msp430.h> 
#include <stdint.h>

/**
 * main.c
 */
#define     SMCLK_FREQ      1000000//1MHz sub-master clk

#define     POWER_ON_PIN    BIT3
#define     POWER_OFF_PIN   BIT4
#define     TRANSITOR_PIN   BIT5

#define     POWER_OFF_TIME  15//5 seconds

#define     USER_CCR0       49999//50000 - 1
#define     MAX_CCR0        65535//2^16 - 1

//#define     MAX_TIME_COUNT  (POWER_OFF_PIN * SMCLK_FREQ)\
//                            / ((USER_CCR0 < MAX_CCR0) ? USER_CCR0 : MAX_CCR0)
#define     POWER_OFF       2
#define     POWER_BEGIN     0
#define     POWER_TURN_ON   1
uint8_t state;
uint16_t MAX_TIME_COUNT;
volatile uint16_t time_count;

void Config_Clock(void);
void Config_IO(void);
void Config_Timer(void);
void Config_I2C(void);
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	Config_Clock();
	Config_IO();
	Config_I2C();
	Config_Timer();

	state = POWER_BEGIN;
	P1OUT |= TRANSITOR_PIN;

	__bis_SR_register(GIE);
	while(1)
	{

	}
	return 0;
}

void Config_Clock(void)
{
    DCOCTL = CALDCO_8MHZ;
    BCSCTL1 = CALBC1_8MHZ;
    BCSCTL1 |= XT2OFF;
    BCSCTL2 |= SELM_1 + DIVS_3;
    BCSCTL3 |= LFXT1S_2;
}

void Config_IO(void)
{

    P1DIR = P2DIR = 0xFF;//Set all as output
    P1OUT = P2OUT = 0x00;//Set default output as high

    P1DIR &= ~(POWER_ON_PIN + POWER_OFF_PIN);
    P1REN |= POWER_ON_PIN + POWER_OFF_PIN;
    P1OUT &= ~POWER_OFF_PIN;
    P1OUT &= ~POWER_ON_PIN;


    P1IES &=~POWER_OFF_PIN;
    P1IES &=~POWER_ON_PIN;
    P1IE |= POWER_ON_PIN + POWER_OFF_PIN;
}
void Config_Timer(void)
{
    CCTL0 |= CCIE;
    if(USER_CCR0 < MAX_CCR0)
        CCR0 = USER_CCR0;
    else
        CCR0 = MAX_CCR0;
    MAX_TIME_COUNT = (POWER_OFF_TIME * SMCLK_FREQ)\
                     / ((USER_CCR0 < MAX_CCR0) ? USER_CCR0 : MAX_CCR0);
    time_count = 0;
    TA0CTL |= TASSEL_2;//run SMCLK as 1MHz
}
void Config_I2C(void)
{

}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TimerA0_ISR(void)
{
    if( MAX_TIME_COUNT > time_count)
        time_count++;
    else
    {
        P1OUT &= ~TRANSITOR_PIN;// turn off the power
        time_count = 0;
        TA0CTL &= ~MC1;
        state = POWER_OFF;
    }
    //here software auto clear the interrupt flag CCIFG
}

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    int i;
    for(i = 0; i < 10000; i++);
    if(P1IFG & POWER_ON_PIN)
    {
        if((POWER_BEGIN == state) && (P1IN & POWER_ON_PIN))
        {
//            if(!(P1OUT & TRANSITOR_PIN))
//                P1OUT |= TRANSITOR_PIN;
//            else
//            {
//                //do nothing
//            }
            state = POWER_TURN_ON;
        }
        P1IFG &= ~POWER_ON_PIN;
    }
    else if(P1IFG & POWER_OFF_PIN)
    {
        //start timer here
        if((POWER_TURN_ON == state) && (P1IN & POWER_OFF_PIN))
        {
            if(P1OUT & TRANSITOR_PIN)
                TA0CTL |= MC1;//start timer to count to 5sec
            else
            {
              //do nothing
            }
        }

        P1IFG &= ~POWER_OFF_PIN;
    }
    else
    {
        //do nothing
    }

}
