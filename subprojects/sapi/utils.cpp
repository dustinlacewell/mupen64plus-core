#ifdef __cplusplus

using namespace std;

#include <optional>
#include <string>
#include <iostream>
#include <fstream>


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

#endif