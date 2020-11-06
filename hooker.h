#pragma once
#include <stdint.h>
#include <stdio.h>

#include "log.h"

#define HOOK_PROLOGUE_SIZE 16

#pragma pack(1)
typedef struct
{
    char* func_name;
    uint64_t imvt_bin_offset;
    uint8_t orig_prologue[HOOK_PROLOGUE_SIZE];
    uint8_t hook_prologue[HOOK_PROLOGUE_SIZE];
} func_hook_data_t;
#pragma pack()

extern const uint8_t abs_jmp_aarch64_bytecode[8];
extern const uint8_t abs_call_aarch64_bytecode[8];

#define HOOK_STRUCT_NAME(function_name) hook__struct__##function_name
#define HOOK_ORIG_ADDR_NAME(function_name) hook__addr__##function_name

/*
 * This macro declares IMVT function.
*/
#define DECL_HOOK_IMVT_FUNCTION(function_name, orig_addr) \
    func_hook_data_t HOOK_STRUCT_NAME(function_name); \
    const uint64_t HOOK_ORIG_ADDR_NAME(function_name) = orig_addr;

/*
 * This macro sets up hook for IMVT function.
*/
#define PROC_HOOK_IMVT_FUNCTION(function_name, hook_addr) \
    log_print("[HOOK] Processing hook for %s -> 0x%lX...\n", #function_name, (uint64_t)hook_addr); \
    HOOK_STRUCT_NAME(function_name).func_name = #function_name; \
    log_print("[HOOK] Reading prologue for %s -> 0x%lX...\n", #function_name, (uint64_t)hook_addr); \
    memcpy(HOOK_STRUCT_NAME(function_name).orig_prologue, (void*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_PROLOGUE_SIZE); \
    log_memory((uint8_t*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_PROLOGUE_SIZE); \
    memcpy(HOOK_STRUCT_NAME(function_name).hook_prologue, abs_jmp_aarch64_bytecode, 8); \
    *(uint64_t*)&HOOK_STRUCT_NAME(function_name).hook_prologue[8] = (uint64_t)hook_addr; \
    log_print("[HOOK] Init abs_jump to address 0x%lX...\n", (uint64_t)hook_addr); \
    memcpy((void*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_STRUCT_NAME(function_name).hook_prologue, HOOK_PROLOGUE_SIZE); \
    log_memory((uint8_t*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_PROLOGUE_SIZE); \
    log_print("[HOOK] Done hook for %s!\n", #function_name);

/*
 * This macro disables (already initialized) hook for IMVT function.
*/
#define DISABLE_HOOK(function_name) \
    /*log_print("[HOOK] Disabling hook for %s; memory before disable:\n", #function_name);*/ \
    /*log_memory((uint8_t*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_PROLOGUE_SIZE);*/ \
    memcpy((void*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_STRUCT_NAME(function_name).orig_prologue, HOOK_PROLOGUE_SIZE); \
    /*log_print("[HOOK] Disabled hook for %s; memory after disable:\n", #function_name);*/ \
    /*log_memory((uint8_t*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_PROLOGUE_SIZE);*/;

/*
 * This macro enables again (already initialized) hook for IMVT function.
*/
#define RENABLE_HOOK(function_name) \
    /*log_print("[HOOK] Renabling hook for %s; memory before renable:\n", #function_name);*/ \
    /*log_memory((uint8_t*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_PROLOGUE_SIZE);*/ \
    memcpy((void*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_STRUCT_NAME(function_name).hook_prologue, HOOK_PROLOGUE_SIZE); \
    /*log_print("[HOOK] Renabled hook for %s; memory after renable:\n", #function_name);*/ \
    /*log_memory((uint8_t*)HOOK_ORIG_ADDR_NAME(function_name), HOOK_PROLOGUE_SIZE);*/;

void hooker_init();