#include <iostream> 
#include <vector> 
#include <fstream>
#include <string>
#include <bitset>
#include <iomanip>
class CHIP { 
    public: 
    std::vector<uint8_t> registers; 
    std::vector<uint8_t> memory = std::vector<uint8_t>(4096, 0); 
    uint16_t indexRegister; 
    uint16_t programCounter = 0x200; 
    std::vector<uint16_t> stack = std::vector<uint16_t>(16); 
    uint8_t stackPointer; 
    uint8_t delayTimer; 
    uint8_t soundTimer; 
    bool running = true; 
    void fetch(); 
    void decode(); 
    void execute(); 
    void loadROM(const std::string &filename);
    void printMemory(); 
};

void CHIP::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    uint8_t data;
    size_t i = 0x200;
    while (i < memory.size() && file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        memory[i] = data;
        ++i;
    }

    file.close();
}
void CHIP::printMemory() 
{ 
    for(size_t i = 0x200; i < memory.size(); i += 2) 
    { 
      uint16_t res = 0;
      res = memory[i]; 
      res = res << 8; 
      res = res | memory[i + 1]; 
      std::cout << std::hex  << std::setw(4) << std::setfill('0') << res << " "; 
    }
}
int main() 
{ 
  CHIP emulator; 
  emulator.loadROM("IBM Logo.ch8"); 
  emulator.printMemory(); 
} 

