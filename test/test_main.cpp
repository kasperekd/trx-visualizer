#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "shared_metrics.h"
#include "visualizer_api.h"

void generate_test_data(NumericMetrics& metrics, VisualizationData& vis_data) {
    static float phase = 0.0f;

    // Текстовые метрики
    metrics.rssi = -50.0 + 10.0 * sin(phase);
    metrics.snr = 10.0 + 5.0 * cos(phase);
    metrics.rxGain = 20.0 + 5.0 * sin(phase * 0.5f);
    metrics.txPower = 10.0 + 2.0 * cos(phase * 0.2f);

    // синусоида с шумом
    vis_data.spectrum.resize(256);
    for (size_t i = 0; i < vis_data.spectrum.size(); ++i) {
        float freq = i / 256.0f;
        vis_data.spectrum[i] = 0.5f * sin(2 * M_PI * freq * 50 + phase) +
                               0.2f * ((float)rand() / RAND_MAX);  // Шум
    }

    // (QPSK с шумом)
    vis_data.constellation.resize(100);
    for (size_t i = 0; i < vis_data.constellation.size(); ++i) {
        float angle = 2 * M_PI * i / vis_data.constellation.size();
        float noise_x = 0.05f * ((float)rand() / RAND_MAX - 0.5f);
        float noise_y = 0.05f * ((float)rand() / RAND_MAX - 0.5f);
        vis_data.constellation[i] =
            std::polar(1.0f, angle) + std::complex<float>(noise_x, noise_y);
    }

    phase += 0.05f;
}

int main(int argc, char* argv[]) {
    // Инициализация визуализатора
    visualizer_init(argc, argv);

    NumericMetrics metrics;
    VisualizationData vis_data;

    // Цикл обновления данных
    while (true) {
        generate_test_data(metrics, vis_data);
        visualizer_update_metrics(&metrics);
        visualizer_update_vis_data(&vis_data);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    visualizer_shutdown();
    return 0;
}