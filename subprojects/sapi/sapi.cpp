#include <string>
#include <optional>
#include <filesystem>

#include <boost/algorithm/string.hpp>
#include <cjson/cJSON.h>
#include <duktape.h>

#include "./sapi.hpp"
#include "./utils.hpp"
#include "./events.hpp"

using namespace std;
namespace fs = std::filesystem;

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

    boost::trim(line);

    return line;
}

vector<string> split_string(string line, string delimiter) {
    vector<string> result;
    boost::split(result, line, boost::is_any_of(delimiter));
    return result;
}

optional<ScriptMeta*> init_script(string path) {
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

duk_ret_t duk_log(duk_context* ctx) {
    if (duk_get_top(ctx) != 1) {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }
    // get first argument as string
    const char* message = duk_to_string(ctx, 0);
    log("%s", message);
    return 0;
}

duk_ret_t duk_register_callback(duk_context* ctx) {
    register_listener(ctx);
    return 0;
}

void _invoke_callback(char* event) {
    call_listeners(event);
}

void _enable_script(char* name) {
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

void _disable_script(char* name) {
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

void _init_sapi() {
    log("Loading scripts");
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
        _enable_script((char*)name.c_str());

        log("Loaded script: %s", name.c_str());

        // increment counter
        i++;
    }

    log("Loaded %d scripts", i);
}