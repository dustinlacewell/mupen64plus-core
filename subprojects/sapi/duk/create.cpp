#pragma once

#include <string>
#include <optional>
#include <variant>

#ifdef __cplusplus
    #include <fmt/core.h>
#endif

#include <duktape.h>

#include "../types.hpp"
#include "./api.cpp"

using namespace std;


void FUNC(duk_set_script_name, duk_context* ctx, const char* name)
    duk_set_secret(ctx, "script_name", name);
}

void FUNC(duk_install_emit, duk_context* ctx)
    duk_push_c_function(ctx, duk_emit_event, 1);
    duk_put_global_string(ctx, "emit");
}

void FUNC(duk_install_log, duk_context* ctx)
    duk_push_c_function(ctx, duk_log, 1);
    duk_put_global_string(ctx, "log");
}

void FUNC(duk_install_events, duk_context* ctx)
    duk_push_c_function(ctx, duk_register_listener, 2);
    duk_put_global_string(ctx, "on");
}

MaybeContext FUNC(duk_create_context)
    MaybeContext maybe = duk_create_heap_default();

    if (const auto ctxPtr (get_if<duk_context*>(&maybe)); ctxPtr) {

        if (!*ctxPtr) {
            maybe = "Failed to create Duktape context.";
            return maybe;
        }

        return *ctxPtr;
    }
}

void FUNC(duk_install_api,duk_context* ctx)
    duk_install_log(ctx);
    duk_install_events(ctx);
    duk_install_emit(ctx);
}

MaybeContext FUNC(duk_init_context, ScriptMeta* script)
    MaybeContext maybe = duk_create_context();

    if (const auto ctxPtr (get_if<duk_context*>(&maybe)); ctxPtr) {

        if (!*ctxPtr) {
            maybe = "Failed to create Duktape context.";
            return maybe;
        }

        duk_context* ctx = *ctxPtr;

        duk_set_script_name(ctx, script->name.c_str());

        duk_install_api(ctx);

        if (duk_peval_string(ctx, script->text.c_str()) == 0) {
            duk_pop(ctx);
            maybe = ctx;
        } else {
            maybe = duk_safe_to_string(ctx, -1);
            duk_destroy_heap(ctx);
        }

        return maybe;
    }
}

MaybeResult FUNC(duk_evaluate_in_context, duk_context* ctx, char* script)
    MaybeResult result;
    char* msg;
    if (duk_peval_string(ctx, script) == 0) {
        result = duk_safe_to_string(ctx, -1);
    } else {
        msg = (char*)duk_safe_to_string(ctx, -1);
        error err = { msg };
        result = err;
    }

    return result;
}

MaybeResult FUNC(duk_evaluate, char* script)
    MaybeContext maybeCtx = duk_create_context();

    if (const auto ctxPtr (get_if<duk_context*>(&maybeCtx)); ctxPtr) {

        if (!*ctxPtr) {
            return "Failed to create Duktape context.";
        }

        duk_context* ctx = *ctxPtr;

        duk_install_api(ctx);

        auto result = duk_evaluate_in_context(ctx, script);

        return result;
    }

}
