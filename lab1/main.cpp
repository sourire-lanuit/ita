#include "Header.h"
#include <iostream>

using namespace Base64;

int main() {
    try {
        while (true) {
            std::cout << "\n=== Base64 Converter ===\n";
            std::cout << "1. Encode file\n";
            std::cout << "2. Decode file\n";
            std::cout << "0. Exit\n";
            std::cout << "Enter: ";

            int choice;
            std::cin >> choice;

            if (choice == 0) {
                std::cout << "Terminate session \n";
                break;
            }

            if (choice == 1) {
                std::string input, output;
                std::cout << "Input file: ";
                std::cin >> input;

                std::cout << "Output .base64 file: ";
                std::cin.ignore();
                getline(std::cin, output);

                if (output.empty()) output = input + ".base64";

                auto data = read_file(input);
                std::string encoded = encode(data);

                std::vector<std::string> lines;
                for (size_t i = 0; i < encoded.size(); i += line_lenght) lines.push_back(encoded.substr(i, line_lenght));

                write_text(output, lines);
                std::cout << "Encoded successfully." << output << '\n';
            }

            else if (choice == 2) {
                std::string input, output;
                std::cout << "Input .base64 file: ";
                std::cin >> input;

                std::cout << "Output file: ";
                std::cin >> output;

                auto lines = read_text(input);
                auto decoded = decode(lines);
                write_file(output, decoded);

                std::cout << "Decoded successfully." << output << '\n';
            }
            else {
                std::cout << "Unknown choice\n";
            }
        }
    }

    catch (const std::exception& except) {
        std::cerr << "\nERROR: " << except.what() << '\n';
        system("pause");
        return 1;
    }

    return 0;
}