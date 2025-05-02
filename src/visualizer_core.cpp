#include "visualizer_core.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <cmath>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "implot.h"

namespace VisualizerCore {

void start_gui(int argc, char* argv[], VisualizerContext* ctx) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "osmoTRX Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1024, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    glewInit();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool running = true;
    auto last_frame_time = std::chrono::steady_clock::now();

    while (ctx->gui_running.load()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                ctx->gui_running.store(false);
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        draw_gui(ctx);

        if (!ctx->is_main_window_open.load() &&
            !ctx->is_spectrum_window_open.load() &&
            !ctx->is_constellation_window_open.load() &&
            !ctx->is_history_window_open.load()) {
            ctx->gui_running.store(false);
        }

        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_gui(VisualizerContext* ctx) {
    bool is_main_open = ctx->is_main_window_open.load();
    bool is_spectrum_open = ctx->is_spectrum_window_open.load();
    bool is_constellation_open = ctx->is_constellation_window_open.load();
    bool is_history_open = ctx->is_history_window_open.load();

    int interval = ctx->update_interval_ms.load();
    bool show_rssi = ctx->show_rssi_history;
    bool show_snr = ctx->show_snr_history;

    // Settings
    if (is_main_open) {
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Settings", &is_main_open)) {
            ImGui::SliderInt("Update Interval (ms)", &interval, 1, 1000);
            ImGui::Checkbox("Show RSSI History", &show_rssi);
            ImGui::Checkbox("Show SNR History", &show_snr);
            ImGui::Separator();
            ImGui::Text("Window Controls");
            ImGui::SameLine();
            ImGui::Checkbox("Spectrum", &is_spectrum_open);
            ImGui::SameLine();
            ImGui::Checkbox("Constellation", &is_constellation_open);
            ImGui::SameLine();
            ImGui::Checkbox("History", &is_history_open);
            ImGui::End();
        }
    }

    // Metrics
    if (is_main_open) {
        ImGui::SetNextWindowPos(ImVec2(100, 350), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Metrics", &is_main_open)) {
            ImGui::Text("RSSI: %.2f dBm", ctx->metrics.rssi);
            ImGui::Text("SNR: %.2f dB", ctx->metrics.snr);
            ImGui::Text("RX Gain: %.2f dB", ctx->metrics.rxGain);
            ImGui::Text("TX Power: %.2f dBm", ctx->metrics.txPower);
            ImGui::Text("RX Drops: %lu", ctx->metrics.rxDropEvents);
            ImGui::Text("TX Underruns: %lu", ctx->metrics.txUnderruns);
            ImGui::End();
        }
    }

    // spectrum
    if (is_spectrum_open) {
        ImGui::SetNextWindowPos(ImVec2(450, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Spectrum", &is_spectrum_open)) {
            if (!ctx->vis_data.spectrum.empty()) {
                ImPlot::BeginPlot("##SpectrumPlot", ImVec2(-1, -1));
                ImPlot::PlotLine("Amplitude", ctx->vis_data.spectrum.data(),
                                 ctx->vis_data.spectrum.size());
                ImPlot::EndPlot();
            }
            ImGui::End();
        }
    }

    // constellation
    if (is_constellation_open) {
        ImGui::SetNextWindowPos(ImVec2(450, 450), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Constellation", &is_constellation_open)) {
            if (!ctx->vis_data.constellation.empty()) {
                ImPlot::BeginPlot("##ConstellationPlot", ImVec2(-1, -1),
                                  ImPlotFlags_Equal);
                ImPlot::PlotScatterG(
                    "Signal",
                    [](int idx, void* data) {
                        auto& vec =
                            *static_cast<std::vector<std::complex<float>>*>(
                                data);
                        return ImPlotPoint(vec[idx].real(), vec[idx].imag());
                    },
                    &ctx->vis_data.constellation,
                    ctx->vis_data.constellation.size());
                ImPlot::EndPlot();
            }
            ImGui::End();
        }
    }

    // history
    if (is_history_open) {
        ImGui::SetNextWindowPos(ImVec2(900, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("History", &is_history_open)) {
            ImPlot::BeginPlot("##HistoryPlot", ImVec2(-1, -1));
            if (show_rssi && !ctx->rssi_history.empty()) {
                ImPlot::PlotLine("RSSI", ctx->rssi_history.data(),
                                 ctx->rssi_history.size());
            }
            if (show_snr && !ctx->snr_history.empty()) {
                ImPlot::PlotLine("SNR", ctx->snr_history.data(),
                                 ctx->snr_history.size());
            }
            ImPlot::EndPlot();
            ImGui::End();
        }
    }

    ctx->is_main_window_open.store(is_main_open);
    ctx->is_spectrum_window_open.store(is_spectrum_open);
    ctx->is_constellation_window_open.store(is_constellation_open);
    ctx->is_history_window_open.store(is_history_open);
    ctx->update_interval_ms.store(interval);
    ctx->show_rssi_history = show_rssi;
    ctx->show_snr_history = show_snr;
}
}  // namespace VisualizerCore