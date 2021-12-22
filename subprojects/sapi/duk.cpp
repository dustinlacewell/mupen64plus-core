#pragma once

#include <string>
#include <optional>
#include <variant>
#include <fmt/core.h>

#include <duktape.h>

#include "./types.hpp"
#include "./utils.cpp"
#include "./events.cpp"

using namespace std;


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

void duk_set_script_name(duk_context* ctx, const char* name) {
    duk_push_heap_stash(ctx);
    duk_push_string(ctx, name);
    duk_put_global_string(ctx, "__script_name");
}

void duk_install_log(duk_context* ctx) {
    duk_push_c_function(ctx, duk_log, 1);
    duk_put_global_string(ctx, "log");
}

void duk_install_events(duk_context* ctx) {
    duk_push_c_function(ctx, duk_register_callback, 2);
    duk_put_global_string(ctx, "on");
}



MaybeContext duk_create_context(ScriptMeta* script, const char* name) {
    MaybeContext maybe = duk_create_heap_default();

    if (const auto ctxPtr (get_if<duk_context*>(&maybe)); ctxPtr) {
        if (!*ctxPtr) {
            maybe = fmt::format("Failed to create Duktape context for script '%s'", name);
            return maybe;
        }

        duk_context* ctx = *ctxPtr;

        duk_set_script_name(ctx, script->name.c_str());

        duk_install_log(ctx);

        duk_install_events(ctx);

        if (duk_peval_string(ctx, script->text.c_str()) == 0) {
            script->ctx = ctx;
        }

        return maybe;
    }
}