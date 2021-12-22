#pragma once

#include <duktape.h>

#include "./types.hpp"
#include "./globals.hpp"
#include "./utils.cpp"


void remove_listener(char* event, ScriptMeta* meta) {
    duk_context* ctx = meta->ctx;
    map<string, ScriptMeta*> listeners = events.at(event);
    listeners.erase(meta->name);
}

char* event_symbol(char* event) {
    char* symbol = (char*)malloc(strlen(event) + 3);
    sprintf(symbol, "__%s", event);
    return symbol;
}

void register_listener(duk_context* ctx) {
    // ensure two args passed
    if (duk_get_top(ctx) != 2) {
        duk_error(ctx, DUK_ERR_ERROR, "Invalid arguments");
    }

    log("Attempting to register listener...");

    duk_require_string(ctx, 0);
    duk_require_function(ctx, 1);

    // get event name
    const char* event = duk_get_string(ctx, 0);
    char* symbol = event_symbol((char*)event);

    log("Found event: %s", event);

    // store callback on es side
    duk_push_heap_stash(ctx);
    duk_dup(ctx, 1);
    duk_put_prop_string(ctx, -2, symbol);

    // get the script name
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, "script_name");
    char* script_name = (char*)duk_to_string(ctx, -1);

    log("Found script name: %s", script_name);

    // check if script with name exists
    if (scripts.find(script_name) == scripts.end()) {
        duk_error(ctx, DUK_ERR_ERROR, "Script with name %s does not exist", script_name);
    }

    // check if event exists
    if (events.find(event) == events.end()) {
        events[event] = map<string, ScriptMeta*>();
    }

    // get the script meta
    ScriptMeta* meta = scripts.at(script_name);

    // add the script's callback a listener
    map<string, ScriptMeta*> listeners = events.at(event);
    listeners.insert(pair<string, ScriptMeta*>(meta->name, meta));
    log("Registered %s listener for script %s", event, script_name);
}

void call_listener(char* event, char* name) {
    log("Calling %s listener for script %s", event, name);
    ScriptMeta* meta = scripts.at(name);

    char* symbol = event_symbol(event);
    duk_push_global_object(meta->ctx);
    duk_get_prop_string(meta->ctx, -1, symbol);
    duk_pcall(meta->ctx, 1);
    duk_pop(meta->ctx);
}

void call_listeners(char* event) {
    log("Calling listeners for %s event", event);
    if (events.find(event) == events.end()) {
        log("No listeners for %s event", event);
        return;
    }
    map<string, ScriptMeta*> listeners = events.at(event);
    log("Found %d listeners for %s event", listeners.size(), event);
    for (auto it = listeners.begin(); it != listeners.end(); it++) {
        call_listener(event, (char*)it->first.c_str());
    }
}