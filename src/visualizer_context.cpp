#include "visualizer_context.h"

VisualizerContext* create_visualizer_context() {
    return new VisualizerContext();
}

void destroy_visualizer_context(VisualizerContext* ctx) { delete ctx; }