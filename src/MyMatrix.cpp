#include "MyMatrix.h"
#include "Settings.h"

namespace  {

uint8_t defaultWidth = 16;
uint8_t defaultHeight = 16;
uint8_t defaultType = NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
                      NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG;

uint8_t defaultMaxBrightness = 80;
uint32_t defaultCurrentLimit = 1000;

uint8_t defaultRoatation = 3;

#if defined(SONOFF)
const uint8_t ledPin = 14;
#elif defined(ESP32)
const uint8_t ledPin = 13;
#else
const uint8_t ledPin = D6;
#endif

uint16_t numLeds = 0;

CRGB* leds = nullptr;

MyMatrix *instance = nullptr;

} // namespace

MyMatrix *MyMatrix::Instance()
{
    return instance;
}

void MyMatrix::Initialize()
{
    if (instance) {
        return;
    }

    uint8_t sizeWidth = mySettings->GetByteField(F("matrix"), F("width"), defaultWidth);
    uint8_t sizeHeight = mySettings->GetByteField(F("matrix"), F("height"), defaultHeight);
    uint8_t matrixType = mySettings->GetByteField(F("matrix"), F("type"), defaultType);

    numLeds = sizeWidth * sizeHeight;
    leds = new CRGB[numLeds]();
    FastLED.addLeds<WS2812B, ledPin, GRB>(leds, numLeds);

    uint8_t maxBrightness = mySettings->GetByteField(F("matrix"), F("maxBrightness"), defaultMaxBrightness);
    Serial.printf_P(PSTR("Set max brightness to: %u\n"), maxBrightness);
    FastLED.setBrightness(maxBrightness);

    uint32_t currentLimit = mySettings->GetULongLongField(F("matrix"), F("currentLimit"), defaultCurrentLimit);
    Serial.printf_P(PSTR("Set current limit to: %u\n"), currentLimit);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, currentLimit);

    instance = new MyMatrix(leds, sizeWidth, sizeHeight, matrixType);
    uint8_t rotation = GetRotation();
    Serial.printf_P(PSTR("Set rotation to: %u\n"), rotation);
    instance->setRotation(rotation);

    instance->begin();
    instance->clear();
    instance->show();
}

uint8_t MyMatrix::GetRotation()
{
    uint8_t rotation = mySettings->GetByteField(F("matrix"), F("rotation"), defaultRoatation);
    return rotation;
}

void MyMatrix::setCurrentLimit(uint32_t maxCurrent)
{
    FastLED.setMaxPowerInVoltsAndMilliamps(5, maxCurrent);
}

MyMatrix::MyMatrix(CRGB *leds, uint8_t w, uint8_t h, uint8_t matrixType)
    : FastLED_NeoMatrix(leds, w, h, 1, 1, matrixType)
{
}

void MyMatrix::fill(CRGB color, bool shouldShow)
{
    fill_solid(leds, numLeds, color);
    if (shouldShow) {
        show();
    }
}

void MyMatrix::fillProgress(double progress)
{
    clear();

    const uint16_t number = static_cast<uint16_t>(numLeds * progress);
    const String percent = String(static_cast<int>(progress * 100));

    const uint8_t fullRows = static_cast<uint8_t>(number / width());
    const uint8_t remainingProgress = static_cast<uint8_t>(number % width());

    if (fullRows > 0) {
        fillRectXY(0, 0, width(), fullRows, CRGB(10, 10, 10));
        delay(1);
    }

    if (remainingProgress > 0) {
        drawLineXY(0, fullRows, remainingProgress, fullRows, CRGB(10, 10, 10));
        delay(1);
    }

    setCursor(0, 0);
    print(percent);
    delay(1);

    show();
}

void MyMatrix::setLed(uint8_t x, uint8_t y, CRGB color, bool shouldShow)
{
    drawPixelXY(x, y, color);
    if (shouldShow) {
        show();
    }
}

void MyMatrix::setLed(uint16_t index, CRGB color, bool shouldShow)
{
    leds[index] = color;
    if (shouldShow) {
        show();
    }
}

void MyMatrix::fadeToBlackBy(uint16_t index, uint8_t step)
{
    leds[index].fadeToBlackBy(step);
}

void MyMatrix::matrixTest()
{
    for (int16_t i = 0; i < 16; i++) {
        clear();
        drawPixelXY(i, 0, CRGB(CRGB::Red));
        drawPixelXY(0, i, CRGB(CRGB::Green));
        drawPixelXY(i, i, CRGB(CRGB::White));
        show();
        delay(100);
    }

    clear();
    show();
}

void MyMatrix::clear(bool shouldShow)
{
    FastLED.clear();
    if (shouldShow) {
        delay(1);
        show();
    }
}

uint16_t MyMatrix::getPixelNumber(uint8_t x, uint8_t y)
{
    return static_cast<uint16_t>(XY(y, x));
}

void MyMatrix::drawPixelXY(uint8_t x, uint8_t y, CRGB color)
{
    setPassThruColor(color);
    drawPixel(y, x, color);
    setPassThruColor();
}

void MyMatrix::drawLineXY(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, CRGB color)
{
    setPassThruColor(color);
    drawLine(y0, x0, y1, x1, 0);
    setPassThruColor();
}

CRGB MyMatrix::getPixColor(uint16_t number)
{
    if (number > numLeds - 1) {
        return 0;
    }
    return leds[number];
}

CRGB MyMatrix::getPixColorXY(uint8_t x, uint8_t y)
{
    return getPixColor(getPixelNumber(x, y));
}

void MyMatrix::fillRectXY(uint8_t x, uint8_t y, uint8_t w, uint8_t h, CRGB color)
{
    setPassThruColor(color);
    fillRect(y, x, h, w, 0);
    setPassThruColor();
}

void MyMatrix::fadePixelXY(uint8_t x, uint8_t y, uint8_t step)
{
    const uint16_t pixelNum = myMatrix->getPixelNumber(x, y);
    const CRGB color = myMatrix->getPixColor(pixelNum);

    if (!color) {
        return;
    }

    if (color.r >= 30 || color.g >= 30 || color.b >= 30) {
        myMatrix->fadeToBlackBy(pixelNum, step);
    } else {
        myMatrix->setLed(pixelNum, CRGB::Black);
    }
}
