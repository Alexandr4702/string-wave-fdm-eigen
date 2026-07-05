#pragma once

#include <cstddef>

#include <Eigen/Core>

namespace string_wave
{

class SimulationObserver
{
  public:
    virtual ~SimulationObserver() = default;

    virtual void on_simulation_step(std::size_t step, double time, const Eigen::VectorXd &displacement) = 0;
};

} // namespace string_wave
