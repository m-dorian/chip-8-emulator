#include "CHIP.h"

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
void CHIP::_EX9E() // If key corresponding to the value of VX is pressed, skip next instruction. 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t key = registers[x]; 
  if(keypad[key]) programCounter += 2; 
}
void CHIP::_EXA1() // If key corresponding to the value of VX is not pressed, skip next instruction. 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t key = registers[x]; 
  if(!keypad[key]) programCounter += 2;
}
void CHIP::_FX07() 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  registers[x] = delayTimer;  
}
void CHIP::_FX0A() 
{ 
    SDL_Event event; 
    uint8_t x = (opcode & 0xF00) >> 8;
    while(SDL_WaitEvent(&event) != 0) 
    {   
         if(event.type == SDL_KEYDOWN) 
        { 
          switch(event.key.keysym.sym) 
          { 
            case SDLK_1: 
            registers[x] = 1;
            break; 
            case SDLK_2: 
            registers[x] = 2; 
            break; 
            case SDLK_3: 
            registers[x] = 3; 
            break; 
            case SDLK_4: 
            registers[x] = 0xC; 
            break; 
            case SDLK_q: 
            registers[x] = 4; 
            break; 
            case SDLK_w: 
            registers[x] = 5; 
            break; 
            case SDLK_e: 
            registers[x] = 6;
            break; 
            case SDLK_r: 
            registers[x] = 0xD; 
            break;
            case SDLK_a: 
            registers[x] = 7; 
            break; 
            case SDLK_s: 
            registers[x] = 8; 
            break; 
            case SDLK_d: 
            registers[x] = 9; 
            break;
            case SDLK_f: 
            registers[x] = 0xE; 
            break; 
            case SDLK_z:
            registers[x] = 0xA; 
            break; 
            case SDLK_x: 
            registers[x] = 0; 
            break; 
            case SDLK_c: 
            registers[x] = 0xB; 
            break;
            case SDLK_v: 
            registers[x] = 0xF; 
            break;
            default: 
            break;
          }
        } 
}
}
void CHIP::_FX15() 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  delayTimer = registers[x];   
}
void CHIP::_FX18() 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  soundTimer = registers[x];  
}
void CHIP::_FX1E() 
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  indexRegister += registers[x];
}
void CHIP::_FX29() // I = location of font sprite corresponding to value of VX
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t value = registers[x];
  indexRegister = 0x50 + (value * 5); //0x50 is the location at which the font is loaded into memory, each font sprite is 5 bytes long
}
void CHIP::_FX33() //Take the decimal value of VX and store the ones digit in I + 3, tens in I + 2 and hundreds in I.  
{ 
  uint8_t x = (opcode & 0xF00) >> 8; 
  uint8_t val = registers[x]; 
  memory[indexRegister] = val/100; 
  memory[indexRegister + 1] = (val/10) % 10; 
  memory[indexRegister + 2] = val % 10; 
}
void CHIP::_FX55() //Store V0 through VX into memory, starting at I
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  for(int i = 0; i <= x; i++) 
  { 
    memory[indexRegister + i] = registers[i];
  }
} 
void CHIP::_FX65() //Read into V0 through VX from memory, starting at I
{ 
  uint8_t x = (opcode & 0xF00) >> 8;
  for(int i = 0; i <= x; i++) 
  { 
    registers[i] = memory[indexRegister + i];
  }
}