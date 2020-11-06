#include <stdio.h>
#include <dlfcn.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>

#include "events.h"
#include "hooker.h"
#include "rec_notificator.h"

const uint8_t abs_jmp_aarch64_bytecode[8] =
{
    0x49, 0x00, 0x00, 0x58,     // LDR x9, .+8
    0x20, 0x01, 0x1F, 0xD6      // BR x9
    // Next must be 8 bytes with address (little-endian)
};

const uint8_t abs_call_aarch64_bytecode[8] =
{
    0x49, 0x00, 0x00, 0x58,     // LDR x9, .+8
    0x20, 0x01, 0x3F, 0xD6      // BLR x9 (call x9; x30 = PC+4)
    // Next must be 8 bytes with address (little-endian)
};

/*
 * Hook declarations.
*/

// Muxer recording start/stop (NOT CALLED FOR SOME REASON)
//DECL_HOOK_IMVT_FUNCTION(fmt_muxer_start_record, 0x6FBDE8);
//DECL_HOOK_IMVT_FUNCTION(fmt_muxer_stop_record, 0x6FBE00);

// Called on ".MOV" file creation
//DECL_HOOK_IMVT_FUNCTION(mov_muxer_prepare_file, 0x6FF2A8);

// Led handlers
//DECL_HOOK_IMVT_FUNCTION(imvt_led_set_rec_start, 0x576A08);          // Sets back LED to "REC" color (is called on recording start)
//DECL_HOOK_IMVT_FUNCTION(imvt_led_set_rec_stop, 0x576A58);           // Sets back LED to "NOT REC" color (is called on recording stop)
//DECL_HOOK_IMVT_FUNCTION(imvt_led_set_cap_start, 0x576C00);          // Sets back LED to "CAP" color (is called on capture start)
//DECL_HOOK_IMVT_FUNCTION(imvt_led_set_cap_stop, 0x576C58);           // Sets back LED to "NOT CAP" color (is called on capture stop)
//DECL_HOOK_IMVT_FUNCTION(imvt_led_camera_event_handler, 0x6131A8);   // Handles camera state change event for LED (to change color, for example)

// Recording start/stop
DECL_HOOK_IMVT_FUNCTION(imvt_cam_movie_start, 0x6C5FD0);            // Called on camera recording start
DECL_HOOK_IMVT_FUNCTION(imvt_cam_movie_stop, 0x6C1A98);             // Called on camera recording stop

/*
 * Hook functions definitions.
*/
/*
uint64_t HOOK__fmt_muxer_start_record(uint64_t arg)
{
    on__fmt_muxer_start_record();

    DISABLE_HOOK(fmt_muxer_start_record);
    uint64_t a = fmt_muxer_start_record(arg);
    RENABLE_HOOK(fmt_muxer_start_record);
    return a;
}
uint64_t HOOK__fmt_muxer_stop_record(uint64_t arg)
{
    on__fmt_muxer_stop_record();

    DISABLE_HOOK(fmt_muxer_stop_record);
    uint64_t a = fmt_muxer_stop_record(arg);
    RENABLE_HOOK(fmt_muxer_stop_record);
    return a;
}
uint64_t HOOK__mov_muxer_prepare_file(uint64_t arg)
{
    FILE* fp = fopen(log_path, "a");
    fprintf(fp, "pre on__mov_muxer_prepare_file...\n");
    fclose(fp);
    on__mov_muxer_prepare_file();
    fp = fopen(log_path, "a");
    fprintf(fp, "post on__mov_muxer_prepare_file...\n");
    fclose(fp);

    DISABLE_HOOK(mov_muxer_prepare_file);
    uint64_t a = mov_muxer_prepare_file(arg);
    RENABLE_HOOK(mov_muxer_prepare_file);
    return a;
}
uint64_t HOOK__imvt_led_set_rec_start()
{
    on__imvt_led_set_rec_start();

    DISABLE_HOOK(imvt_led_set_rec_start);
    uint64_t a = imvt_led_set_rec_start();
    RENABLE_HOOK(imvt_led_set_rec_start);
    return a;
}
uint64_t HOOK__imvt_led_set_rec_stop()
{
    on__imvt_led_set_rec_stop();

    DISABLE_HOOK(imvt_led_set_rec_stop);
    uint64_t a = imvt_led_set_rec_stop();
    RENABLE_HOOK(imvt_led_set_rec_stop);
    return a;
}
uint64_t HOOK__imvt_led_set_cap_start()
{
    on__imvt_led_set_cap_start();

    DISABLE_HOOK(imvt_led_set_cap_start);
    uint64_t a = imvt_led_set_cap_start();
    RENABLE_HOOK(imvt_led_set_cap_start);
    return a;
}
uint64_t HOOK__imvt_led_set_cap_stop()
{
    on__imvt_led_set_cap_stop();

    DISABLE_HOOK(imvt_led_set_cap_stop);
    uint64_t a = imvt_led_set_cap_stop();
    RENABLE_HOOK(imvt_led_set_cap_stop);
    return a;
}
int64_t HOOK__imvt_led_camera_event_handler(uint64_t address)
{
    on__imvt_led_camera_event_handler(address);

    DISABLE_HOOK(imvt_led_camera_event_handler);
    int64_t a = imvt_led_camera_event_handler(address);
    RENABLE_HOOK(imvt_led_camera_event_handler);
    return a;
}*/
int64_t HOOK__imvt_cam_movie_start(uint32_t stream_style)
{
    on__imvt_cam_movie_start(stream_style);

    DISABLE_HOOK(imvt_cam_movie_start);
    int64_t a = ((int64_t(*)(uint64_t))0x6C5FD0)(stream_style);
    RENABLE_HOOK(imvt_cam_movie_start);
    return a;
}

void HOOK__imvt_cam_movie_stop(uint32_t stream_style)
{
    // Process notification
    on__imvt_cam_movie_stop(stream_style);

    // Get stream_style parameter back (previous call corrupted W0 register)
    asm("LDR W0, [SP,#0x1C]");

    // Epilogue for current function
    asm("LDP X29, X30, [SP], #0x20");

    // Prologue copy from original function (imvt_cam_movie_stop)
    asm("STP X29, X30, [SP, #-0x80]!");
    asm("MOV X29, SP");
    asm("STP X19, X20, [SP, #0x10]");
    asm("MOV W19, W0");

    // Exit hook to function body (jumpout)
    asm("MOV X9, #0x1AA8");                 // 0x6C1AA8 - JUMPOUT address
    asm("MOVK X9, #0x6C, LSL#16");
    asm("BR X9");
}

void hooker_init()
{
    log_print("[SC] Injecting...\n");
    //PROC_HOOK_IMVT_FUNCTION(fmt_muxer_start_record, &HOOK__fmt_muxer_start_record);
    //PROC_HOOK_IMVT_FUNCTION(fmt_muxer_stop_record, &HOOK__fmt_muxer_stop_record);
    //PROC_HOOK_IMVT_FUNCTION(mov_muxer_prepare_file, &HOOK__mov_muxer_prepare_file);
    //PROC_HOOK_IMVT_FUNCTION(imvt_led_set_rec_start, &HOOK__imvt_led_set_rec_start);
    //PROC_HOOK_IMVT_FUNCTION(imvt_led_set_rec_stop, &HOOK__imvt_led_set_rec_stop);
    //PROC_HOOK_IMVT_FUNCTION(imvt_led_set_cap_start, &HOOK__imvt_led_set_cap_start);
    //PROC_HOOK_IMVT_FUNCTION(imvt_led_set_cap_stop, &HOOK__imvt_led_set_cap_stop);
    //PROC_HOOK_IMVT_FUNCTION(imvt_led_camera_event_handler, &HOOK__imvt_led_camera_event_handler);
    //PROC_HOOK_IMVT_FUNCTION(imvt_cam_movie_start, &HOOK__imvt_cam_movie_start);
    //PROC_HOOK_IMVT_FUNCTION(imvt_cam_movie_stop, &HOOK__imvt_cam_movie_stop);
    log_print("[SC] Injected!\n");
}