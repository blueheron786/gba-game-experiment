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
    // Initialize display
    GBADisplay display;
    
    int frame_counter = 0;
    
    // Main game loop
    while (true) {
        frame_counter++;
        
        // Clear screen to dark blue
        display.clearScreen(RGB15(0, 0, 10));
        
        // Test 1: Draw some pixels to verify basic pixel plotting works
        display.plotPixel(10, 10, COLOR_WHITE);
        display.plotPixel(11, 10, COLOR_RED);
        display.plotPixel(12, 10, COLOR_GREEN);
        display.plotPixel(13, 10, COLOR_BLUE);
        display.plotPixel(14, 10, COLOR_YELLOW);
        
        // Test 2: Draw a static rectangle
        display.drawRect(50, 50, 40, 30, COLOR_RED);
        
        // Test 3: Draw a moving rectangle
        int moving_x = 100 + static_cast<int>(60.0f * std::sin(frame_counter * 0.03f));
        display.drawRect(moving_x, 80, 20, 20, COLOR_YELLOW);
        
        // Test 4: Draw some lines
        display.drawLine(0, 0, 100, 0, COLOR_WHITE);
        display.drawLine(0, 0, 0, 100, COLOR_WHITE);
        
        // Test 5: Draw animated dots showing frame counter
        int dots = (frame_counter / 20) % 30;
        for (int i = 0; i < dots; i++) {
            display.plotPixel(200 + i, 20, COLOR_MAGENTA);
        }
        
        // Test 6: Draw a pattern
        for (int i = 0; i < 20; i++) {
            display.plotPixel(150 + i, 120, COLOR_GREEN);
            display.plotPixel(150, 120 + i, COLOR_GREEN);
        }
        
        // Frame rate control
        for (volatile int i = 0; i < 10000; i++) {
            // Busy wait
        }
    }
    
    return 0;
}
