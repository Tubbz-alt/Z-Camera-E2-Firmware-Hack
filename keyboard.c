#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>

#include "keyboard.h"
#include "log.h"

typedef struct input_event input_event;

pthread_mutex_t __keyboard_thread_mutex;
int __keyboard_mutexed_state;

void *keyboard_handler_thread(void *param)
{
    int fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    if (fd == -1)
    {
        log_print("[keyboard_handler_thread] Failed to open /dev/input/event0!\n");

        pthread_mutex_lock(&__keyboard_thread_mutex);
        __keyboard_mutexed_state = -1;
        pthread_mutex_unlock(&__keyboard_thread_mutex);

        return NULL;
    }

    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 0;
    while (1)
    {
        // Get async state
        int state;
        pthread_mutex_lock(&__keyboard_thread_mutex);
        state = __keyboard_mutexed_state;
        pthread_mutex_unlock(&__keyboard_thread_mutex);

        // Exit if smb asked for it
        if (state == 1)
            return NULL;

        input_event event;
        read(fd, &event, sizeof(input_event));

        if (time.tv_sec != event.time.tv_sec || time.tv_usec != event.time.tv_usec)
        {
            // HANDLE KEY HERE
            //log_print("[keyboard_handler_thread] code = %d, time = %d, type = %d, value = %d\n", event.code, event.time, event.type, event.value);
            time = event.time;

            switch (event.code)
            {
            case KEYBOARD_KEY_MENU:
                break;
            case KEYBOARD_KEY_FN:
                break;
            case KEYBOARD_KEY_UP:
                break;
            case KEYBOARD_KEY_DOWN:
                break;
            case KEYBOARD_KEY_OK:
                break;
            case KEYBOARD_KEY_REC:
                break;
            case KEYBOARD_KEY_F1:
                break;
            case KEYBOARD_KEY_F2:
                break;
            case KEYBOARD_KEY_F3:
                break;
            case KEYBOARD_KEY_POWER:
                break;
            default:
                break;
            }

            // MENU button:
            // [keyboard_handler_thread] code = 139, time = 1604790363, type = 655100, value = 1
            // [keyboard_handler_thread] code = 139, time = 1604790363, type = 801078, value = 1

            // FN button:
            // [keyboard_handler_thread] code = 62, time = 1604790428, type = 73082, value = 1
            // [keyboard_handler_thread] code = 62, time = 1604790428, type = 275073, value = 1
            // [keyboard_handler_thread] code = 62, time = 1604790432, type = 11080, value = 1
            // [keyboard_handler_thread] code = 62, time = 1604790432, type = 116105, value = 1

            // UP button:
            // [keyboard_handler_thread] code = 103, time = 1604790526, type = 588087, value = 1
            // [keyboard_handler_thread] code = 103, time = 1604790526, type = 735077, value = 1

            // DOWN button:
            // [keyboard_handler_thread] code = 108, time = 1604790568, type = 151069, value = 1
            // [keyboard_handler_thread] code = 108, time = 1604790568, type = 246071, value = 1

            // OK button:
            // [keyboard_handler_thread] code = 28, time = 1604790604, type = 225091, value = 1
            // [keyboard_handler_thread] code = 28, time = 1604790604, type = 340072, value = 1

            // REC button:
            // [keyboard_handler_thread] code = 57, time = 1604790638, type = 766083, value = 1
            // [keyboard_handler_thread] code = 57, time = 1604790638, type = 976075, value = 1

            // F1 button:
            // [keyboard_handler_thread] code = 59, time = 1604790685, type = 992087, value = 1
            // [keyboard_handler_thread] code = 59, time = 1604790686, type = 228081, value = 1

            // F2 button:
            // [keyboard_handler_thread] code = 60, time = 1604790716, type = 293084, value = 1
            // [keyboard_handler_thread] code = 60, time = 1604790716, type = 506095, value = 1

            // F3 button:
            // [keyboard_handler_thread] code = 61, time = 1604790738, type = 562095, value = 1
            // [keyboard_handler_thread] code = 61, time = 1604790740, type = 943081, value = 1

            // POWER button:
            // [keyboard_handler_thread] code = 116, time = 1604790765, type = 854104, value = 1
            // [keyboard_handler_thread] code = 116, time = 1604790766, type = 34084, value = 1
        }
    }

    return NULL;
}

pthread_t __keyboard_tid = 0;
pthread_attr_t __keyboard_tattr;

void keyboard_init()
{
    // If already running - exit
    if (__keyboard_tid != 0)
    {
        int code;
        pthread_mutex_lock(&__keyboard_thread_mutex);
        code = __keyboard_mutexed_state;
        pthread_mutex_unlock(&__keyboard_thread_mutex);

        if (code != -1)
            return;

        keyboard_deinit();
    }

    // Init thread state
    __keyboard_mutexed_state = 0;

    // Init mutex
    if (pthread_mutex_init(&__keyboard_thread_mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return;
    }

    // Create thread
    pthread_create(&__keyboard_tid, &__keyboard_tattr, keyboard_handler_thread, NULL);
}

void keyboard_deinit()
{
    // If not running - exit
    if (__keyboard_tid == 0)
        return;

    // Set flag to exit thread
    pthread_mutex_lock(&__keyboard_thread_mutex);
    __keyboard_mutexed_state = 1;
    pthread_mutex_unlock(&__keyboard_thread_mutex);

    // Await for thread to stop
    pthread_join(__keyboard_tid, NULL);

    // Remove tid
    __keyboard_tid = 0;

    // Destroy mutex
    pthread_mutex_destroy(&__keyboard_thread_mutex);
}