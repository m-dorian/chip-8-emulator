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
    std::vector<uint8_t> registers = std::vector<uint8_t>(16); 
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
    void decodeNextInstruction(uint16_t opcode); 
    void loadROM(const std::string &filename);
    void printMemory(); 
    //Instructions 
    void CLS();
    void JP(const uint16_t opcode); 
    void LDVX_byte(const uint16_t opcode);
    void ADDVX_byte(const uint16_t opcode); 
    void LD_I(const uint16_t opcode); 
    void DXYN(const uint16_t opcode); 
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

void CHIP::decodeNextInstruction(uint16_t opcode) 
{ 
  uint16_t firstNibble = opcode >> 12;
  switch(firstNibble) 
  { 
    case 0x0: 
    { 
      if(opcode == 0x00E0) 
      { 
        CLS(); 
        break;
      }
      else break; 
    }
    case 0x1: 
    { 
      JP(opcode); 
      break;
    }
    case 0x6: 
    { 
      LDVX_byte(opcode); 
      break;
    }
    case 0x7: 
    { 
      ADDVX_byte(opcode);
      break;
    }
    case 0xA: 
    { 
      LD_I(opcode); 
      break;
    }
    case 0xD: 
    { 
      DXYN(opcode); 
      break;
    }
  }
}

void CHIP::CLS() //00E0, clears the screen
{ 
  std::fill(display.begin(), display.end(), 0); 
}

void CHIP::JP(const uint16_t opcode) //1nnn, sets PC to nnn
{ 
  programCounter = opcode & 0xFFF;
}

void CHIP::LDVX_byte(const uint16_t opcode) //6xkk, puts kk into register VX
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t kk = opcode & 0xFF; 
  registers[x] = kk; 
}
void CHIP::ADDVX_byte(const uint16_t opcode) //7xkk, adds kk to register VX
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  registers[x] += opcode & 0xFF;
}
void CHIP::LD_I(const uint16_t opcode) //Annn, register I is set to nnn
{ 
  indexRegister = opcode & 0xFFF; 
}
void CHIP::DXYN(const uint16_t opcode) 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t y = (opcode & 0xF0) >> 4;
  int coordX = registers[x] % 64; 
  int coordY = registers[y] % 32; 
  uint16_t N = opcode & 0xF;
  registers[0xF] = 0; 
  for(int i = 0; i < N; i++) 
  { 
    uint8_t spriteRow = memory[indexRegister + i]; 
    for(int j = 0; j < 8; j++) 
    { 
      if(coordX + j < 64 && coordY + i < 32)
      { 
        uint8_t screenPixel = display[(i * 64) + j]
      }
    }
  }
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
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); 
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
  /*uint8_t first = opcode >> 12;
  uint8_t second = (opcode & 0xF00) >> 8;
  uint8_t third = (opcode & 0xF0) >> 4;
  uint8_t fourth = opcode & 0xF; */
  while(emulator.running) 
  { 
    SDL_SetRenderDrawColor(emulator.renderer, 0, 0, 0, 0 );
    SDL_RenderClear(emulator.renderer);
    SDL_Event event; 
    while(SDL_PollEvent(&event) != 0) 
    {   
        if(event.type == SDL_QUIT)
        { 
            return false;
        }
    } 
      uint16_t opcode = emulator.getNextInstruction(); 
      emulator.decodeNextInstruction(opcode); 
    
  }
} 

