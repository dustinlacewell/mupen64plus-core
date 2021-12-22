#ifdef __cplusplus

using namespace std;

#include <optional>
#include <string>
#include <iostream>
#include <fstream>

#include <cstdarg>

extern "C" {
    #include "api/callbacks.h"
}

optional<string> read_file_text(string path) {
    // check if file exists
    ifstream file(path);
    if (!file.good()) {
        return {};
    }
    // return file text
    string text;
    file.seekg(0, ios::end);
    text.reserve(file.tellg());
    file.seekg(0, ios::beg);
    text.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return text;
}

// C++
// take format string and vargs and call DebugMessage
void log(char* fmt...) {
    // format string then pass to SAPI
    va_list args;
    va_start(args, fmt);
    char* str = nullptr;
    vasprintf(&str, fmt, args);

    DebugMessage(M64MSG_INFO, "SAPI: %s\n", str);
}

#endif
