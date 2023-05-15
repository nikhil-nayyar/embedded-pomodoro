
#include "application.h"
#include "array.h"
#include "display.h"
#include "tm4c123gh6pm.h"


Event event;

static Application application;

volatile static uint32_t timer1a_count;
volatile static uint32_t matrix_limit;
volatile static uint32_t matrix_count;

void initialize_application(){

    /* Initialize Application Logic */
    application = (Application) {
        (OptionState) {
            15,
            5,
            25,
            0.1,
        },
        (SessionState){
            0,
            0,
        },
        (DisplayState){
            0
        }
    };

    event = (Event) {0, 0}; // register no events at beginning

}

void initialize_wtimer1a(){

    /* Configure System */
    SYSCTL_RCGCWTIMER_R |= 0x2; // Enable Timer 1 Clock

    /* Configure Timer */
    WTIMER1_CTL_R &= ~(TIMER_CTL_TAEN); // disable timer
    WTIMER1_CTL_R |= TIMER_CTL_TASTALL; // disable timer while debugging
    WTIMER1_CFG_R = TIMER_CFG_16_BIT; // Set at 32 bit timer
    WTIMER1_TAILR_R = 79999999;
    WTIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // set timer to fire periodically
    WTIMER1_IMR_R = TIMER_IMR_TATOIM; // set interrupt to trigger on end of count

    /* Configure NVIC */
    // vector 112 int 96
    NVIC_EN3_R |= (0x1 << (96-96)); // 
    NVIC_PRI24_R |= ((NVIC_PRI24_INTA_M&0x3) << NVIC_PRI24_INTA_S); // set priority to 3

 }

/* Fire every 1 second */
void WTimer1A_Handler(void){

    timer1a_count--;
    matrix_count++;

    if(timer1a_count == 0){
        WTIMER1_CTL_R &= ~(TIMER_CTL_TAEN); // disable timer
    }

    if(matrix_count == matrix_limit){
        matrix_count = 0;
        event.led = -1;
    }

    WTIMER1_ICR_R = TIMER_ICR_TATOCINT; // clear flag

}

void start_timer(uint32_t mins){
    timer1a_count = mins*60;

    matrix_limit = ((uint32_t) timer1a_count / 32)+1;
    matrix_count = 0;

    WTIMER1_TAILR_R = 79999999; // load timer value
    WTIMER1_CTL_R |= (TIMER_CTL_TAEN); // enable timer
}