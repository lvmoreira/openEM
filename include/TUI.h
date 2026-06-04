#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


void terminal_interface();
json json_parser();