#include <CHIP.h> 
#include <iostream>
#include <fstream>
#include <iomanip> 

CHIP::CHIP(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) 
{ 
  PIXEL_WIDTH = SCREEN_WIDTH/64;
  PIXEL_HEIGHT = SCREEN_HEIGHT/32;
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

void CHIP::loop() 
{ 
    while(running) 
  { 
    SDL_Event event; 
    while(SDL_PollEvent(&event) != 0) 
    {   
        if(event.type == SDL_QUIT)
        { 
            return;
        }
        else if(event.type == SDL_KEYDOWN) 
        { 
          switch(event.key.keysym.sym) 
          { 
            case SDLK_1: 
            keypad[1] = 1;
            break; 
            case SDLK_2: 
            keypad[2] = 1; 
            break; 
            case SDLK_3: 
            keypad[3] = 1; 
            break; 
            case SDLK_4: 
            keypad[0xC] = 1; 
            break; 
            case SDLK_q: 
            keypad[4] = 1; 
            break; 
            case SDLK_w: 
            keypad[5] = 1; 
            break; 
            case SDLK_e: 
            keypad[6] = 1;
            break; 
            case SDLK_r: 
            keypad[0xD] = 1; 
            break;
            case SDLK_a: 
            keypad[7] = 1; 
            break; 
            case SDLK_s: 
            keypad[8] = 1; 
            break; 
            case SDLK_d: 
            keypad[9] = 1; 
            break;
            case SDLK_f: 
            keypad[0xE] = 1; 
            break; 
            case SDLK_z:
            keypad[0xA] = 1; 
            break; 
            case SDLK_x: 
            keypad[0] = 1; 
            break; 
            case SDLK_c: 
            keypad[0xB] = 1; 
            break;
            case SDLK_v: 
            keypad[0xF] = 1; 
            break;
            default: 
            break;
          }
        }
        else if(event.type == SDL_KEYUP) 
        { 
          switch(event.key.keysym.sym) 
          { 
            case SDLK_1: 
            keypad[1] = 0;
            break; 
            case SDLK_2: 
            keypad[2] = 0; 
            break; 
            case SDLK_3: 
            keypad[3] = 0; 
            break; 
            case SDLK_4: 
            keypad[0xC] = 0; 
            break; 
            case SDLK_q: 
            keypad[4] = 0; 
            break; 
            case SDLK_w: 
            keypad[5] = 0; 
            break; 
            case SDLK_e: 
            keypad[6] = 0;
            break; 
            case SDLK_r: 
            keypad[0xD] = 0; 
            break;
            case SDLK_a: 
            keypad[7] = 0; 
            break; 
            case SDLK_s: 
            keypad[8] = 0; 
            break; 
            case SDLK_d: 
            keypad[9] = 0; 
            break;
            case SDLK_f: 
            keypad[0xE] = 0; 
            break; 
            case SDLK_z:
            keypad[0xA] = 0; 
            break; 
            case SDLK_x: 
            keypad[0] = 0; 
            break; 
            case SDLK_c: 
            keypad[0xB] = 0; 
            break;
            case SDLK_v: 
            keypad[0xF] = 0; 
            break;
            default: 
            break;
          }
        }
    } 
      if(programCounter <= 0xFFF) 
      {
        getNextInstruction(); 
        //std::cout << std::hex << opcode << '\n';  
        decodeNextInstruction(); 
      }
      updateDisplay(); 
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
    case 0xE: 
    { 
      uint8_t fourthNibble = opcode & 0xF;
      if(fourthNibble == 0xE) 
        _EX9E(); 
      else _EXA1();
    }
  }
}

void CHIP::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    uint8_t data;
    size_t i = 0x200;
    while (i < memory.size() && file.read(reinterpret_cast<char*>(&data), sizeof(data))) 
    {
        memory[i] = data;
        ++i;
    }
}

void CHIP::printMemory() 
{ 
    for(size_t i = 0x200; i < memory.size(); i += 2) 
    { 
      uint16_t res = 0;
      res = memory[i]; 
      res = res << 8; 
      res = res | memory[i + 1]; 
      std::cout << std::hex << std::setw(4) << std::setfill('0') << res << " "; 
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
