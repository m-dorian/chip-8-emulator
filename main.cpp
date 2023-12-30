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
const int PIXEL_WIDTH = SCREEN_WIDTH/64; 
const int PIXEL_HEIGHT = SCREEN_HEIGHT/32;

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
    void updateDisplay(); 
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
  uint16_t opcode;
  opcode = memory[programCounter]; 
  opcode = opcode << 8; 
  opcode = opcode | memory[programCounter + 1]; 
  programCounter += 2; 
  //std::cout << std::hex << opcode << ' ';
  return opcode;
}

void CHIP::decodeNextInstruction(uint16_t opcode) 
{ 
  uint16_t firstNibble = opcode >> 12;
  //std::cout << "nibble equal to " << firstNibble << '\n'; 
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

void CHIP::updateDisplay() 
{ 
  SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
  SDL_RenderClear(renderer);
  for(int i = 0; i < 64; i++) 
  {
    for(int j = 0; j < 32; j++) 
    { 
      SDL_Rect pixel = {i * PIXEL_WIDTH, j * PIXEL_HEIGHT, PIXEL_WIDTH, PIXEL_HEIGHT}; 
      SDL_SetRenderDrawColor(renderer, display[j * 64 + i] * 255, display[j * 64 + i] * 255, display[j * 64 + i] * 255, 255);
      SDL_RenderFillRect(renderer, &pixel);
    }
  }
  SDL_RenderPresent(renderer); 
}
void CHIP::CLS() //00E0, clears the screen
{ //std::cout << "called 00E0\n" ; 
  std::fill(display.begin(), display.end(), 0); 
}

void CHIP::JP(const uint16_t opcode) //1nnn, sets PC to nnn
{ //std::cout << "called 1NNN\n"; 
  programCounter = opcode & 0xFFF;
}

void CHIP::LDVX_byte(const uint16_t opcode) //6xkk, puts kk into register VX
{ //std::cout << "called 6XKK\n"; 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t kk = opcode & 0xFF; 
  registers[x] = kk; 
}
void CHIP::ADDVX_byte(const uint16_t opcode) //7xkk, adds kk to register VX
{ //std::cout << "called 7XKK\n"; 
  uint8_t x = (opcode & 0xF00) >> 8; 
  registers[x] += opcode & 0xFF;
}
void CHIP::LD_I(const uint16_t opcode) //Annn, register I is set to nnn
{ //std::cout << "called ANNN\n"; 
  uint16_t nnn = opcode & 0xFFF; 
  indexRegister = nnn;
  //std::cout << std::hex << opcode << '\n'; 
  //std::cout << std::hex << nnn << '\n' << '\n';  
}
void CHIP::DXYN(const uint16_t opcode) 
{ 
  //std::cout << "called DXYN\n"; 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t y = (opcode & 0xF0) >> 4;
  int coordX = registers[x] % 64; 
  int coordY = registers[y] % 32; 
  //std::cout << "coordX is " << coordX << '\n' << "coordY is " << coordY << '\n'; 
  uint16_t N = opcode & 0xF;
  registers[0xF] = 0; 
  //std::cout << "before update\n";
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 64; j++) {
        //std::cout << static_cast<int>(display[i * 64 + j]) << " ";
    }
    //std::cout << '\n';
}
  for(int i = 0; i < N; ++i) 
  { 
    uint8_t spriteRow = memory[indexRegister + i]; 
    //std::cout << std::bitset<8>(spriteRow) << '\n'; 
    for(int j = 0; j < 8; ++j) 
    { 
      if(coordX + j < 64 && coordY + i < 32)
      { 
        int id = ((i + coordY) * 64) + j + coordX;
        uint8_t screenPixel = display[id];
        uint8_t spritePixel = spriteRow >> (7 - j) & 0x1; 
         if (spritePixel != 0) 
         {
          if(screenPixel != 0) registers[0xF] = 1; 
          display[id] ^= 1; 
         }
      }
    }
  }
  //std::cout << "after update\n";
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 64; j++) {
        //std::cout << static_cast<int>(display[i * 64 + j]) << " ";
    }
    //std::cout << '\n';
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
  emulator.loadROM("1-chip8-logo.ch8"); 
  /*uint8_t first = opcode >> 12;
  uint8_t second = (opcode & 0xF00) >> 8;
  uint8_t third = (opcode & 0xF0) >> 4;
  uint8_t fourth = opcode & 0xF; */
  while(emulator.running) 
  { 
    SDL_Event event; 
    while(SDL_PollEvent(&event) != 0) 
    {   
        if(event.type == SDL_QUIT)
        { 
            return false;
        }
    } 
      if(emulator.programCounter <= 0xFFF) 
      {
        uint16_t opcode = emulator.getNextInstruction(); 
        //std::cout << std::hex << opcode << '\n';  
        emulator.decodeNextInstruction(opcode); 
      }
      emulator.updateDisplay(); 
  }
  return 0;
} 

