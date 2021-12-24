#pragma once

#ifdef __cplusplus

#define TRACE 1

#include <string>
#include <vector>

using namespace std;

vector<string> split(string str, string delimiter) {
    vector<string> result;
    size_t pos = 0;
    string token;
    while ((pos = str.find(delimiter)) != string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    result.push_back(str);
    return result;
}

void removeSubstrs(string& s, string& p) {
  string::size_type n = p.length();
  for (string::size_type i = s.find(p);
      i != string::npos;
      i = s.find(p))
      s.erase(i, n);
}

// get sapi/* from path
char* get_filename(char* path) {
    auto s = string(path);
    // find location of lasf "/sapi/" substring
    auto pos = s.rfind("/sapi/");
    // get /sapi/ and everything after
    auto partial = s.substr(pos);

    // remove all "/." substrings
    string needle = "/.";
    removeSubstrs(partial, needle);

    char* result = strdup(partial.c_str());
    return result;
}


#if TRACE == 1
#  define FUNC(name, ...) name(__VA_ARGS__) { \
    DebugMessage(M64MSG_INFO, "+ %s(%d) %s\n", get_filename(__FILE__), __LINE__, #name);
#else
#  define FUNC(name, ...) name(__VA_ARGS__) {
#endif

extern "C" {
#endif

    void init_sapi();
    void enable_script(char* name);
    void disable_script(char* name);
    void invoke_callback(char* name);
    void evaluate(char* code);

#ifdef __cplusplus
}
#endif



