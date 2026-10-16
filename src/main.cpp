#include <iostream>

int main(int argc, char* argv[]){

    std::cout << "Portscanner v0.1.0" << '\n';
    std::cout << "аргументов: " << argc << '\n';

    for(int i = 0; i < argc; i++){
        std::cout << "argv[" << i << "] = " << argv[i] << '\n';
    }

    return 0;
}