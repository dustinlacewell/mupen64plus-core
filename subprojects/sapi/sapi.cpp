#include <string>
#include <optional>
#include <filesystem>

#include <boost/algorithm/string.hpp>
#include <cjson/cJSON.h>
#include <duktape.h>

#include "./sapi.hpp"
#include "./types.hpp"
#include "./globals.hpp"

#include "./utils.cpp"
#include "./events.cpp"
#include "./duk.cpp"

using namespace std;
namespace fs = std::filesystem;

optional<ScriptMeta*> init_script(string path) {
    log("Initializing script: %s", path);
    optional<string> text = read_file_text(path);

    if (!text) {
        log("Could not read script %s", path.c_str());
        return {};
    }

    // split text into lines with boost
    vector<string> lines = split_string(*text, "\n");

    optional<string> name = get_script_name(lines[0]);

    if (!name) {
        log("Could not find script name in %s", path.c_str());
        return {};
    }

    ScriptMeta* meta = new ScriptMeta();
    meta->name = *name;
    meta->path = path;
    meta->text = *text;
    return meta;
}

void invoke_callback(char* event) {
    log("Invoking callbacks for event %s", event);
    call_listeners(event);
}

void enable_script(char* name) {
    log("Enabling script %s", name);
    ScriptMeta* s = scripts.at(name);
    if (s) {
        if (s->ctx == NULL) {
            s->ctx = duk_create_heap_default();

            if (!s->ctx) {
                log("Failed to create Duktape heap for script: %s", name);
                return;
            }

            duk_push_heap_stash(s->ctx);
            duk_push_string(s->ctx, name);
            duk_put_prop_string(s->ctx, -2, "script_name");

            // add console.log as log to the ctx
            duk_push_c_function(s->ctx, duk_log, 1);
            duk_put_global_string(s->ctx, "log");

            duk_push_c_function(s->ctx, duk_register_callback, 2);
            duk_put_global_string(s->ctx, "on");

            if (duk_peval_string(s->ctx, s->text.c_str()) != 0) {
                log("Failed to execute script: %s", name);
                log("%s", duk_safe_to_string(s->ctx, -1));
                duk_destroy_heap(s->ctx);
                s->ctx = NULL;
                return;
            }
        }
    }
}

void disable_script(char* name) {
    log("Disabling script %s", name);
    ScriptMeta* s = scripts.at(name);
    if (s) {
        if (s->ctx) {
            duk_destroy_heap(s->ctx);
            s->ctx = NULL;
        }
    }
}

vector<string> load_scripts_from(const char* path) {
    log("Loading scripts from %s", path);
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

void init_sapi() {
    log("Initializing SAPI");
    games = GameMap();
    scripts = ScriptMap();
    events = EventMap();


    vector<string> filenames = load_scripts_from("/home/ldlework/.config/mupen64plus/scripts/");
    int n_scripts = filenames.size();

    log("Found %d scripts", n_scripts);

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
        enable_script((char*)name.c_str());

        log("Loaded script: %s", name.c_str());

        // increment counter
        i++;
    }

    log("Loaded %d scripts", i);
}