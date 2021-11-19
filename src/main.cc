#include "exception.h"
#include "lexer.h"
#include "token.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <file.xt>" << std::endl;
        return 1;
    }

    std::string code = readFile(argv[1]);
}