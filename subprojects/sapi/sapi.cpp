#include <string>
#include <optional>
#include <filesystem>
#include <variant>

#include <boost/algorithm/string.hpp>
#include <cjson/cJSON.h>
#include <duktape.h>

#include "./sapi.hpp"
#include "./types.hpp"
#include "./globals.hpp"

#include "./utils.cpp"
#include "./events.cpp"
#include "./duk/index.cpp"

using namespace std;
namespace fs = std::filesystem;

optional<ScriptMeta*> FUNC(init_script, string path)
    log("Initializing script: %s", path.c_str());
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

void FUNC(invoke_callback, char* event)
    log("Invoking callbacks for event %s", event);
    dump_scripts();
    call_listeners(event);
    log("Finished invoking event");
}

void FUNC(enable_script, char* name)
    log("Enabling script %s", name);
    ScriptMeta* s = scripts.at(name);
    if (s) {
        if (s->ctx == NULL) {
            MaybeContext maybeCtx = duk_init_context(s);

            if (holds_alternative<string>(maybeCtx)) {
                log("Could not enable script %s: %s", name, get<string>(maybeCtx).c_str());
                return;
            }

            s->ctx = get<duk_context*>(maybeCtx);
            log("Script %s enabled", name);
        }
    }
}

void FUNC(disable_script, char* name)
    log("Disabling script %s", name);
    ScriptMeta* s = scripts.at(name);
    if (s) {
        if (s->ctx) {
            duk_destroy_heap(s->ctx);
            s->ctx = NULL;
        }
    }
}

vector<string> FUNC(load_scripts_from, const char* path)
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

void FUNC(init_sapi)
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
    dump_scripts();
}

void FUNC(evaluate, char* script)
    log("Evaluating some script!");
    MaybeResult result = duk_evaluate(script);
    if (holds_alternative<string>(result)) {
        log("Result: %s", get<string>(result).c_str());
    } else {
        log("Error: %s", get<error>(result).msg.c_str());
    }
}