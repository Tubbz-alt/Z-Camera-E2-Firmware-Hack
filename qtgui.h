#include <stdint.h>

/*

Here are functions, that are used from Qt libraries:

libQtCore.so
libQtGui.so
libQtNetwork.so

>> we need to use mangled names to work with there libraries from C code

*/

// QPainter type representation
#pragma pack(1)
typedef struct
{
    uint64_t d_ptr;
} QPainter;
#pragma pack()

// QColor type representation
#pragma pack(1)
typedef struct
{
    uint8_t __data[0x10];   // sizeof(QColor) == 0x10
} QColor;
#pragma pack()

// QRect type representation
#pragma pack(1)
typedef struct
{
    uint32_t x1;
    uint32_t y1;
    uint32_t x2;
    uint32_t y2;
} QRect;
#pragma pack()

// QWidget_data type representation
#pragma pack(1)
typedef struct
{
    uint8_t __data0[20];    // Unknown data
    QRect crect;            // QWidget rect (size)
} QWidget_data;
#pragma pack()

// QWidget type representation
#pragma pack(1)
typedef struct
{
    uint8_t __data0[32];    // Unknown data
    QWidget_data* data;     // QWidget internal data
    // ...
} QWidget;
#pragma pack()

// QPainter::QPainter(QPainter* this, QPaintDevice*)
typedef struct {} QPaintDevice;
void _ZN8QPainterC2EP12QPaintDevice(QPainter* __this, QPaintDevice* device);
#define QPainter(this, device) _ZN8QPainterC2EP12QPaintDevice(this, device)

// QColor::QColor(QColor *this, unsigned int)
void _ZN6QColorC2Ej(QColor* __this, unsigned int color);
#define QColor(this, color) _ZN6QColorC2Ej(this, color)

// QPainter::fillRect(QPainter* this, QRect const&, QColor const&)
void _ZN8QPainter8fillRectERK5QRectRK6QColor(QPainter* __this, const QRect* rect, const QColor* color);
#define QPainter__fillRect(this, rect, color) _ZN8QPainter8fillRectERK5QRectRK6QColor(this, rect, color)

void _ZN8QPainterD2Ev(QPainter* __this);
#define QPainter__destruct(this) _ZN8QPainterD2Ev(this)