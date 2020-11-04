#include <sys/mman.h>
#include <memory.h>

#include "rec_notificator.h"
#include "log.h"
#include "qtgui.h"
#include "hooker.h"

// ###
void *__HdmiPage_handle = NULL;
// Replacement for empty HdmiPage::paintEvent
void HOOK__HdmiPage__paintEvent(QWidget *this, void *event)
{
    //log_print("Entering hook for HdmiPage::paintEvent...\n");

    // if (!imvt_cam_movie_is_recording(1))
    if (!((int64_t(*)(uint64_t))0x6C0B38)(1))
        return;

    if (__HdmiPage_handle == NULL)
        __HdmiPage_handle = this;

    if (__HdmiPage_handle != this)
        log_print("Warning! Found another HdmiPage! Old = 0x%X | New = 0x%X\n", __HdmiPage_handle, this);

    void *paintDevice = &(((uint8_t *)this)[16]); // QPaintDevice ptr

    // Create painter
    QPainter painter;
    QPainter(&painter, paintDevice);

    // Create color
    QColor color;
    QColor(&color, 0xFFFF0000);     // QRgb = uint32_t : #AARRGGBB

    // Settings
    const int line_width = 4;

    // Log window rect
    //log_print("HdmiPage crect : x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", this->data->crect.x1, this->data->crect.y1, this->data->crect.x2, this->data->crect.y2);

    // QRect
    QRect rect;

    // Top line
    rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = this->data->crect.x2;
    rect.y2 = line_width;
    QPainter__fillRect(&painter, &rect, &color);

    // Bottom line
    rect.x1 = 0;
    rect.y1 = this->data->crect.y2 - line_width;
    rect.x2 = this->data->crect.x2;
    rect.y2 = this->data->crect.y2;
    QPainter__fillRect(&painter, &rect, &color);

    // Left line
    rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = line_width;
    rect.y2 = this->data->crect.y2;
    QPainter__fillRect(&painter, &rect, &color);

    // Right line
    rect.x1 = this->data->crect.x2 - line_width;
    rect.y1 = 0;
    rect.x2 = this->data->crect.x2;
    rect.y2 = this->data->crect.y2;
    QPainter__fillRect(&painter, &rect, &color);

    // Destruct painter
    QPainter__destruct(&painter);

    //log_print("Finishing hook for HdmiPage::paintEvent...\n");
}

DECL_HOOK_IMVT_FUNCTION(HdmiPage__paintEvent, 0x5A3070);             // Called on each HdmiPage drawing
void rec_notificator_init()
{
    log_print("Initializing rec_notificator...\n");

    PROC_HOOK_IMVT_FUNCTION(HdmiPage__paintEvent, &HOOK__HdmiPage__paintEvent);

    log_print("Done!\n");
}