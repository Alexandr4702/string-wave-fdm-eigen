#include <cmath>
#include <exception>
#include <iostream>

#include "string_wave/opengl_visualizer.hpp"
#include "string_wave/wave_simulation.hpp"

namespace
{

string_wave::WaveProblem create_problem()
{
    const string_wave::SimulationParameters parameters{
        10.0, 0.5, 0.2, 0.00025, 0.0005,
    };

    return {
        parameters,
        [parameters](double position) {
            const double center = parameters.length / 2.0;
            return std::abs(position - center) < parameters.space_step / 2.0 ? 0.1 : 0.0;
        },
        [](double) { return 0.0; },
        [](double) { return 0.0; },
        [](double) { return 0.0; },
    };
}

} // namespace

int main(int argc, char **argv)
{
    try
    {
        string_wave::WaveSimulation simulation(create_problem());
        string_wave::OpenGLVisualizer visualizer;
        visualizer.run(argc, argv, simulation);
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
