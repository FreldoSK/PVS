#pragma once
#include "mbed.h"
#include "Uart.h"
#include <cstdint>
#include <memory>
#include <string>


struct Details {
    unsigned short numberOfHeroes;  
    uint8_t capacityOfTable; 
    uint8_t heroTime;
    uint8_t shopTime;
};

class Joystick {
private:
    DigitalIn joyUp, joyDown, joyRight, joyLeft;
    uint8_t counter, response;  
    bool skiped;


public:
    Joystick() : counter(0), skiped(false), response(0), joyUp(BUTTON4), joyDown(BUTTON5), 
    joyRight(BUTTON3), joyLeft(BUTTON2) {
        joyUp.mode(PullDown);
        joyDown.mode(PullDown);
        joyRight.mode(PullDown);
        joyLeft.mode(PullDown);
    }

    bool getSkiped();
    uint8_t getResponse();
    void setResponse(const uint8_t& response);
    void joyPressed(std::shared_ptr<Uart>& uart, const std::shared_ptr<Details>& details);
    uint16_t getCounter();
    void setCounter(const uint16_t& counter); 
};
