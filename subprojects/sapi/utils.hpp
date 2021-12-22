#pragma once

#ifdef __cplusplus

using namespace std;

#include <optional>
#include <string>
#include <iostream>
#include <fstream>


optional<string> read_file_text(string path);
void log(char* fmt...);

#endif