#ifdef __cplusplus

#pragma once

#include <duktape.h>

#include "./sapi.hpp"

void remove_listener(char* event, ScriptMeta* meta);
char* event_symbol(char* event);
void register_listener(duk_context* ctx);
void call_listener(char* event, char* name);
void call_listeners(char* event);

#endif