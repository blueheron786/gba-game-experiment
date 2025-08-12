#include <gba.h>
#include <gba_video.h>

int main() {
    // Set video mode 3 (bitmap) and background 2
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    // Draw a white rectangle as a placeholder for "Hello World"
    u16* fb = (u16*)VRAM;
    int x = 80, y = 70; // Center-ish
    int w = 80, h = 16; // Rectangle size
    u16 color = RGB5(31,31,31);
    for (int dy = 0; dy < h; ++dy) {
        for (int dx = 0; dx < w; ++dx) {
            fb[(y + dy) * 240 + (x + dx)] = color;
        }
    }

    while (1) {
        VBlankIntrWait();
    }
    return 0;
}