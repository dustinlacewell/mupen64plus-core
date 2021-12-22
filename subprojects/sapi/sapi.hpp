#ifdef __cplusplus

using namespace std;

#include <vector>
#include <map>
#include <string>
#include <duktape.h>

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

GameMap games;
ScriptMap scripts;

void load_scripts();
ScriptMeta init_script(char* path);
void enable_script(char* name);
void disable_script(char* name);


extern "C" {
    void init_sapi() {
        load_scripts();
    };
}
#else
void init_sapi();
#endif



