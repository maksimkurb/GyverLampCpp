#include "VerticalRainbowEffect.h"

namespace  {

uint8_t hue = 0;

} // namespace

VerticalRainbowEffect::VerticalRainbowEffect()
{
}

void VerticalRainbowEffect::tick()
{
    hue += 2;
    for (uint8_t j = 0; j < height; j++) {
        const CHSV thisColor = CHSV((hue + j * scale()), 255, 255);
        for (uint8_t i = 0; i < width; i++) {
            myMatrix->drawPixelXY(i, j, thisColor);
        }
    }
}
