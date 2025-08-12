#ifndef GAME_H
#define GAME_H

#include <cstdint>

// GBA Hardware Registers
#define REG_DISPCNT     *reinterpret_cast<volatile std::uint32_t*>(0x4000000)
#define REG_DISPSTAT    *reinterpret_cast<volatile std::uint16_t*>(0x4000004)
#define REG_VCOUNT      *reinterpret_cast<volatile std::uint16_t*>(0x4000006)

// Input registers
#define REG_KEYINPUT    *reinterpret_cast<volatile std::uint16_t*>(0x4000130)
#define REG_KEYCNT      *reinterpret_cast<volatile std::uint16_t*>(0x4000132)

// Display modes
#define MODE_0          0x0000
#define MODE_1          0x0001
#define MODE_2          0x0002
#define MODE_3          0x0003
#define MODE_4          0x0004
#define MODE_5          0x0005

// Background enable bits
#define BG0_ENABLE      0x0100
#define BG1_ENABLE      0x0200
#define BG2_ENABLE      0x0400
#define BG3_ENABLE      0x0800

// Object enable bit
#define OBJ_ENABLE      0x1000

// Screen dimensions
constexpr int SCREEN_WIDTH = 240;
constexpr int SCREEN_HEIGHT = 160;

// Video memory
#define VRAM            reinterpret_cast<volatile std::uint16_t*>(0x6000000)
#define VIDEO_BUFFER    reinterpret_cast<volatile std::uint16_t*>(0x6000000)

// Input key definitions
#define KEY_A           0x0001
#define KEY_B           0x0002
#define KEY_SELECT      0x0004
#define KEY_START       0x0008
#define KEY_RIGHT       0x0010
#define KEY_LEFT        0x0020
#define KEY_UP          0x0040
#define KEY_DOWN        0x0080
#define KEY_R           0x0100
#define KEY_L           0x0200

// Color utility functions
constexpr std::uint16_t RGB15(int r, int g, int b) {
    return static_cast<std::uint16_t>((r & 0x1F) | ((g & 0x1F) << 5) | ((b & 0x1F) << 10));
}

constexpr std::uint16_t RGB8_TO_RGB15(int r, int g, int b) {
    return RGB15(r >> 3, g >> 3, b >> 3);
}

// Common colors
constexpr std::uint16_t COLOR_BLACK     = RGB15(0, 0, 0);
constexpr std::uint16_t COLOR_WHITE     = RGB15(31, 31, 31);
constexpr std::uint16_t COLOR_RED       = RGB15(31, 0, 0);
constexpr std::uint16_t COLOR_GREEN     = RGB15(0, 31, 0);
constexpr std::uint16_t COLOR_BLUE      = RGB15(0, 0, 31);
constexpr std::uint16_t COLOR_YELLOW    = RGB15(31, 31, 0);
constexpr std::uint16_t COLOR_MAGENTA   = RGB15(31, 0, 31);
constexpr std::uint16_t COLOR_CYAN      = RGB15(0, 31, 31);
constexpr std::uint16_t COLOR_GRAY      = RGB15(16, 16, 16);
constexpr std::uint16_t COLOR_DARK_GRAY = RGB15(8, 8, 8);

// Math utilities (since GBA doesn't have floating point)
class FixedPoint {
private:
    static constexpr int SHIFT = 8;
    static constexpr int SCALE = 1 << SHIFT;
    
    std::int32_t value;

public:
    FixedPoint() : value(0) {}
    FixedPoint(int i) : value(i << SHIFT) {}
    FixedPoint(float f) : value(static_cast<std::int32_t>(f * SCALE)) {}
    
    int toInt() const { return value >> SHIFT; }
    float toFloat() const { return static_cast<float>(value) / SCALE; }
    
    FixedPoint operator+(const FixedPoint& other) const {
        FixedPoint result;
        result.value = value + other.value;
        return result;
    }
    
    FixedPoint operator-(const FixedPoint& other) const {
        FixedPoint result;
        result.value = value - other.value;
        return result;
    }
    
    FixedPoint operator*(const FixedPoint& other) const {
        FixedPoint result;
        result.value = (static_cast<std::int64_t>(value) * other.value) >> SHIFT;
        return result;
    }
    
    FixedPoint& operator+=(const FixedPoint& other) {
        value += other.value;
        return *this;
    }
};

// Simple vector class for 2D positions
struct Vec2 {
    FixedPoint x, y;
    
    Vec2() : x(0), y(0) {}
    Vec2(int x_, int y_) : x(x_), y(y_) {}
    Vec2(FixedPoint x_, FixedPoint y_) : x(x_), y(y_) {}
    
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

// Input handling class
class Input {
private:
    static std::uint16_t current_keys;
    static std::uint16_t previous_keys;
    
public:
    static void update();
    static bool isPressed(std::uint16_t key);
    static bool isHeld(std::uint16_t key);
    static bool isReleased(std::uint16_t key);
};

// Display management class
class GBADisplay {
private:
    volatile std::uint16_t* video_buffer;

public:
    GBADisplay();
    
    // Basic drawing functions
    void setMode(std::uint32_t mode);
    void plotPixel(int x, int y, std::uint16_t color) const;
    void clearScreen(std::uint16_t color);
    void drawRect(int x, int y, int width, int height, std::uint16_t color);
    void drawRectOutline(int x, int y, int width, int height, std::uint16_t color);
    void drawLine(int x1, int y1, int x2, int y2, std::uint16_t color);
    void drawCircle(int centerX, int centerY, int radius, std::uint16_t color);
    
    // Advanced drawing
    void drawSprite(const std::uint16_t* sprite_data, int x, int y, int width, int height);
    void drawScaledSprite(const std::uint16_t* sprite_data, int x, int y, 
                         int src_width, int src_height, int scale);
    
    // Utility functions
    bool isValidPixel(int x, int y) const;
    std::uint16_t getPixel(int x, int y) const;
};

// Bitmap font rendering
class BitmapFont {
private:
    static constexpr int CHAR_WIDTH = 8;
    static constexpr int CHAR_HEIGHT = 8;
    static constexpr int CHAR_SPACING = 2;
    
    // Font data for printable ASCII characters (32-126)
    static const std::uint8_t font_data[95][8];

public:
    static void drawChar(const GBADisplay& display, char c, int x, int y, std::uint16_t color);
    static void drawString(const GBADisplay& display, const char* str, int x, int y, std::uint16_t color);
    static void drawStringCentered(const GBADisplay& display, const char* str, int center_x, int y, std::uint16_t color);
    static int getStringWidth(const char* str);
    static constexpr int getCharWidth() { return CHAR_WIDTH; }
    static constexpr int getCharHeight() { return CHAR_HEIGHT; }
};

// Simple game state management
enum class GameState {
    TITLE_SCREEN,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// Game object base class
class GameObject {
protected:
    Vec2 position;
    Vec2 velocity;
    bool active;

public:
    GameObject(int x = 0, int y = 0) : position(x, y), velocity(0, 0), active(true) {}
    virtual ~GameObject() = default;
    
    virtual void update() = 0;
    virtual void render(const GBADisplay& display) = 0;
    
    Vec2 getPosition() const { return position; }
    void setPosition(const Vec2& pos) { position = pos; }
    Vec2 getVelocity() const { return velocity; }
    void setVelocity(const Vec2& vel) { velocity = vel; }
    
    bool isActive() const { return active; }
    void setActive(bool state) { active = state; }
    
    // Simple AABB collision detection
    virtual bool collidesWith(const GameObject& other, int width = 8, int height = 8) const;
};

// Simple particle system
class Particle : public GameObject {
private:
    std::uint16_t color;
    int lifetime;
    int max_lifetime;

public:
    Particle(int x, int y, FixedPoint vx, FixedPoint vy, std::uint16_t c, int life);
    
    void update() override;
    void render(const GBADisplay& display) override;
};

// Utility functions
namespace GBAUtils {
    // Random number generation (simple LCG)
    void seedRandom(std::uint32_t seed);
    std::uint32_t random();
    int randomRange(int min, int max);
    
    // Math functions
    FixedPoint sin(FixedPoint angle);
    FixedPoint cos(FixedPoint angle);
    FixedPoint sqrt(FixedPoint value);
    
    // Frame timing
    void waitForVBlank();
    void delay(int frames);
    
    // Memory utilities
    void fastMemSet16(volatile std::uint16_t* dest, std::uint16_t value, int count);
    void fastMemCopy16(volatile std::uint16_t* dest, const std::uint16_t* src, int count);
}

// Game framework class
class GBAGame {
protected:
    GBADisplay display;
    GameState current_state;
    int frame_counter;

public:
    GBAGame();
    virtual ~GBAGame() = default;
    
    // Main game loop
    void run();
    
    // Override these in your game
    virtual void initialize() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void handleInput() = 0;
    
    // State management
    GameState getState() const { return current_state; }
    void setState(GameState state) { current_state = state; }
    
    int getFrameCount() const { return frame_counter; }
};

#endif // GAME_H