#include <iostream> 
#include <vector> 
#include <fstream>
#include <string>
#include <bitset>
#include <iomanip>
#include <cstdlib>
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
    uint8_t stackPointer = 0; 
    uint8_t delayTimer; 
    uint8_t soundTimer; 
    uint16_t opcode; 
    bool running = true; 
    SDL_Window* window = NULL; 
    SDL_Renderer* renderer = NULL; 
    CHIP();
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

void CHIP::getNextInstruction() 
{ 
  opcode = memory[programCounter]; 
  opcode = opcode << 8; 
  opcode = opcode | memory[programCounter + 1]; 
  programCounter += 2; 
  //std::cout << std::hex << opcode << ' ';
}

void CHIP::decodeNextInstruction() 
{ 
  uint16_t firstNibble = opcode >> 12;
  //std::cout << "nibble equal to " << firstNibble << '\n'; 
  switch(firstNibble) 
  { 
    case 0x0: 
    { 
      uint8_t fourthNibble = opcode & 0xF; 
      if(fourthNibble == 0xE) 
        _00EE(); 
      else _00E0(); 
       break; 
    }
    case 0x1: 
    { 
      _1NNN(); 
      break;
    }
    case 0x2: 
    { 
      _2NNN(); 
      break;
    }
    case 0x3: 
    { 
      _3XKK(); 
      break;
    }
    case 0x4: 
    { 
      _4XKK(); 
      break;
    }
    case 0x5: 
    { 
      _5XY0(); 
      break;
    }
    case 0x6: 
    { 
      _6XKK(); 
      break;
    }
    case 0x7: 
    { 
      _7XKK();
      break;
    }
    case 0x8: 
    { 
      uint8_t fourthNibble = opcode & 0xF; 
      switch(fourthNibble) 
      { 
        case 0: 
        {
        _8XY0(); 
        break; 
        }
        case 1:
        { 
        _8XY1(); 
        break; 
        }
        case 2: 
        {
        _8XY2(); 
        break; 
        }
        case 3: 
        { 
        _8XY3(); 
          break; 
        }
        case 4: 
        { 
          _8XY4(); 
          break;
        }
        case 5: 
        { 
          _8XY5(); 
          break; 
        }
        case 6: 
        { 
          _8XY6(); 
          break; 
        }
        case 7: 
        { 
          _8XY7(); 
          break; 
        }
        case 0xE: 
        { 
          _8XYE();
          break;
        }
        default: 
        break;
      }
    }
    case 0x9: 
    { 
      _9XY0(); 
      break;
    }
    case 0xA: 
    { 
      _ANNN(); 
      break;
    }
    case 0xB: 
    { 
      _BNNN(); 
      break; 
    }
    case 0xC: 
    { 
      _CXKK(); 
      break;
    }
    case 0xD: 
    { 
      _DXYN(); 
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
void CHIP::_00EE() // sets PC to address at the top of the stack, then decrements SP by one. 
{ 
  --stackPointer;
  programCounter = stack[stackPointer];  
}
void CHIP::_00E0() //clears the screen
{ //std::cout << "called 00E0\n" ; 
  std::fill(display.begin(), display.end(), 0); 
}

void CHIP::_1NNN() // sets PC to nnn
{ //std::cout << "called 1NNN\n"; 
  programCounter = opcode & 0xFFF;
}

void CHIP::_2NNN() // increments stack pointer, then puts current PC at the top of the stack. PC then set to nnn
{ 
  stack[stackPointer] = programCounter; 
  stackPointer++; 
  programCounter = opcode & 0xFFF;
}
void CHIP::_3XKK() // skip next instruction if VX = kk 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t kk = opcode & 0xFF; 
  if(registers[x] == kk) 
  {
    programCounter += 2;
  }
}

void CHIP::_4XKK() // skip next instruction if VX != kk
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t kk = opcode & 0xFF; 
  if(registers[x] != kk) 
  {
    programCounter += 2;
  }
}
void CHIP::_5XY0() // skip next instruction if VX == VY
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t y =  (opcode & 0xF0) >> 4;
  if(registers[x] == registers[y]) 
  {
    programCounter += 2;
  }
}
void CHIP::_6XKK() //puts kk into register VX
{ //std::cout << "called 6XKK\n"; 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t kk = opcode & 0xFF; 
  registers[x] = kk; 
}
void CHIP::_7XKK() //adds kk to register VX
{ //std::cout << "called 7XKK\n"; 
  uint8_t x = (opcode & 0xF00) >> 8; 
  registers[x] += opcode & 0xFF;
}
void CHIP::_8XY0() //set VX = VY
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  registers[x] = registers[y]; 
}

void CHIP::_8XY1() // set VX |= VY; 
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  registers[x] |= registers[y]; 
}

void CHIP::_8XY2() // set VX &= VY;
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  registers[x] &= registers[y]; 
}

void CHIP::_8XY3() // set VX ^= VY;
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  registers[x] ^= registers[y]; 
}

void CHIP::_8XY4() // set VX += VY, if result bigger than 8 bits, VF = 1, otherwise 0, lowest 8 bits of result are kept. 
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  uint16_t result = registers[x] + registers[y]; 
  if(result > 255) 
    registers[0xF] = 1; 
  else registers[0xF] = 0;  
  registers[x] = result & 0xFF; 
}
void CHIP::_8XY5() // set VX -= VY, if VX > VY, VF = 1, otherwise 0;
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  if(registers[x] > registers[y]) 
    registers[0xF] = 1; 
  else registers[0xF] = 0; 
  registers[x] -= registers[y]; 
}
void CHIP::_8XY6() // VX >>= 1, if LSB of VX == 1, VF = 1, otherwise VF = 0
{
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t lsb = registers[x] & 1; 
  registers[0xF] = lsb; 
  registers[x] >>= 1; 
}
void CHIP::_8XY7() // set VX = VY - VX, if VY > VX, VF = 1, otherwise 0;
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  if(registers[y] > registers[x]) 
    registers[0xF] = 1; 
  else registers[0xF] = 0; 
  registers[x] = registers[y] - registers[x]; 
}
void CHIP::_8XYE() // VX >>= 1, if MSB of VX == 1, VF = 1, otherwise VF = 0
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t msb = registers[x] & 0x80; 
  registers[0xF] = msb; 
  registers[x] <<= 1; 
}
void CHIP::_9XY0() // skip next instruction if VX != VY
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t y = (opcode & 0xF0) >> 4;
  if(registers[x] != registers[y]) programCounter += 2;
}

void CHIP::_ANNN() //register I is set to nnn
{ //std::cout << "called ANNN\n"; 
  uint16_t nnn = opcode & 0xFFF; 
  indexRegister = nnn;
  //std::cout << std::hex << opcode << '\n'; 
  //std::cout << std::hex << nnn << '\n' << '\n';  
}
void CHIP::_BNNN() // PC = nnn + V0
{ 
  uint16_t nnn = opcode & 0xFFF;
  programCounter = nnn + registers[0]; 
}
void CHIP::_CXKK() // VX = random byte & kk
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t kk = opcode & 0xFF; 
  uint8_t byte = rand() % 256; 
  registers[x] = byte & kk;
}
void CHIP::_DXYN() 
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
  srand((unsigned) time(NULL)); 
  CHIP emulator; 
  emulator.loadROM("flags.ch8"); 
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
        emulator.getNextInstruction(); 
        //std::cout << std::hex << opcode << '\n';  
        emulator.decodeNextInstruction(); 
      }
      emulator.updateDisplay(); 
  }
  return 0;
} 

