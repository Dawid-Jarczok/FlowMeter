#pragma once

#include <Arduino.h>

#define FLOW_METER_TASK_STACK_SIZE 1024
#define FLOW_METER_USE_ISR true

class FlowMeter {
public:
    FlowMeter(uint8_t pin);

    void setPulsesPerLiter(uint32_t pulses);
    uint32_t getPulsesPerLiter() const { return (uint32_t)_pulsesPerLiter; }

    void setIterateInterval(uint32_t interval) { _iterateInterval = interval; }
    void setMinPulseInterval(uint32_t interval) { _minPulseInterval = interval; }

    float getFlowrate() const { return _flowrate; }
    float getFlowratePerMinute() const { return _flowrate * 60.0f; }
    void resetVolume() { _volume = 0.0f; }
    void setVolume(float volume) { _volume = volume; }
    float getVolume() const { return _volume; }
    uint32_t getPulseCount() const { return _pulseCount; }

    void setupTask(UBaseType_t priority = 1);
    void iterate();

    #if FLOW_METER_USE_ISR
    IRAM_ATTR void onPulseISR();
    #endif
private:
    void _iterateTask();

    uint8_t _pin;
    volatile uint32_t _pulseCount;
    uint32_t _lastPulseCount;
    volatile uint32_t _nextValidPulseRead = 0;
    uint32_t _minPulseInterval = 1000; // Minimum pulse interval in microseconds to filter out noise

    float _pulsesPerLiter = 1.0f;
    float _volume; // Litres
    float _flowrate; // Litres per second

    uint32_t _lastTimestamp;
    uint32_t _iterateInterval = 10;
    uint32_t _lastCalcTimestamp = 0;
    uint32_t _minInterval4Calc = 50;
    uint32_t _maxInterval4Calc = 250;

    TaskHandle_t _taskHandle;
};