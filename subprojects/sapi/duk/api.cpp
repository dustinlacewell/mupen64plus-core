#pragma once

#include <duktape.h>

#include "../events.cpp"

#include "../utils.cpp"
#include "./utils.cpp"
#include "./events.cpp"

using namespace std;


duk_ret_t duk_log(duk_context* ctx) {
    if (duk_get_top(ctx) != 1) {
        /* throw TypeError if no arguments given */
        return DUK_RET_TYPE_ERROR;
    }
    // get first argument as string
    auto message = duk_get_string(ctx, 0);
    log("%s", message);
    return 0;
}

duk_ret_t FUNC(duk_register_listener, duk_context* ctx)
    // ensure two args passed
    if (duk_get_top(ctx) != 2) {
        log("%s", "duk_register_listener: invalid number of arguments");
        duk_error(ctx, DUK_ERR_ERROR, "Invalid arguments");
    }

    duk_require_string(ctx, 0);
    duk_require_function(ctx, 1);

    auto event_name = duk_get_string(ctx, 0);
    duk_set_event_callback(ctx, event_name, 1);

    return 0;
}

duk_ret_t FUNC(duk_emit_event, duk_context* ctx)
    if (duk_get_top(ctx) != 1) {
        log("%s", "duk_register_listener: invalid number of arguments");
        return DUK_RET_TYPE_ERROR;
    }

    duk_require_string(ctx, 0);

    auto event_name = (char*)duk_get_string(ctx, 0);
    call_listeners(event_name);
    return 0;
}