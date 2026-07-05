#include <exception>
#include <iostream>

#include "string_wave/opengl_visualizer.hpp"
#include "string_wave/wave_simulation.hpp"

int main(int argc, char **argv)
{
    try
    {
        string_wave::WaveSimulation simulation;
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
