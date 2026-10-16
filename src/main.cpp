#include <iostream>
#include <cstdint>
#include <optional>
#include <string>
#include <charconv>

std::optional<std::uint16_t> parse_port(const std::string& text) {
    int value = 0;
    auto [ptr, ec] = std::from_chars(text.data(), text.data() + text.size(), value);

    if(ec != std::errc{}){

        return std::nullopt;
    }
    if(value < 1 || value > 65535){
        return std::nullopt;
    }
    if(ptr != text.data() + text.size()){
        return std::nullopt;
    }
    
    return static_cast<std::uint16_t>(value);
}

int main(int argc, char* argv[]){

    std::cout << "Portscanner v0.1.0" << '\n';
    std::cout << "аргументов: " << argc << '\n';

    for(int i = 0; i < argc; i++){
        std::cout << "argv[" << i << "] = " << argv[i] << '\n';
    }

    if(argc >= 2){
        auto box = parse_port(argv[1]);
        if(box){
            std::cout << "Порт: " << *box << '\n';
        }
        else{
            std::cout << "портов нету" << '\n';
        }
    }

    return 0;
}