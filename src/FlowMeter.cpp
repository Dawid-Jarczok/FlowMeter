#include "FlowMeter.h"

FlowMeter::FlowMeter(uint8_t pin) : _pin(pin) {
    pinMode(_pin, INPUT_PULLUP);
}

void FlowMeter::setPulsesPerLiter(uint32_t pulses) {
    _pulsesPerLiter = (float)pulses;
}

void FlowMeter::setupTask(UBaseType_t priority) {
    xTaskCreatePinnedToCore(
        [](void* param) {
            FlowMeter* flowMeter = static_cast<FlowMeter*>(param);
            for (;;) {
                flowMeter->_iterateTask();
                vTaskDelay(pdMS_TO_TICKS(flowMeter->_iterateInterval));
            }
        },
        "FlowMeter", FLOW_METER_TASK_STACK_SIZE, this, priority, NULL, 1);
}

void FlowMeter::iterate() {
    if (millis() - _lastTimestamp < _iterateInterval) return; 
    _lastTimestamp = millis();
    _iterateTask();
}

void FlowMeter::_iterateTask() {
    uint32_t t = millis();
    uint32_t dt = t - _lastCalcTimestamp;
    if (dt >= _minInterval4Calc) {
        uint32_t dPulses = _pulseCount - _lastPulseCount;
        // Ensure to perform calculation only if there is a significant change in volume or no changes after a certain time
        if (dPulses >= 0 || dt >= _maxInterval4Calc) {
            _lastPulseCount = _pulseCount;
            float dV = (float)dPulses / _pulsesPerLiter; // Convert to liters
            _volume += dV;

            _flowrate = dV / ((float)dt / 1000.0f); // Liters per second
        }
    }
}

#if FLOW_METER_USE_ISR
IRAM_ATTR void FlowMeter::onPulseISR() {
    if (micros() < _nextValidPulseRead) return;
    _pulseCount++;
    _nextValidPulseRead = micros() + _minPulseInterval;
}
#endif