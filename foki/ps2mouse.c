#include "ps2mouse.h"
#include "vga.h"
#include "process.h"
#include "kmain.h"
#include "utils.h"
#include "memory.h"

uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];
int8_t mouse_x = 0;
int8_t mouse_y = 0;

// Mouse functions
__attribute__((interrupt))
void ps2MouseISR(struct interruptFrame *frame __attribute__((unused))) {
    SET_DS(0x10);
    SET_ES(0x10);
    switch(mouse_cycle) {
        case 0:
            mouse_byte[0] = inb(0x60);
            if(!(mouse_byte[0] & 0x08)) {
                outb(0xA0, 0x20);
                outb(0x20, 0x20);
                return;
            }
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = inb(0x60);
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = inb(0x60);
            mouse_x = mouse_byte[1];
            mouse_y = mouse_byte[2];
            mouse_cycle = 0;
            if(ownerPID != 0) {
                procMsg msg;
                mouseMovMsg mouseMsg;
                msg.fromPID = 0; // Indicates kernel
                strcpy(mouseMsg.signature, "MOUMOV");
                mouseMsg.x = mouse_x;
                mouseMsg.y = mouse_y;
                mouseMsg.status = mouse_byte[0];
                memcpy(msg.msg, &mouseMsg, sizeof(mouseMovMsg));
                sendMessageToProcess(ownerPID, &msg);
            }
            break;
    }

    outb(0xA0, 0x20);
    outb(0x20, 0x20);
    if(systemInitialized) {
        SET_DS(0x0F);
        SET_ES(0x0F);
    }
}

inline void mouse_wait(uint8_t a_type) {
    uint32_t _time_out=100000;
    if(a_type==0) {
        while(_time_out--) {
            if((inb(0x64) & 1)==1) return;
        }
        return;
    }
    else {
        while(_time_out--) {
            if((inb(0x64) & 2)==0) return;
        }
        return;
    }
}

inline void mouseWrite(uint8_t a_write) {
    // Wait to be able to send a command
    mouse_wait(1);
    // Tell the mouse we are sending a command
    outb(0x64, 0xD4);
    // Wait for the final part
    mouse_wait(1);
    // Finally write
    outb(0x60, a_write);
}

uint8_t mouseRead() {
    // Gets response from mouse
    mouse_wait(0); 
    return inb(0x60);
}

void initPS2Mouse() {
    uint8_t _status;

    // Enable the auxiliary mouse device
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable the interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    _status = (inb(0x60) | 2);
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, _status);

    // Tell the mouse to use default settings
    mouseWrite(0xF6);
    mouseRead();

    // Set the sample rate
    mouseWrite(0xF3);
    mouseRead();

    mouseWrite(200);
    mouseRead();

    // Enable the mouse
    mouseWrite(0xF4);
    mouseRead();
}