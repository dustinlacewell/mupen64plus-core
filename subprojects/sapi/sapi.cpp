#ifdef __cplusplus

using namespace std;

#include <string>
#include <optional>
#include <filesystem>

#include <boost/algorithm/string.hpp>
#include <cjson/cJSON.h>
#include <duktape.h>

#include "api/callbacks.h"

#include "./sapi.hpp"
#include "./utils.hpp"

namespace fs = std::filesystem;

void init_scripts() {
    games = GameMap();
    scripts = ScriptMap();
}

optional<string> get_script_name(string line) {
    // check line is >= 3 chars
    if (line.length() < 3) {
        return {};
    }

    // trim whitespace
    boost::trim(line);

    // check line starts with "//"
    if (line[0] != '/' || line[1] != '/') {
        return {};
    }

    // remove "//" prefix
    line = line.substr(2);

    return line;
}

optional<ScriptMeta*> init_script(string path) {
    optional<string> text = read_file_text(path);

    if (!text) {
        DebugMessage(M64MSG_ERROR, "Could not read script %s", path.c_str());
        return {};
    }

    optional<string> name = get_script_name(*text);

    if (!name) {
        DebugMessage(M64MSG_ERROR, "Could not find script name in %s", path.c_str());
        return {};
    }

    ScriptMeta* meta = new ScriptMeta();
    meta->name = *name;
    meta->path = path;
    meta->text = *text;
    return meta;
}

void enable_script(char* name) {
    ScriptMeta* s = scripts.at(name);
    if (s) {
        if (s->ctx == NULL) {
            s->ctx = duk_create_heap_default();

            if (!s->ctx) {
                DebugMessage(M64MSG_ERROR, "Failed to create Duktape heap for script: %s", name);
                return;
            }

            if (duk_peval_string(s->ctx, s->text.c_str()) != 0) {
                DebugMessage(M64MSG_ERROR, "Failed to execute script: %s", name);
                duk_destroy_heap(s->ctx);
                s->ctx = NULL;
                return;
            }
        }
    }
}

void disable_script(char* name) {
    ScriptMeta* s = scripts.at(name);
    if (s) {
        if (s->ctx) {
            duk_destroy_heap(s->ctx);
            s->ctx = NULL;
        }
    }
}

vector<string> load_scripts_from(const char* path) {
    // create new vector
    vector<string> scripts = vector<string>();

    // get all files in path
    for (const auto & entry : fs::directory_iterator(path))
    {
        // check file extension
        if (entry.path().extension() == ".js") {
            // add file to vector
            scripts.push_back(entry.path().string());
        }
    }

    return scripts;
}

void load_scripts() {
    DebugMessage(M64MSG_STATUS, "DBG: Loading scripts\n");
    init_scripts();

    vector<string> filenames = load_scripts_from("/home/ldlework/.config/mupen64plus/scripts/");
    int n_scripts = filenames.size();

    DebugMessage(M64MSG_STATUS, "DBG: Found %d scripts\n", n_scripts);

    int i = 0;
    // iterate over all filenames
    for (string filename : filenames) {
        // init script
        optional<ScriptMeta*> meta = init_script(filename);

        if (!meta) {
            continue;
        }

        string name = (*meta)->name;

        // add script to map
        scripts.insert({ name, meta.value() });

        // increment counter
        i++;
    }

    DebugMessage(M64MSG_STATUS, "DBG: Loaded %d scripts\n", i);
}

#endif