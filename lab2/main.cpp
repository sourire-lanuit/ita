#include "Header.h"
#include <iostream>
#include <limits>

void file_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    int choice;

    while (true) {
        std::cout << "\n===== MENU =====\n";
        std::cout << "1. Encode file\n";
        std::cout << "2. Decode file\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose option: ";

        std::cin >> choice;

        if (std::cin.fail()) {
            file_input();
            std::cout << "Invalid input\n";
            continue;
        }

        std::string input, output;

        switch (choice) {
            case 0: {
                std::cout << "Exit.\n";
                break;
            }

            case 1: {
                std::cout << "Enter input file: ";
                std::cin >> input;
                std::cout << "Enter output file (if not then auto .rle): ";
                file_input();
                std::getline(std::cin, output);
                if (output.empty())  output = input + ".rle";
                if (RLE::encode(input, output)) std::cout << "Encoded successfully.\n";
                else std::cout << "Encoding failed.\n";
                break;
            }

            case 2: {
                std::cout << "Enter encoded file: ";
                std::cin >> input;
                std::cout << "Enter output file: ";
                std::cin >> output;
                if (RLE::decode(input, output)) std::cout << "Decoded successfully.\n";
                else std::cout << "Decoding failed.\n";
                break;
            }
        }
    }

    return 0;
}