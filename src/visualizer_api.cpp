#include "visualizer_api.h"

#include <mutex>
#include <thread>

#include "visualizer_context.h"
#include "visualizer_core.h"

namespace {
VisualizerContext* g_context = nullptr;
std::thread g_gui_thread;
}  // namespace

extern "C" {
void visualizer_init(int argc, char* argv[]) {
    if (!g_context) {
        g_context = create_visualizer_context();
        g_gui_thread = std::thread([argc, argv]() {
            VisualizerCore::start_gui(argc, argv, g_context);
        });
    }
}

void visualizer_update_metrics(const NumericMetrics* metrics) {
    if (!g_context) return;
    std::lock_guard<std::mutex> lock(g_context->data_mutex);
    g_context->metrics = *metrics;

    // update history
    g_context->rssi_history.push_back(metrics->rssi);
    g_context->snr_history.push_back(metrics->snr);

    if (g_context->rssi_history.size() > 100)
        g_context->rssi_history.erase(g_context->rssi_history.begin());
    if (g_context->snr_history.size() > 100)
        g_context->snr_history.erase(g_context->snr_history.begin());
}

void visualizer_update_vis_data(const VisualizationData* vis_data) {
    if (!g_context) return;
    std::lock_guard<std::mutex> lock(g_context->data_mutex);
    g_context->vis_data = *vis_data;
}

int visualizer_get_update_interval() {
    if (!g_context) return 50;
    return g_context->update_interval_ms.load();
}

bool visualizer_is_gui_running() {
    if (!g_context) return false;
    return g_context->gui_running.load();
}

void visualizer_toggle_spectrum_window(bool enable) {
    if (g_context) g_context->is_spectrum_window_open.store(enable);
}

void visualizer_toggle_constellation_window(bool enable) {
    if (g_context) g_context->is_constellation_window_open.store(enable);
}

void visualizer_toggle_history_window(bool enable) {
    if (g_context) g_context->is_history_window_open.store(enable);
}

void visualizer_shutdown() {
    if (g_context) {
        g_context->gui_running.store(false);
        if (g_gui_thread.joinable()) {
            g_gui_thread.join();
        }
        destroy_visualizer_context(g_context);
        g_context = nullptr;
    }
}
}