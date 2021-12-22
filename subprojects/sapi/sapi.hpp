#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    void init_sapi();
    void enable_script(char* name);
    void disable_script(char* name);
    void invoke_callback(char* name);

#ifdef __cplusplus
}
#endif



