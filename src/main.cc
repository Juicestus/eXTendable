#include "main.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <file.xt>" << std::endl;
        return 1;
    }

    XT* xt = new XT(argv[1]);

    try {
        xt->execute(ignoreShebang(readFile(argv[1])), true);
    } catch (Exception* e) { 
        std::printf("ERROR: %s\n", e->text.c_str()); 
    }
}