#pragma once

#include <duktape.h>

#include "./types.hpp"
#include "./utils.cpp"
#include "./duk/events.cpp"
#include "./globals.hpp"


void remove_listener(char* event, ScriptMeta* meta) {
    map<string, ScriptMeta*> listeners = events.at(event);
    listeners.erase(meta->name);
}

// void register_listener(duk_context* ctx) {
//     // ensure two args passed
//     if (duk_get_top(ctx) != 2) {
//         duk_error(ctx, DUK_ERR_ERROR, "Invalid arguments");
//     }

//     log("Attempting to register listener...");

//     duk_require_string(ctx, 0);
//     duk_require_function(ctx, 1);

//     // get event name
//     char* event = (char*)duk_get_string(ctx, 0);
//     char* symbol = event_symbol(event);

//     log("Found event: %s", event);

//     // store callback on es side
//     duk_push_heap_stash(ctx);
//     duk_dup(ctx, 1);
//     duk_put_prop_string(ctx, -2, symbol);

//     // get the script name
//     duk_push_heap_stash(ctx);
//     duk_get_prop_string(ctx, -1, "__script_name");
//     char* script_name = (char*)duk_to_string(ctx, -1);

//     log("Found script name: %s", script_name);

//     // check if script with name exists
//     if (scripts.find(script_name) == scripts.end()) {
//         duk_error(ctx, DUK_ERR_ERROR, "Script with name %s does not exist", script_name);
//     }

//     // debug log the keys of the events map
//     log("Registered events:");
//     for (auto& kv : events) {
//         log("%s", kv.first.c_str());
//     }

//     // check if event exists
//     if (events.find(event) == events.end()) {
//         events[event] = map<string, ScriptMeta*>();
//     }

//     // get the script meta
//     ScriptMeta* meta = scripts.at(script_name);

//     // add the script's callback a listener
//     map<string, ScriptMeta*> listeners = events.at(event);
//     listeners.insert(pair<string, ScriptMeta*>(meta->name, meta));
//     log("Registered %s listener for script %s", event, script_name);
// }

void call_listener(char* event, ScriptMeta* meta) {
    if (duk_has_event_callback(meta->ctx, event)) {
        log("Calling %s listener for script %s", event, meta->name.c_str());
        duk_invoke_event_callback(meta->ctx, event);
        log("Finished callback for %s", meta->name.c_str());
    } else {
        log("No %s listener for script %s", event, meta->name.c_str());
    }
}

void call_listeners(char* event) {
    log("Calling listeners for %s event", event);

    dump_scripts();

    log("There are %d scripts", scripts.size());

    for (auto& [name, meta] : scripts) {
        log("Calling listener for %s event for script %s", event, name.c_str());
        log("%s", name.c_str());
        log("- name: %s", meta->name.c_str());
        log("- path: %s", meta->path.c_str());
        log("- ctx is null: %s", meta->ctx == nullptr ? "true" : "false");
        call_listener(event, meta);
    }

    log("finished calling listeners");
}