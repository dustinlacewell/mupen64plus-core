#pragma once

#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>

using namespace std;

extern "C" {
    #include "api/callbacks.h"
}

#include "./globals.hpp"


optional<string> read_file_text(string path) {
    // check if file exists
    ifstream file(path);
    if (!file.good()) {
        return {};
    }
    // return file text
    string text;
    file.seekg(0, ios::end);
    text.reserve(file.tellg());
    file.seekg(0, ios::beg);
    text.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return text;
}

void log(char* fmt...) {
    // format string then pass to frontend
    va_list args;
    va_start(args, fmt);
    char* str = nullptr;
    vasprintf(&str, fmt, args);
    DebugMessage(M64MSG_INFO, "SAPI: %s\n", str);
}

vector<string> split_string(string line, string delimiter) {
    vector<string> result;
    boost::split(result, line, boost::is_any_of(delimiter));
    return result;
}

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

void dump_context(duk_context* ctx) {
    #if DUK_DUMP != 0
    log("Dumping context");
    duk_push_context_dump(ctx);
    string dump = duk_to_string(ctx, -1);
    duk_pop(ctx);
    log("%s\n", dump.c_str());
    #endif
}

void dump_scripts() {
    #if DUK_DUMP != 0
    log("%s", "Dumping scripts...");
    for (auto& [name, meta] : scripts) {
        log("%s", name.c_str());
        log("- name: %s", meta->name.c_str());
        log("- path: %s", meta->path.c_str());
        log("- ctx is null: %s", meta->ctx == nullptr ? "true" : "false");
        if (meta->ctx != nullptr) {
            dump_context(meta->ctx);
        }
    }
    #endif
}