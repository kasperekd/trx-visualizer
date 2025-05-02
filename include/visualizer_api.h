#pragma once

#include "shared_metrics.h"

extern "C" {
void visualizer_init(int argc, char* argv[]);
void visualizer_shutdown();

// update
void visualizer_update_metrics(const NumericMetrics* metrics);
void visualizer_update_vis_data(const VisualizationData* vis_data);

// control
int visualizer_get_update_interval();
bool visualizer_is_gui_running();

void visualizer_toggle_spectrum_window(bool enable);
void visualizer_toggle_constellation_window(bool enable);
void visualizer_toggle_history_window(bool enable);
}