#include "../src/CHIP.h"
#include <string>
#include <iostream>
int main(int argc, char** argv) 
{ 
  srand((unsigned) time(NULL)); 
  int SCREEN_HEIGHT = 1280; 
  int SCREEN_WIDTH = 720;
  if(argc < 2) 
  { 
    std::cout << "Proper usage is: ./emulator ROM_NAME SCREEN_WIDTH SCREEN_HEIGHT - the last two arguments are optional, defaults are 1280 and 720.\n"; 
    return 0; 
  }
  if(argc >= 4) 
  { 
    SCREEN_HEIGHT = std::stoi(argv[2]); 
    SCREEN_WIDTH = std::stoi(argv[3]); 
  }
  CHIP emulator(SCREEN_HEIGHT, SCREEN_WIDTH);  
  emulator.loadROM(argv[1]); 
  emulator.loop();
  return 0;
} 

