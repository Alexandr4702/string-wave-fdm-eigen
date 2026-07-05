#pragma once

#include <vector>

#include "string_wave/simulation_observer.hpp"
#include "string_wave/wave_simulation.hpp"

namespace string_wave
{

class OpenGLVisualizer final : public SimulationObserver
{
  public:
    void run(int argc, char **argv, WaveSimulation &simulation);
    void on_simulation_step(std::size_t step, double time, const Eigen::VectorXd &displacement) override;

  private:
    static void display_callback();
    static void keyboard_callback(unsigned char key, int x, int y);
    static void timer_callback(int value);

    void display();
    void advance();

    static OpenGLVisualizer *active_instance_;
    WaveSimulation *simulation_ = nullptr;
    std::vector<double> line_;
};

} // namespace string_wave
