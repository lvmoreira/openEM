#include "TUI.h"
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void terminal_interface(){
    // Creates a banner for the terminal interface
    const char* banner = R"(
   ____                   ________  ___
  / __ \____  ___  ____  / ____/  |/  /
 / / / / __ \/ _ \/ __ \/ __/ / /|_/ / 
/ /_/ / /_/ /  __/ / / / /___/ /  / /  
\____/ .___/\___/_/ /_/_____/_/  /_/   
    /_/                                                              
)";
    std::cout << "\033[36m" << banner << "\033[0m\n";
    std::cout << "===================================================== \n";
    std::cout << "OpenEM v1.0.0 - Computational Electromagnetics Solver \n";
    std::cout << "===================================================== \n\n";
}

int json_parser(){
    // Open json file and check for opening errors
    std::ifstream file("../simconfig.json");
    if(!file.is_open()){
        std::cerr << "Error: Could not open the JSON file.\n";
        return 1;
    }

    // Parse the JSON file using the nlohmann library
    json config;
    file >> config;

    // Extract the simulation parameters and initialize necessary objects
    return 0;
}