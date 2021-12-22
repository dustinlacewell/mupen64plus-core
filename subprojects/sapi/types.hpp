#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>
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

typedef variant<string, duk_context*> MaybeContext;