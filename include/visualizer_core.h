#pragma once

#include <mutex>
#include <thread>

#include "shared_metrics.h"

namespace VisualizerCore {
void start_gui(int argc, char* argv[]);

void update_metrics(const NumericMetrics& metrics);
void update_vis_data(const VisualizationData& vis_data);

void draw_gui();
}  // namespace VisualizerCore