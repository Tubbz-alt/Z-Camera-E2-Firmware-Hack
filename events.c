#include <stdio.h>
#include <stdint.h>

#include "events.h"
#include "imvt_types.h"
#include "log.h"
#include "rec_notificator.h"

void on__fmt_muxer_start_record()
{
    log_print("on__fmt_muxer_start_record\n");
}

void on__fmt_muxer_stop_record()
{
    log_print("on__fmt_muxer_stop_record\n");
}

void on__mov_muxer_prepare_file()
{
    log_print("on__mov_muxer_prepare_file\n");
}

void on__imvt_led_set_rec_start()
{
    log_print("LED color switched to <REC>.\n");
}

void on__imvt_led_set_rec_stop()
{
    log_print("LED color switched to <NOT REC>.\n");
}

void on__imvt_led_set_cap_start()
{
    log_print("LED color switched to <CAP>.\n");
}

void on__imvt_led_set_cap_stop()
{
    log_print("LED color switched to <NOT CAP>.\n");
}

void on__imvt_led_camera_event_handler(uint64_t address)
{
    imvt_camera_event_0* obj = (imvt_camera_event_0*)address;
    log_print("LED event encountered! Event value = %u\n", obj->what);
}

void on__imvt_cam_movie_start(uint32_t stream_style)
{
    log_print("[imvt_cam_movie_start] stream_style = %u\n", stream_style);
}

void on__imvt_cam_movie_stop(uint32_t stream_style)
{
    log_print("[imvt_cam_movie_stop] stream_style = %u\n", stream_style);
}