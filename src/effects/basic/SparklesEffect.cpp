#include "SparklesEffect.h"

namespace {

} // namespace

SparklesEffect::SparklesEffect()
{
}

void SparklesEffect::tick()
{
    for (uint8_t i = 0; i < scale(); i++) {
        uint8_t x = random(0, width);
        uint8_t y = random(0, height);
        if (!myMatrix->getPixColorXY(x, y)) {
            myMatrix->setLed(x, y, CHSV(random(0, 255), 255, 255));
        }
    }
    fader(70);
}

void SparklesEffect::fader(uint8_t step) {
    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++) {
            myMatrix->fadePixelXY(x, y, step);
        }
    }
}
