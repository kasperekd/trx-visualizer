#pragma once

#include <complex>
#include <cstdint>
#include <vector>

struct NumericMetrics {
    double rssi = 0.0;
    double snr = 0.0;
    double rxGain = 0.0;
    double txPower = 0.0;
    unsigned long rxDropEvents = 0;
    unsigned long rxDropSamples = 0;
    unsigned long txUnderruns = 0;
    double timing = 0.0;
    double frequency = 0.0;
    double channelQuality = 0.0;
};

struct VisualizationData {
    std::vector<std::complex<float>> channelResponse;
    std::vector<std::complex<float>> constellation;
    std::vector<std::complex<float>> rawSignal;
    std::vector<float> spectrum;
};