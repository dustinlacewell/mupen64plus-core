#pragma once

#include <string>

#include <duktape.h>

#include "../utils.cpp"
#include "./utils.cpp"

using namespace std;


void FUNC(duk_set_event_callback, duk_context *ctx, string event_name, int idx)
    // [ event_name callback ]

    string symbol = event_symbol(event_name);
    duk_push_heap_stash(ctx);
    // [ event_name callback stash ]

    duk_dup(ctx, idx);
    // [ event_name callback stash callback ]

    duk_put_prop_string(ctx, -2, symbol.c_str());
    // [ event_name callback stash ]

    duk_pop(ctx);
    // [ event_name callback ]
}

void FUNC(duk_get_event_callback, duk_context *ctx, string event_name)
    string symbol = event_symbol(event_name);

    log("duk_get_event_callback: %s", symbol.c_str());

    duk_push_heap_stash(ctx);
    // [ stash ]

    duk_get_prop_string(ctx, -1, symbol.c_str());
    // [ stash callback ]
}

void FUNC(duk_invoke_event_callback, duk_context *ctx, string event_name)
    duk_get_event_callback(ctx, event_name);
    // [ stash callback ]

    duk_pcall(ctx, 0);
    // [ stash ]

    duk_pop(ctx);
    // [ ]
}

bool FUNC(duk_has_event_callback, duk_context *ctx, string event_name)
    duk_get_event_callback(ctx, event_name);
    // [ stash callback ]

    auto result = duk_is_callable(ctx, -1);
    // [ stash callback is_callable ]

    duk_pop(ctx);
    // [ stash ]

    return result == 1;
}