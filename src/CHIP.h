#pragma once
#ifndef CHIP_h
#define CHIP_h

#include <vector>
#include <SDL2/SDL.h>
#include <string>

class CHIP { 
    public: 
    std::vector<uint8_t> registers = std::vector<uint8_t>(16); 
    std::vector<uint8_t> memory = std::vector<uint8_t>(4096, 0); 
    uint16_t indexRegister; 
    uint16_t programCounter = 0x200; // Adddress at which ROM is loaded 
    std::vector<uint16_t> stack = std::vector<uint16_t>(16); 
    std::vector<uint8_t> display = std::vector<uint8_t>(64 * 32); 
    std::vector<uint8_t> keypad = std::vector<uint8_t>(16, 0); 
    uint8_t stackPointer = 0; 
    uint8_t delayTimer; 
    uint8_t soundTimer; 
    uint16_t opcode; 
    bool running = true; 
    SDL_Window* window = NULL; 
    SDL_Renderer* renderer = NULL; 
    int PIXEL_WIDTH; 
    int PIXEL_HEIGHT; 
    //Core
    CHIP(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
    void loop(); 
    void getNextInstruction();
    void decodeNextInstruction(); 
    void updateDisplay(); 
    void loadROM(const std::string &filename);
    void printMemory(); 
    //Instructions 
    void _00EE(); 
    void _00E0();
    void _1NNN(); 
    void _2NNN(); 
    void _3XKK(); 
    void _4XKK(); 
    void _5XY0(); 
    void _6XKK();
    void _7XKK(); 
    void _8XY0(); 
    void _8XY1(); 
    void _8XY2(); 
    void _8XY3(); 
    void _8XY4(); 
    void _8XY5(); 
    void _8XY6();
    void _8XY7(); 
    void _8XYE(); 
    void _9XY0(); 
    void _ANNN(); 
    void _BNNN(); 
    void _CXKK(); 
    void _DXYN(); 
    void _EX9E(); 
    void _EXA1();
};

#endif