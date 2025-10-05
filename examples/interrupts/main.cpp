#include <Arduino.h>
#include "FlowMeter.h"

#define FLOW_METER_PIN 4

FlowMeter flowMeter(FLOW_METER_PIN);

void setup() {
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(FLOW_METER_PIN), []() { flowMeter.onPulseISR(); }, CHANGE);
    flowMeter.setupTask();
    flowMeter.setPulsesPerLiter(2275);
    flowMeter.setMinPulseInterval(200);
}

void loop() {
    Serial.printf("Q: %.2f L/min\tV: %.3f L\tP: %lu\n", flowMeter.getFlowratePerMinute(), flowMeter.getVolume(), flowMeter.getPulseCount());
    delay(100);
}