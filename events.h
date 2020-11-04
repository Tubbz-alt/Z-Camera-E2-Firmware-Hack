extern const char* log_path;

void on__fmt_muxer_start_record();

void on__fmt_muxer_stop_record();

void on__mov_muxer_prepare_file();

void on__imvt_led_set_rec_start();

void on__imvt_led_set_rec_stop();

void on__imvt_led_set_cap_start();

void on__imvt_led_set_cap_stop();

// address points to internal camera object (imvt_camera_event_0) with info about camera state
void on__imvt_led_camera_event_handler(uint64_t address);

void on__imvt_cam_movie_start(uint32_t stream_style);
void on__imvt_cam_movie_stop(uint32_t stream_style);