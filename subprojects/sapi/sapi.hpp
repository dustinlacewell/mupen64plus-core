#pragma once

#ifdef __cplusplus

#include <vector>
#include <map>
#include <string>
#include <duktape.h>

using namespace std;


class ScriptMeta {
    public:
    string name;
    string path;
    string text;
    duk_context *ctx;
};

class GameMeta {
    string name;
    vector<string> scripts;
};

typedef map<string, ScriptMeta*> ScriptMap;
typedef map<string, GameMeta*> GameMap;
typedef map<string, map<string, ScriptMeta*>> EventMap;

GameMap games;
ScriptMap scripts;
EventMap events;

void _init_sapi();
void _enable_script(char* name);
void _disable_script(char* name);
void _invoke_callback(char* name);

extern "C" {
    void init_sapi() {
        _init_sapi();
    };
    void enable_script(char* name) {
        _enable_script(name);
    }
    void disable_script(char* name) {
        _disable_script(name);
    }
    void invoke_callback(char* name) {
        _invoke_callback(name);
    }
}
#else
    // C exported functions
    void init_sapi();
    void enable_script(char* name);
    void disable_script(char* name);
    void invoke_callback(char* name);
#endif



