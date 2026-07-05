#pragma once

#include <cstddef>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "string_wave/simulation_observer.hpp"

namespace string_wave
{

struct SimulationParameters
{
    double duration = 10.0;
    double length = 0.5;
    double wave_speed = 0.2;
    double time_step = 0.00025;
    double space_step = 0.0005;
    double initial_amplitude = 0.1;
};

class WaveSimulation
{
  public:
    explicit WaveSimulation(SimulationParameters parameters = {});

    void add_observer(SimulationObserver &observer);
    bool advance();

    [[nodiscard]] const SimulationParameters &parameters() const noexcept;
    [[nodiscard]] const Eigen::VectorXd &displacement() const noexcept;
    [[nodiscard]] std::size_t current_step() const noexcept;
    [[nodiscard]] std::size_t step_count() const noexcept;
    [[nodiscard]] std::size_t point_count() const noexcept;

  private:
    void initialize_operator();
    void initialize_state();
    void notify_observers();

    [[nodiscard]] double initial_displacement(double position) const;
    [[nodiscard]] double initial_velocity(double position) const;
    [[nodiscard]] double left_boundary(double time) const;
    [[nodiscard]] double right_boundary(double time) const;

    SimulationParameters parameters_;
    std::size_t point_count_;
    std::size_t step_count_;
    std::size_t current_step_ = 0;

    Eigen::SparseMatrix<double> evolution_matrix_;
    Eigen::VectorXd previous_frame_;
    Eigen::VectorXd current_frame_;
    Eigen::VectorXd next_frame_;
    std::vector<SimulationObserver *> observers_;
};

} // namespace string_wave
