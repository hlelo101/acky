#pragma once
#define VIDEO_MEM_ADDR 0xB8000
#include <stdint.h>
#include <stdbool.h>

#define REG_SEQUENCER 0x3C4
#define REG_GRAPHICS 0x3CE
#define REG_CRTC 0x3D4
#define REG_MISC 0x3C2
#define REG_ATTRIBUTE 0x3C0
#define REG_ATTRIBUTE_RESET 0x3DA
#define DATA(reg) ((reg) + 1) // Won't work with the attribute register
#define GRAPHICS_MEM_ADDR 0xA0000

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

typedef struct {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
}__attribute__((packed)) vbeReturn;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint16_t x;
    uint16_t y;
}__attribute__((packed)) pixelInfo;

typedef struct {
    pixelInfo firstPoint;
    uint16_t x;
    uint16_t y;
}__attribute__((packed)) lineInfo;

typedef struct {
    char signature[4]; // 'DREQ'
    uint8_t type; // 0: Pixel; 1: Line; 2: Rectangle
    union {
        pixelInfo pinfo;
        lineInfo linfo;
    } info;
}__attribute__((packed)) drawReq;

extern uint32_t ownerPID;
extern bool ownerSet;
extern uint32_t VBEBufferAddr;
extern vbeReturn VBEInfo;

enum currentMode {
    VGA_MODE_TEXT,
    VGA_MODE_13H,
    VGA_MODE_VBE
};

void printChar(const char c);
void setColorAttribute(const int attr);
void print(const char *str);
void clearScr();
void printInt(const uint32_t integer);
void updateCursorPos(const unsigned int offset);
void disableCursor();
void enableCursor(uint8_t cursor_start, uint8_t cursor_end);
void intToStr(uint32_t num, char *str);

void drawLine(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b);
void enableGraphics();
void putPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void getPixel(int x, int y, pixelInfo *info);
void enableText();
