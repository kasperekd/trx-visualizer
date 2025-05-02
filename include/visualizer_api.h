#pragma once

#include "shared_metrics.h"

extern "C" {
void visualizer_init(int argc, char* argv[]);

void visualizer_update_metrics(const NumericMetrics* metrics);
void visualizer_update_vis_data(const VisualizationData* vis_data);

void visualizer_shutdown();
}