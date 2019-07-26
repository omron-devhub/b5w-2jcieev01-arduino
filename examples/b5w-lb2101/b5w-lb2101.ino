/*
 * MIT License
 * Copyright (c) 2019, 2018 - present OMRON Corporation
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* includes */
#include <Arduino.h>

/* defines */
#if defined(ARDUINO_FEATHER_ESP32)
#define PIN_B5WLB_IN      13
#define PIN_B5WLB_OUT     33
#else
#define PIN_B5WLB_IN      19
#define PIN_B5WLB_OUT     17
#endif

/** <!-- setup {{{1 -->
 * 1. initialize a Serial port for output.
 * 2. initialize GPIO pins.
 */
void setup() {
    Serial.begin(115200);  // Serial baudrate = 115200bps

    // turn off LED for safe operation.
    pinMode(PIN_B5WLB_IN, OUTPUT);
    pinMode(PIN_B5WLB_OUT, INPUT);
    digitalWrite(PIN_B5WLB_IN, LOW);
}

/** <!-- loop - Light convergent reflective type sensor {{{1 -->
 * 1. read and convert sensor.
 * 2. output results, format is: [V]
 */
void loop() {
    // get Vout(OFF): output voltage when LED is turn off.
    digitalWrite(PIN_B5WLB_IN, LOW);
    delayMicroseconds(1000);

    int32_t i_low = analogRead(PIN_B5WLB_OUT);
    float f_low = i_low * 3.3 / 1024.0;
    delayMicroseconds(200);

    // get Vout(ON): output voltage when LED is turn on.
    digitalWrite(PIN_B5WLB_IN, HIGH);
    delayMicroseconds(500);  // 400us as minimum.

    int32_t i_high = analogRead(PIN_B5WLB_OUT);
    #if defined(ARDUINO_FEATHER_ESP32)
    float f_high = i_high * 3.6 / 4096.0;   // for Feather ESP32
    #else
    float f_high = i_high * 3.3 / 1024.0;   // for Arduino MKR
    #endif
    delayMicroseconds(300);  // should not be exceed 40% ON time of period.

    // turn off to next period.
    digitalWrite(PIN_B5WLB_IN, LOW);


    // print results to serial
    // cancellation processing for external disturbing light interference.
    float f_diff = f_high - f_low;
    f_diff = f_diff < 0.0 ? 0.0: f_diff;
    Serial.print("B5W-LB:");
    Serial.print(f_diff, 2);
    Serial.println("[V]");
    delay(500);
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80
