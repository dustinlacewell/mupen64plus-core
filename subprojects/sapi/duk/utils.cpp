#pragma once

#include <string>
#include <duktape.h>

using namespace std;


string event_symbol(string event_name) {
    // add "__" prefix to event name
    return "__" + event_name;
}

string FUNC(duk_get_secret, duk_context* ctx, string key)
    string symbol = event_symbol(key);
    duk_push_heap_stash(ctx);
    duk_get_prop_string(ctx, -1, symbol.c_str());
    string secret = duk_to_string(ctx, -1);
    duk_pop_2(ctx);
    return secret;
}

void FUNC(duk_set_secret, duk_context* ctx, string key, string value)
    string symbol = event_symbol(key);
    duk_push_heap_stash(ctx);
    duk_push_string(ctx, value.c_str());
    duk_put_prop_string(ctx, -2, symbol.c_str());
    duk_pop(ctx);
}