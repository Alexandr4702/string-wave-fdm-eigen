#pragma once

#include <cstddef>
#include <functional>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "string_wave/simulation_observer.hpp"

namespace string_wave
{

struct SimulationParameters
{
    double duration;
    double length;
    double wave_speed;
    double time_step;
    double space_step;
};

struct WaveProblem
{
    SimulationParameters parameters;
    std::function<double(double)> initial_displacement;
    std::function<double(double)> initial_velocity;
    std::function<double(double)> left_boundary;
    std::function<double(double)> right_boundary;
};

class WaveSimulation
{
  public:
    explicit WaveSimulation(WaveProblem problem);

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

    WaveProblem problem_;
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
