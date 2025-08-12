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

    void plotPixel(int x, int y, std::uint16_t color) const {
        if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
            video_buffer[y * SCREEN_WIDTH + x] = color;
        }
    }

    void clearScreen(std::uint16_t color) const {
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            video_buffer[i] = color;
        }
    }

    void drawRect(int x, int y, int width, int height, std::uint16_t color) const {
        for (int dy = 0; dy < height; dy++) {
            for (int dx = 0; dx < width; dx++) {
                plotPixel(x + dx, y + dy, color);
            }
        }
    }

    void drawLine(int x1, int y1, int x2, int y2, std::uint16_t color) const {
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

// Simple bitmap font class
class BitmapFont {
private:
    // Font data for letters (8x8 pixels each)
    static constexpr std::uint8_t font_data[26][8] = {
        // A
        {0b00111100, 0b01100110, 0b11000011, 0b11000011, 0b11111111, 0b11000011, 0b11000011, 0b00000000},
        // B
        {0b11111100, 0b11000110, 0b11000110, 0b11111100, 0b11000110, 0b11000110, 0b11111100, 0b00000000},
        // C
        {0b00111100, 0b01100110, 0b11000000, 0b11000000, 0b11000000, 0b01100110, 0b00111100, 0b00000000},
        // D
        {0b11111000, 0b11001100, 0b11000110, 0b11000110, 0b11000110, 0b11001100, 0b11111000, 0b00000000},
        // E
        {0b11111111, 0b11000000, 0b11000000, 0b11111100, 0b11000000, 0b11000000, 0b11111111, 0b00000000},
        // F
        {0b11111111, 0b11000000, 0b11000000, 0b11111100, 0b11000000, 0b11000000, 0b11000000, 0b00000000},
        // G
        {0b00111100, 0b01100110, 0b11000000, 0b11001111, 0b11000011, 0b01100110, 0b00111100, 0b00000000},
        // H
        {0b11000011, 0b11000011, 0b11000011, 0b11111111, 0b11000011, 0b11000011, 0b11000011, 0b00000000},
        // I
        {0b01111110, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b01111110, 0b00000000},
        // J
        {0b00001111, 0b00000110, 0b00000110, 0b00000110, 0b11000110, 0b11000110, 0b01111100, 0b00000000},
        // K
        {0b11000011, 0b11000110, 0b11001100, 0b11111000, 0b11001100, 0b11000110, 0b11000011, 0b00000000},
        // L
        {0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11000000, 0b11111111, 0b00000000},
        // M
        {0b11000011, 0b11100111, 0b11111111, 0b11011011, 0b11000011, 0b11000011, 0b11000011, 0b00000000},
        // N
        {0b11000011, 0b11100011, 0b11110011, 0b11011011, 0b11001111, 0b11000111, 0b11000011, 0b00000000},
        // O
        {0b00111100, 0b01100110, 0b11000011, 0b11000011, 0b11000011, 0b01100110, 0b00111100, 0b00000000},
        // P
        {0b11111100, 0b11000110, 0b11000110, 0b11111100, 0b11000000, 0b11000000, 0b11000000, 0b00000000},
        // Q
        {0b00111100, 0b01100110, 0b11000011, 0b11000011, 0b11011011, 0b01100110, 0b00111101, 0b00000000},
        // R
        {0b11111100, 0b11000110, 0b11000110, 0b11111100, 0b11001100, 0b11000110, 0b11000011, 0b00000000},
        // S
        {0b01111110, 0b11000000, 0b11000000, 0b01111100, 0b00000011, 0b00000011, 0b11111110, 0b00000000},
        // T
        {0b11111111, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00000000},
        // U
        {0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b01111110, 0b00000000},
        // V
        {0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b01100110, 0b00111100, 0b00011000, 0b00000000},
        // W
        {0b11000011, 0b11000011, 0b11000011, 0b11011011, 0b11111111, 0b11100111, 0b11000011, 0b00000000},
        // X
        {0b11000011, 0b01100110, 0b00111100, 0b00011000, 0b00111100, 0b01100110, 0b11000011, 0b00000000},
        // Y
        {0b11000011, 0b11000011, 0b01100110, 0b00111100, 0b00011000, 0b00011000, 0b00011000, 0b00000000},
        // Z
        {0b11111111, 0b00000110, 0b00001100, 0b00011000, 0b00110000, 0b01100000, 0b11111111, 0b00000000}
    };

public:
    static void drawChar(const GBADisplay& display, char c, int x, int y, std::uint16_t color) {
        if (c < 'A' || c > 'Z') return;
        
        int index = c - 'A';
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (font_data[index][row] & (1 << (7 - col))) {
                    display.plotPixel(x + col, y + row, color);
                }
            }
        }
    }

    static void drawString(const GBADisplay& display, const char* str, int x, int y, std::uint16_t color) {
        int currentX = x;
        while (*str) {
            if (*str >= 'a' && *str <= 'z') {
                drawChar(display, *str - 32, currentX, y, color);  // Convert to uppercase
            } else if (*str >= 'A' && *str <= 'Z') {
                drawChar(display, *str, currentX, y, color);
            }
            currentX += 10;  // 8 pixels + 2 pixel spacing
            str++;
        }
    }
};

// Simple animation class
class HelloAnimation {
private:
    const GBADisplay& display;
    int frame_counter;
    
public:
    HelloAnimation(const GBADisplay& disp) : display(disp), frame_counter(0) {}

    void update() {
        frame_counter++;
        display.clearScreen(COLOR_BLACK);
        int base_x = (SCREEN_WIDTH - (5 * 10)) / 2;  // Center "HELLO"
        int base_y = SCREEN_HEIGHT / 2 - 4;
        const char* text = "HELLO";
        for (int i = 0; i < 5; i++) {
            int wave_offset = static_cast<int>(10.0f * std::sin((frame_counter + i * 8) * 0.1f));
            std::uint16_t color = (i % 2 == 0) ? COLOR_WHITE : COLOR_YELLOW;
            BitmapFont::drawChar(display, text[i], base_x + i * 10, base_y + wave_offset, color);
        }
        BitmapFont::drawString(display, "WORLD", base_x - 5, base_y + 30, COLOR_GREEN);
        for (int i = 0; i < 8; i++) {
            int x = 20 + i * 25;
            int y = 20 + static_cast<int>(15.0f * std::sin((frame_counter + i * 10) * 0.15f));
            display.drawRect(x, y, 4, 4, COLOR_RED);
            y = 120 + static_cast<int>(10.0f * std::cos((frame_counter + i * 12) * 0.12f));
            display.drawRect(x, y, 4, 4, COLOR_BLUE);
        }
        std::uint16_t border_color = ((frame_counter / 30) % 2) ? COLOR_MAGENTA : COLOR_BLACK;
        display.drawLine(0, 0, SCREEN_WIDTH-1, 0, border_color);
        display.drawLine(0, 0, 0, SCREEN_HEIGHT-1, border_color);
        display.drawLine(SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, border_color);
        display.drawLine(0, SCREEN_HEIGHT-1, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, border_color);
    }
};

int main() {
    // Initialize display
    GBADisplay display;
    HelloAnimation animation(display);
    
    // Main game loop
    while (true) {
        animation.update();
        
        // Simple frame rate control (very basic delay)
        for (volatile int i = 0; i < 1000; i++) {
            // Busy wait to slow down animation
        }
    }
    
    return 0;
}