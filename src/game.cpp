#include "game.h"

// Input class implementation
std::uint16_t Input::current_keys = 0;
std::uint16_t Input::previous_keys = 0;

void Input::update() {
    previous_keys = current_keys;
    current_keys = ~REG_KEYINPUT & 0x3FF;  // Invert bits (pressed = 0 on GBA)
}

bool Input::isPressed(std::uint16_t key) {
    return (current_keys & key) && !(previous_keys & key);
}

bool Input::isHeld(std::uint16_t key) {
    return current_keys & key;
}

bool Input::isReleased(std::uint16_t key) {
    return !(current_keys & key) && (previous_keys & key);
}

// GBADisplay implementation
GBADisplay::GBADisplay() : video_buffer(VIDEO_BUFFER) {
    setMode(MODE_3 | BG2_ENABLE);
}

void GBADisplay::setMode(std::uint32_t mode) {
    REG_DISPCNT = mode;
}

void GBADisplay::plotPixel(int x, int y, std::uint16_t color) const {
    if (isValidPixel(x, y)) {
        video_buffer[y * SCREEN_WIDTH + x] = color;
    }
}

void GBADisplay::clearScreen(std::uint16_t color) {
    GBAUtils::fastMemSet16(video_buffer, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void GBADisplay::drawRect(int x, int y, int width, int height, std::uint16_t color) {
    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            plotPixel(x + dx, y + dy, color);
        }
    }
}

void GBADisplay::drawRectOutline(int x, int y, int width, int height, std::uint16_t color) {
    // Top and bottom lines
    for (int dx = 0; dx < width; dx++) {
        plotPixel(x + dx, y, color);
        plotPixel(x + dx, y + height - 1, color);
    }
    // Left and right lines
    for (int dy = 1; dy < height - 1; dy++) {
        plotPixel(x, y + dy, color);
        plotPixel(x + width - 1, y + dy, color);
    }
}

void GBADisplay::drawLine(int x1, int y1, int x2, int y2, std::uint16_t color) {
    // Bresenham's line algorithm
    int dx = x2 - x1;
    int dy = y2 - y1;
    int dx_abs = (dx < 0) ? -dx : dx;
    int dy_abs = (dy < 0) ? -dy : dy;
    int x_inc = (dx < 0) ? -1 : 1;
    int y_inc = (dy < 0) ? -1 : 1;
    
    int x = x1, y = y1;
    
    if (dx_abs >= dy_abs) {
        int error = dy_abs - dx_abs / 2;
        for (int i = 0; i <= dx_abs; i++) {
            plotPixel(x, y, color);
            if (error >= 0) {
                y += y_inc;
                error -= dx_abs;
            }
            error += dy_abs;
            x += x_inc;
        }
    } else {
        int error = dx_abs - dy_abs / 2;
        for (int i = 0; i <= dy_abs; i++) {
            plotPixel(x, y, color);
            if (error >= 0) {
                x += x_inc;
                error -= dy_abs;
            }
            error += dx_abs;
            y += y_inc;
        }
    }
}

void GBADisplay::drawCircle(int centerX, int centerY, int radius, std::uint16_t color) {
    // Bresenham's circle algorithm
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (x <= y) {
        // Draw 8 symmetric points
        plotPixel(centerX + x, centerY + y, color);
        plotPixel(centerX - x, centerY + y, color);
        plotPixel(centerX + x, centerY - y, color);
        plotPixel(centerX - x, centerY - y, color);
        plotPixel(centerX + y, centerY + x, color);
        plotPixel(centerX - y, centerY + x, color);
        plotPixel(centerX + y, centerY - x, color);
        plotPixel(centerX - y, centerY - x, color);
        
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

bool GBADisplay::isValidPixel(int x, int y) const {
    return x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT;
}

std::uint16_t GBADisplay::getPixel(int x, int y) const {
    if (isValidPixel(x, y)) {
        return video_buffer[y * SCREEN_WIDTH + x];
    }
    return 0;
}

// BitmapFont implementation - Basic ASCII font data
const std::uint8_t BitmapFont::font_data[95][8] = {
    // Space (32)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    // ! (33)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},
    // " (34)
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    // # (35)
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},
    // $ (36)
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},
    // % (37)
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},
    // & (38)
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},
    // ' (39)
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
    // ( (40)
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},
    // ) (41)
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},
    // * (42)
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},
    // + (43)
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},
    // , (44)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x00},
    // - (45)
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},
    // . (46)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},
    // / (47)
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},
    // 0-9 and A-Z would continue here...
    // For brevity, I'll include just a few key characters
    // 0 (48)
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},
    // 1 (49)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},
    // A (65)
    {0x1C, 0x36, 0x63, 0x7F, 0x63, 0x63, 0x63, 0x00},
    // E (69)
    {0x7F, 0x60, 0x60, 0x7E, 0x60, 0x60, 0x7F, 0x00},
    // H (72)
    {0x63, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x63, 0x00},
    // L (76)
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7F, 0x00},
    // O (79)
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},
    // ... (add more characters as needed)
};

void BitmapFont::drawChar(const GBADisplay& display, char c, int x, int y, std::uint16_t color) {
    if (c < 32 || c > 126) return;  // Only printable ASCII
    
    int index = c - 32;
    for (int row = 0; row < CHAR_HEIGHT; row++) {
        for (int col = 0; col < CHAR_WIDTH; col++) {
            if (font_data[index][row] & (1 << (7 - col))) {
                display.plotPixel(x + col, y + row, color);
            }
        }
    }
}

void BitmapFont::drawString(const GBADisplay& display, const char* str, int x, int y, std::uint16_t color) {
    int currentX = x;
    while (*str) {
        drawChar(display, *str, currentX, y, color);
        currentX += CHAR_WIDTH + CHAR_SPACING;
        str++;
    }
}

int BitmapFont::getStringWidth(const char* str) {
    int length = 0;
    while (*str++) length++;
    return length * (CHAR_WIDTH + CHAR_SPACING) - CHAR_SPACING;
}

// GameObject implementation
bool GameObject::collidesWith(const GameObject& other, int width, int height) const {
    int x1 = position.x.toInt();
    int y1 = position.y.toInt();
    int x2 = other.position.x.toInt();
    int y2 = other.position.y.toInt();
    
    return (x1 < x2 + width && x1 + width > x2 && 
            y1 < y2 + height && y1 + height > y2);
}

// Particle implementation
Particle::Particle(int x, int y, FixedPoint vx, FixedPoint vy, std::uint16_t c, int life) 
    : GameObject(x, y), color(c), lifetime(life), max_lifetime(life) {
    velocity = Vec2(vx, vy);
}

void Particle::update() {
    if (!active) return;
    
    position += velocity;
    lifetime--;
    
    if (lifetime <= 0) {
        active = false;
    }
    
    // Simple gravity effect
    velocity.y += FixedPoint(0.1f);
}

void Particle::render(const GBADisplay& display) {
    if (!active) return;
    
    // Fade color based on lifetime
    int alpha = (lifetime * 31) / max_lifetime;
    std::uint16_t faded_color = RGB15(
        ((color >> 0) & 0x1F) * alpha / 31,
        ((color >> 5) & 0x1F) * alpha / 31,
        ((color >> 10) & 0x1F) * alpha / 31
    );
    
    display.plotPixel(position.x.toInt(), position.y.toInt(), faded_color);
}

// GBAUtils implementation
namespace GBAUtils {
    static std::uint32_t rng_state = 1;
    
    void seedRandom(std::uint32_t seed) {
        rng_state = seed;
    }
    
    std::uint32_t random() {
        // Simple Linear Congruential Generator
        rng_state = rng_state * 1664525 + 1013904223;
        return rng_state;
    }
    
    int randomRange(int min, int max) {
        return min + (random() % (max - min + 1));
    }
    
    void waitForVBlank() {
        while (REG_VCOUNT < 160);  // Wait for VBlank
        while (REG_VCOUNT >= 160); // Wait for VBlank to end
    }
    
    void delay(int frames) {
        for (int i = 0; i < frames; i++) {
            waitForVBlank();
        }
    }
    
    void fastMemSet16(volatile std::uint16_t* dest, std::uint16_t value, int count) {
        for (int i = 0; i < count; i++) {
            dest[i] = value;
        }
    }
    
    void fastMemCopy16(volatile std::uint16_t* dest, const std::uint16_t* src, int count) {
        for (int i = 0; i < count; i++) {
            dest[i] = src[i];
        }
    }
}

// GBAGame implementation
GBAGame::GBAGame() : current_state(GameState::TITLE_SCREEN), frame_counter(0) {
    // Initialize random number generator with a simple seed
    GBAUtils::seedRandom(12345);
}

void GBAGame::run() {
    initialize();
    
    while (true) {
        Input::update();
        handleInput();
        update();
        
        GBAUtils::waitForVBlank();
        render();
        
        frame_counter++;
    }
}