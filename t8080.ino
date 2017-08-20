/**************************************************************
 * Arduino TINY BASIC with a 8080 Emulation
 * 
 * Arduino 8080 Virtual Machine with up to 4kB ROM (0x0000-0x0fff) and 1 kB RAM (0x1000-0x13FF)
 * 
 * OUT 1 sends data to serial port
 * IN 1 reads data from serial port
 * OUT 0xFE controls LED on pin13 (bit 0)
 * IN 0 returns serial status: 0x02 for no data on input buffer, 0x22 means data are available
 * 
 * 8080 emulator part is Copyright (C) 2012 Alexander Demin <alexander@demin.ws> under GPL2
 * Tiny Basic and Tiny Basic 2 are copylefted by LI-CHEN WANG
 * Source code for BASIC has been compiled with www.asm80.com
 * 
 */


#include "i8080.h"
#include "i8080_hal.h"

void setup() {
    Serial.begin(115200);
    pinMode(13, OUTPUT);
    i8080_init();
    i8080_jump(0);
}

// for debug purposes
void examine() {
    Serial.print("\nA:");
    Serial.print(i8080_regs_a());
    Serial.print(" BC:");
    Serial.print(i8080_regs_bc());
    Serial.print(" DE:");
    Serial.print(i8080_regs_de());
    Serial.print(" HL:");
    Serial.print(i8080_regs_hl());
    Serial.print(" PC:");
    Serial.print(i8080_pc());
    //Serial.print("\n");
}

void loop() {
    //delay(500);
    //examine();
    i8080_instruction();
}

//// MEMORY DEFINITIONS

//test for 8080 emu
//const byte PROGMEM  ROM[4096] = {0x3e, 0x20, 0xd3, 0x01, 0xd3, 0xfe, 0x3c, 0xc3, 0x02,0x00};

//Uncomment for TINY BASIC v1
//#include "basic.h"

//Uncomment for TINY BASIC v2
#include "basic2.h"

//some initial RAM constants for Tiny BASIC 1
byte RAM [1024] = {0xff,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x15,0x10};

byte readByte(int addr) {
    if (addr>=0 && addr < 0x1000) return pgm_read_byte_near(ROM + addr);
    if (addr>=0x1000 && addr < 0x1400) return RAM[addr];
    return 0xFF; //void memory
}

void writeByte(int addr, byte value) {
    if (addr>=0x1000 && addr < 0x1400) RAM[addr]=value;
}

//// HAL - Hardware Abstraction Layer for Emulator

int i8080_hal_memory_read_byte(int addr) {
    return (int)readByte(addr);
}

void i8080_hal_memory_write_byte(int addr, int value) {
    writeByte(addr,value);
}


int i8080_hal_memory_read_word(int addr) {
    return readByte(addr) | (readByte(addr + 1) << 8);
}

void i8080_hal_memory_write_word(int addr, int value) {
    writeByte(addr, value & 0xff);
    writeByte(addr + 1, (value >> 8) & 0xff);
}

int i8080_hal_io_input(int port) {
    switch (port) {
    case 0x00: //serial status
        return Serial.available() ? 0x22 : 0x02;
        break;
    case 0x01: //serial input
        return Serial.available() ? Serial.read() : 0;
        break;
    default:
        return 0xff;
    }
}

void i8080_hal_io_output(int port, int value) {
    switch (port) {
    case 0x01: //serial out
        Serial.print((char)(value & 0x7f));
        break;
    case 0xfe: //led control
        digitalWrite(13, value & 0x01);
        break;
    default:
        break;
    }
}

void i8080_hal_iff(int on) {
    //no interrupts implemented
}
