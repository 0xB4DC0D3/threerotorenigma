#include <iostream>
#include "Enigma.h"

int main() {
    ThreeRotorEnigma x{
        std::set<std::string> {},
        0,
        std::array<int, 3>{ 0, 1, 2 },
        //std::array<int, 3>{ 16, 20, 2 },
        std::array<int, 3>{0,0,0},
        std::array<int, 3>{ 0, 0, 0 }
    };

    std::cout << "\x1b[33m[Enigma] \x1b[37m" << x.encrypt_message("VV RZU KH") << "\x1b[0m" << std::endl;
}