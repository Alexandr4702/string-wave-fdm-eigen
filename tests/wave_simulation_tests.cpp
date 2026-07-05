#include <cmath>
#include <cstdlib>
#include <iostream>

#include "string_wave/wave_simulation.hpp"

namespace
{

class RecordingObserver final : public string_wave::SimulationObserver
{
  public:
    void on_simulation_step(std::size_t step, double time, const Eigen::VectorXd &displacement) override
    {
        ++notification_count;
        last_step = step;
        last_time = time;

        if (displacement[0] != 0.0 || displacement[displacement.size() - 1] != 0.0)
        {
            fixed_boundaries = false;
        }
    }

    std::size_t notification_count = 0;
    std::size_t last_step = 0;
    double last_time = 0.0;
    bool fixed_boundaries = true;
};

} // namespace

int main()
{
    string_wave::SimulationParameters parameters;
    parameters.duration = 0.2;
    parameters.length = 1.0;
    parameters.wave_speed = 1.0;
    parameters.time_step = 0.05;
    parameters.space_step = 0.1;

    string_wave::WaveSimulation simulation(parameters);
    RecordingObserver observer;
    simulation.add_observer(observer);

    while (simulation.advance())
    {
    }

    const bool passed = observer.notification_count == simulation.step_count() &&
                        observer.last_step == simulation.step_count() - 1 &&
                        std::abs(observer.last_time - parameters.duration) < 1e-12 && observer.fixed_boundaries;

    if (!passed)
    {
        std::cerr << "Observer integration test failed.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
