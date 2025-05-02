#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "shared_metrics.h"
#include "visualizer_api.h"
// #include "visualizer_context.h"

// NumericMetrics metrics;
// VisualizationData vis_data;

void generate_test_data(NumericMetrics *metrics, VisualizationData *vis_data) {
    static float phase = 0.0f;

    // test met
    metrics->rssi = -50.0 + 10.0 * sin(phase);          // RSSI -60 to -40
    metrics->snr = 10.0 + 5.0 * cos(phase);             // SNR 5 to 15
    metrics->rxGain = 20.0 + 5.0 * sin(phase * 0.5f);   // RX Gain 15 to 25
    metrics->txPower = 10.0 + 2.0 * cos(phase * 0.2f);  // TX Power 8 to 12

    // spectrum
    vis_data->spectrum.resize(256);
    for (size_t i = 0; i < vis_data->spectrum.size(); ++i) {
        float freq = i / 256.0f;
        vis_data->spectrum[i] = 0.5f * sin(2 * M_PI * freq * 50 + phase) +
                                0.2f * ((float)rand() / RAND_MAX);  // Шум
    }

    // Constellation
    vis_data->constellation.resize(100);
    for (size_t i = 0; i < vis_data->constellation.size(); ++i) {
        float angle = 2 * M_PI * i / vis_data->constellation.size();
        float noise_x = 0.05f * ((float)rand() / RAND_MAX - 0.5f);
        float noise_y = 0.05f * ((float)rand() / RAND_MAX - 0.5f);
        vis_data->constellation[i] =
            std::polar(1.0f, angle) + std::complex<float>(noise_x, noise_y);
    }

    phase += 0.05f;
}

int main(int argc, char *argv[]) {
    visualizer_init(argc, argv);

    NumericMetrics metrics;
    VisualizationData vis_data;

    while (visualizer_is_gui_running()) {
        generate_test_data(&metrics, &vis_data);
        visualizer_update_metrics(&metrics);
        visualizer_update_vis_data(&vis_data);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(visualizer_get_update_interval()));
    }

    visualizer_shutdown();
    return 0;
}