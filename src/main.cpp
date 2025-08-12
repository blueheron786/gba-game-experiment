#include <cstdint>
#include <cmath>

// GBA specific definitions
#define REG_DISPCNT *reinterpret_cast<volatile std::uint32_t*>(0x4000000)
#define MODE_3 0x3
#define BG2_ENABLE 0x400

constexpr int SCREEN_WIDTH = 240;
constexpr int SCREEN_HEIGHT = 160;
#define VIDEO_BUFFER (reinterpret_cast<volatile std::uint16_t*>(0x6000000))

// 15-bit color utility function
constexpr std::uint16_t RGB15(int r, int g, int b) {
    return static_cast<std::uint16_t>((r) | ((g) << 5) | ((b) << 10));
}

// Color constants
constexpr std::uint16_t COLOR_WHITE = RGB15(31, 31, 31);
constexpr std::uint16_t COLOR_BLACK = RGB15(0, 0, 0);
constexpr std::uint16_t COLOR_RED = RGB15(31, 0, 0);
constexpr std::uint16_t COLOR_GREEN = RGB15(0, 31, 0);
constexpr std::uint16_t COLOR_BLUE = RGB15(0, 0, 31);
constexpr std::uint16_t COLOR_YELLOW = RGB15(31, 31, 0);
constexpr std::uint16_t COLOR_MAGENTA = RGB15(31, 0, 31);

// Simple GBA display class
class GBADisplay {
private:
    volatile std::uint16_t* video_buffer;

public:
    GBADisplay() : video_buffer(VIDEO_BUFFER) {
        REG_DISPCNT = MODE_3 | BG2_ENABLE;
    }

    void plotPixel(int x, int y, std::uint16_t color) {
        if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
            video_buffer[y * SCREEN_WIDTH + x] = color;
        }
    }

    void clearScreen(std::uint16_t color) {
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            video_buffer[i] = color;
        }
    }

    void drawRect(int x, int y, int width, int height, std::uint16_t color) {
        for (int dy = 0; dy < height; dy++) {
            for (int dx = 0; dx < width; dx++) {
                plotPixel(x + dx, y + dy, color);
            }
        }
    }

    void drawLine(int x1, int y1, int x2, int y2, std::uint16_t color) {
        // Simple horizontal/vertical line drawing
        if (y1 == y2) {  // Horizontal line
            int startX = (x1 < x2) ? x1 : x2;
            int endX = (x1 < x2) ? x2 : x1;
            for (int x = startX; x <= endX; x++) {
                plotPixel(x, y1, color);
            }
        } else if (x1 == x2) {  // Vertical line
            int startY = (y1 < y2) ? y1 : y2;
            int endY = (y1 < y2) ? y2 : y1;
            for (int y = startY; y <= endY; y++) {
                plotPixel(x1, y, color);
            }
        }
    }
};

extern "C" int main() {
    // Set GBA to Mode 3 (240x160 bitmap mode)
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    
    // Fill the entire screen with bright red to test
    volatile std::uint16_t* vram = VIDEO_BUFFER;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vram[i] = RGB15(31, 0, 0);  // Bright red
    }
    
    // Simple infinite loop to keep the program running
    while (true) {
        // Do nothing, just keep the red screen displayed
        for (volatile int i = 0; i < 100000; i++) {
            // Busy wait
        }
    }
    
    return 0;
}
