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
#include <Arduino.h>

/* defines */
#define PIN_VOUT1     13
#define PIN_VOUT2     14
#define PIN_VTH       15

static int counts_idx = 0;
static int counts20sec_vout1 = 0;
static int counts20sec_vout2 = 0;


/** <!-- setup {{{1 -->
 * 1. initialize a Serial port for output.
 * 2. initialize GPIO pins.
 */
void setup() {
    Serial.begin(115200);  // Serial baudrate = 115200bps
    pinMode(PIN_VOUT1, INPUT);
    pinMode(PIN_VOUT2, INPUT);

    // setup B5W Threshould Voltage to 0.5[V]
    pinMode(PIN_VTH, OUTPUT);
    analogWriteResolution(10);
    analogWrite(PIN_VTH, 0.5 / (3.3 / 1024));
}


/** <!-- loop - dust sensor {{{1 -->
 * 1. read and convert sensor.
 * 2. output results, format is: [count]
 */
void loop() {
    // VOUT1/2 sampling in 2sec.
    uint8_t prv_gpio1 = LOW;
    uint8_t prv_gpio2 = LOW;
    int now = millis(), prev = now;
    int counts_vout1 = 0;
    int counts_vout2 = 0;
    while (now - prev < 2000) {   // Measurement interval = 2000 [msec]
        delayMicroseconds(400);   // Sampling period about 400 [usec]
        uint8_t cur_gpio1 = digitalRead(PIN_VOUT1);
        uint8_t cur_gpio2 = digitalRead(PIN_VOUT2);
        if (cur_gpio1 == HIGH && prv_gpio1 == LOW) {
            counts_vout1++;
        }
        if (cur_gpio2 == HIGH && prv_gpio2 == LOW) {
            counts_vout2++;
        }
        prv_gpio1 = cur_gpio1;
        prv_gpio2 = cur_gpio2;
        now = millis();
    }

    // output 2sec counts, just for debug.
    Serial.print("VOUT1 :");
    Serial.print(counts_vout1);
    Serial.print("[count], VOUT2 :");
    Serial.print(counts_vout2);
    Serial.print("[count], VOUT1-VOUT2 :");
    Serial.print(counts_vout1 - counts_vout2);

    // summarize and output 20sec counts
    Serial.print("COUNTS20sec_VOUT1 :");
    if (counts_idx < 10) {   // summarize, output empty data.
        Serial.println("-----[count],"
                       "COUNTS20sec_VOUT2 :-----[count],"
                       "COUNTS20sec :-----");
        counts20sec_vout1 += counts_vout1;
        counts20sec_vout2 += counts_vout2;
        counts_idx++;
    } else {                 // output 20sec counts.
        Serial.print(counts20sec_vout1);
        Serial.print("[count], COUNTS20sec_VOUT2: ");
        Serial.print(counts20sec_vout2);
        Serial.print("[count], COUNTS20sec :");
        Serial.print(counts20sec_vout1 - counts20sec_vout2);
        counts_idx = counts20sec_vout1 = counts20sec_vout2 = 0;
    }
    Serial.println("[count]");
}
// vi: ft=arduino:fdm=marker:et:sw=4:tw=80
