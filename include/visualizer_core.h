#pragma once

#include "visualizer_context.h"

namespace VisualizerCore {
void start_gui(int argc, char* argv[], VisualizerContext* ctx);
void draw_gui(VisualizerContext* ctx);
}  // namespace VisualizerCore