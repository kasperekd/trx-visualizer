#include "visualizer_core.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <cmath>
#include <iostream>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "implot.h"

namespace VisualizerCore {
static std::mutex data_mutex;
static NumericMetrics current_metrics;
static VisualizationData current_vis_data;

void start_gui(int argc, char* argv[]) {
    // Инициализация SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Настройка OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* window =
        SDL_CreateWindow("osmoTRX Visualizer - Test Mode",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024,
                         768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    // Инициализация ImGui
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
        }

        // Новая кадровая сессия
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        draw_gui();

        // Рендеринг
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update_metrics(const NumericMetrics& metrics) {
    std::lock_guard<std::mutex> lock(data_mutex);
    current_metrics = metrics;
}

void update_vis_data(const VisualizationData& vis_data) {
    std::lock_guard<std::mutex> lock(data_mutex);
    current_vis_data = vis_data;
}

void draw_gui() {
    std::lock_guard<std::mutex> lock(data_mutex);

    // Текстовые метрики
    ImGui::Begin("Metrics");
    ImGui::Text("RSSI: %.2f dBm", current_metrics.rssi);
    ImGui::Text("SNR: %.2f dB", current_metrics.snr);
    ImGui::Text("RX Gain: %.2f dB", current_metrics.rxGain);
    ImGui::Text("TX Power: %.2f dBm", current_metrics.txPower);
    ImGui::Text("RX Drops: %lu", current_metrics.rxDropEvents);
    ImGui::Text("TX Underruns: %lu", current_metrics.txUnderruns);
    ImGui::End();

    // Графики
    if (ImPlot::BeginPlot("Spectrum")) {
        ImPlot::PlotLine("Amplitude", current_vis_data.spectrum.data(),
                         current_vis_data.spectrum.size());
        ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Constellation", ImVec2(-1, -1), ImPlotFlags_Equal)) {
        ImPlot::SetAxesLimits(-1, -1, 1, 1);
        ImPlot::PlotScatter(
            "Signal",
            [](int idx) { return current_vis_data.constellation[idx].real(); },
            [](int idx) { return current_vis_data.constellation[idx].imag(); },
            current_vis_data.constellation.size());
        ImPlot::EndPlot();
    }
}
}  // namespace VisualizerCore