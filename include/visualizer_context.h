#pragma once

#include <atomic>
#include <mutex>
#include <vector>

#include "shared_metrics.h"

#define MAX_HISTORY_SIZE 100

struct VisualizerContext {
    std::mutex data_mutex;

    std::atomic<bool> gui_running = true;
    std::atomic<int> update_interval_ms = 50;
    std::atomic<bool> is_main_window_open = true;
    std::atomic<bool> is_spectrum_window_open = true;
    std::atomic<bool> is_constellation_window_open = true;
    std::atomic<bool> is_history_window_open = true;

    NumericMetrics metrics;
    VisualizationData vis_data;

    std::vector<double> rssi_history;
    std::vector<double> snr_history;

    bool show_rssi_history = true;
    bool show_snr_history = true;

    VisualizerContext()
        : gui_running(true),
          update_interval_ms(50),
          is_main_window_open(true),
          is_spectrum_window_open(true),
          is_constellation_window_open(true),
          is_history_window_open(true) {}
};

extern "C" {
VisualizerContext* create_visualizer_context();

void destroy_visualizer_context(VisualizerContext* ctx);
}