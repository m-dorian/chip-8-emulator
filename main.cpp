#include <iostream> 
#include <vector> 
#include <fstream>
#include <string>
#include <bitset>
#include <iomanip>
#include <SDL2/SDL.h>
const unsigned int START = 0x200;
const unsigned int SCREEN_HEIGHT = 720; 
const unsigned int SCREEN_WIDTH = 1280; 
class CHIP { 
    public: 
    std::vector<uint8_t> registers; 
    std::vector<uint8_t> memory = std::vector<uint8_t>(4096, 0); 
    uint16_t indexRegister; 
    uint16_t programCounter = START; 
    std::vector<uint16_t> stack = std::vector<uint16_t>(16); 
    std::vector<uint8_t> display = std::vector<uint8_t>(64 * 32); 
    uint8_t stackPointer; 
    uint8_t delayTimer; 
    uint8_t soundTimer; 
    bool running = true; 
    SDL_Window* window = NULL; 
    SDL_Renderer* renderer = NULL; 
    CHIP();
    uint16_t getNextInstruction();
    void loadROM(const std::string &filename);
    void printMemory(); 
};

void CHIP::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    uint8_t data;
    size_t i = START;
    while (i < memory.size() && file.read(reinterpret_cast<char*>(&data), sizeof(data))) 
    {
        memory[i] = data;
        ++i;
    }

    file.close();
}
void CHIP::printMemory() 
{ 
    for(size_t i = START; i < memory.size(); i += 2) 
    { 
      uint16_t res = 0;
      res = memory[i]; 
      res = res << 8; 
      res = res | memory[i + 1]; 
      std::cout << std::hex << std::setw(4) << std::setfill('0') << res << " "; 
    }
}
uint16_t CHIP::getNextInstruction() 
{ 
  uint16_t opcode = 0;
  opcode = memory[programCounter + 2]; 
  opcode = opcode << 8; 
  opcode = opcode | memory[programCounter + 1]; 
  programCounter += 2; 
  return opcode;
}
CHIP::CHIP() 
{ 
  const unsigned int FONT_START = 0x50;
  std::vector<uint8_t> font = { 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
  for(int i = 0; i < font.size(); i++) 
  { 
    memory[i + FONT_START] = font[i]; 
  }
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) 
    {
        std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl; 
        
    }
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIDDEN); 
    if(window == 0) 
    { 
        std::cout << "Error creating window: " << SDL_GetError() << std::endl; 
        
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == 0) 
    {  
        std::cout << "Error creating renderer: " << SDL_GetError() << std::endl; 
    }
}
int main() 
{ 
  CHIP emulator; 
  emulator.loadROM("IBM Logo.ch8"); 
  uint16_t opcode = 0;
  opcode = emulator.memory[0x208 + 4]; 
  opcode = opcode << 8; 
  opcode = opcode | emulator.memory[0x208 + 5]; 
  uint8_t first = opcode >> 12;
  uint8_t second = (opcode & 0xF00) >> 8;
  uint8_t third = (opcode & 0xF0) >> 4;
  uint8_t fourth = opcode & 0xF;
} 

