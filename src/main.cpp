#include "main.h"

void lexsh() {
    std::printf("\033[33m[Lexer Test Shell]\033[m\n");
    xt::Lexer lexer;
    while (true) {
        std::string input;
        std::printf("\033[1m\033[32m>>> \033[m");
        std::getline(std::cin, input);
        lexer.New(input);
        printf("| Type     | Literal  |\n|––––––––––|––––––––––|\n");
        xt::Token token = lexer.nextToken();
        while (token.type != xt::END) {
            std::printf("| %8s | %8s |\n", token.type.c_str(), 
                    token.literal.c_str());
            token = lexer.nextToken();
        }
    }
}

void parsesh() {
    std::printf("\033[33m[Parser Test Shell]\033[m\n");
    
}

int main(int argc, char** argv) {
    printf("\033[1m\033[36mE\033[31mXT\033[36mENDABLE\033[32m C++\033[m");
    //printf("\033[1m  (C)\033[m Justus Languell\n");
    lexsh();
    return 0;
}
