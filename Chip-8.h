#ifndef CHIP8_H
#define CHIP8_H

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <fstream>
#include <stdint.h>
#include <cassert>
#include <bitset>

#include "to_hex.h"

#define TITLE "CHIP-8 Emulator"

extern bool storeBitShiftResultOfY;
extern bool incrementIAfterMemoryOperation;

extern uint8_t bgColorR;
extern uint8_t bgColorG;
extern uint8_t bgColorB;

extern uint8_t fgColorR;
extern uint8_t fgColorG;
extern uint8_t fgColorb;


class Registers
{
private:
    uint8_t m_registers[16]{};

    bool    m_isRegisterWritten[16]{};
    bool    m_isRegisterRead[16]{};

public:
    uint8_t get(int index, bool isInternal=false)
    {
        assert(index >= 0);
        assert(index < 16);

        if (!isInternal)
            m_isRegisterRead[index] = true;

        return m_registers[index];
    }

    void set(int index, uint8_t value, bool isInternal=false)
    {
        assert(index >= 0);
        assert(index < 16);

        if (!isInternal)
            m_isRegisterWritten[index] = true;

        m_registers[index] = value;
    }

    void clearReadWrittenFlags()
    {
        // Clear m_isRegisterWritten
        memset(m_isRegisterWritten, false, sizeof(m_isRegisterWritten));
        // Clear m_isRegisterRead
        memset(m_isRegisterRead, false, sizeof(m_isRegisterRead));
    }

    bool getIsRegisterWritten(int index)
    {
        return m_isRegisterWritten[index];
    }

    bool getIsRegisterRead(int index)
    {
        return m_isRegisterRead[index];
    }
};

class Framebuffer
{
public:
    int m_frameBuffer[64*32]{};
    
    Framebuffer()
    {}

    int& operator[](int index)
    {
        //assert(index >= 0);
        //assert(index < 32);
        
        bool isOutOfBounds{false};
        
        if (index < 0)
        {
            isOutOfBounds = true;
            std::cout << "Frame buffer index lower than 0" << std::endl;
        }
        if (index >= 64*32)
        {
            isOutOfBounds = true;
            std::cout << "Frame buffer index out of bounds" << std::endl;
        }
        
        if (isOutOfBounds)
            return m_frameBuffer[0];
        
        return m_frameBuffer[index];
    }
    
    void print()
    {
        std::cout << "--- frame buffer ---" << std::endl;
        for (int i{}; i < 64*32; ++i)
        {
            std::cout << m_frameBuffer[i];
            if (!((i+1) % 64))
                std::cout << std::endl;
        }
        std::cout << "--------------------" << std::endl;
    }
};

class Chip8
{
private:
    // stack
    uint16_t stack[16]{};
    // stack pointer
    uint8_t sp          = 0;
    // registers
    Registers registers;
    // memory - 0x00 - 0xfff
    uint8_t memory[0xfff+1]{};
    // program counter - the programs start at 0x200
    uint16_t pc         = 0x200;
    // current opcode
    uint16_t opcode     = 0;
    // index register
    uint16_t I          = 0;
    // delay timer
    uint8_t delayTimer  = 0;
    // sound timer
    uint8_t soundTimer  = 0;
    // framebuffer - stores which pixels are turned on
    // We don't fill it with zeros, because the original implementation doesn't do so
    Framebuffer frameBuffer;

    // rom file size in bytes
    int romSize;

    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};

    TTF_Font *font;

    double scale{1.0};
    bool isFullscreen{false};
    bool isDebugMode{false};
    bool isReadingKey{false};
    bool isCursorShown{true};

    // Helps to decrement the sound and delay timers at 60 FPS
    // This is decremented after every frame and if 0, the timers decremented.
    double timerDecrementCountdown{16.67};

    bool hasDeinitCalled{false};

    
    void loadFile(std::string romFilename);
    void loadFontSet();
    void initVideo();
    
    void clearRenderer();
    void clearDebugInfo();

    void fetchOpcode();
    
    void turnOnFullscreen();
    void turnOffFullscreen();

    void renderText(const std::string &text, int line, int row=0, const SDL_Color &bgColor={0, 0, 0, 100});

    void reportInvalidOpcode(uint8_t opcode);


public:
    Chip8(const std::string &romFilename);
    ~Chip8();
    
    void deinit();

    void emulateCycle();
    void renderFrameBuffer();
    
    inline void updateRenderer() { SDL_RenderPresent(renderer); }
    
    void setDebugTitle();
    void setPaused();
    
    void whenWindowResized(int width, int height);

    void toggleFullscreen();
    void toggleDebugMode();
    void toggleCursor();

    void displayDebugInfoIfInDebugMode();

    uint32_t getWindowID();

    void clearLastRegisterOperationFlags();
    void clearIsReadingKeyStateFlag();

    bool hasEnded{false}; // marks whether the program ended
    // Marks whether we need to redraw the framebuffer
    bool renderFlag = true;
};

#endif // CHIP8_H
