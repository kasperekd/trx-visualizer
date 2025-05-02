#include "visualizer_api.h"

#include <mutex>
#include <thread>

#include "visualizer_core.h"

namespace {
std::thread gui_thread;
bool running = false;
std::mutex data_mutex;

NumericMetrics current_metrics;
VisualizationData current_vis_data;
}  // namespace

extern "C" {
void visualizer_init(int argc, char* argv[]) {
    if (!running) {
        running = true;
        gui_thread = std::thread(
            [argc, argv]() { VisualizerCore::start_gui(argc, argv); });
    }
}

void visualizer_update_metrics(const NumericMetrics* metrics) {
    std::lock_guard<std::mutex> lock(data_mutex);
    current_metrics = *metrics;
}

void visualizer_update_vis_data(const VisualizationData* vis_data) {
    std::lock_guard<std::mutex> lock(data_mutex);
    current_vis_data = *vis_data;
}

void visualizer_shutdown() {
    running = false;
    if (gui_thread.joinable()) {
        gui_thread.join();
    }
}
}