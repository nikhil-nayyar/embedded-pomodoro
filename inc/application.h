#ifndef APPLICATION_H
#define APPLICATION_H

#include "display.h"

enum MenuState { Firmware, Session, Time, Options, Short, Long, Pomo};

typedef struct{
    int8_t time;
    int8_t led;
} Event;

typedef struct {
    uint32_t period_long;
    uint32_t period_short;
    uint32_t pomodoro;
    float version;
} OptionState;

typedef struct {
    uint8_t session;
    uint32_t time_remaining;
} SessionState;

typedef struct{
    uint8_t menu_state;
} DisplayState;

typedef struct {
    OptionState options;
    SessionState application;
    DisplayState display;
} Application;

void initialize_application();

/**
 * initialize Timer
*/
void initialize_wtimer1a();

/**
 * start timer with given count
*/
void start_timer(uint32_t mins);

#endif