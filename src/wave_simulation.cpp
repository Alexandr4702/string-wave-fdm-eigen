#include "string_wave/wave_simulation.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace string_wave
{
namespace
{

SimulationParameters validate(SimulationParameters parameters)
{
    if (parameters.length <= 0.0 || parameters.duration <= 0.0 || parameters.wave_speed <= 0.0 ||
        parameters.time_step <= 0.0 || parameters.space_step <= 0.0)
    {
        throw std::invalid_argument("Simulation parameters must be positive.");
    }

    if (parameters.wave_speed * parameters.time_step / parameters.space_step > 1.0)
    {
        throw std::invalid_argument("The Courant number must not exceed one.");
    }

    return parameters;
}

} // namespace

WaveSimulation::WaveSimulation(SimulationParameters parameters)
    : parameters_(validate(std::move(parameters))),
      point_count_(static_cast<std::size_t>(parameters_.length / parameters_.space_step) + 1),
      step_count_(static_cast<std::size_t>(parameters_.duration / parameters_.time_step) + 1)
{
    initialize_operator();
    initialize_state();
}

void WaveSimulation::add_observer(SimulationObserver &observer)
{
    observers_.push_back(&observer);
    observer.on_simulation_step(current_step_, current_step_ * parameters_.time_step, current_frame_);
}

bool WaveSimulation::advance()
{
    if (current_step_ + 1 >= step_count_)
    {
        return false;
    }

    if (current_step_ > 0)
    {
        next_frame_.noalias() = evolution_matrix_ * current_frame_;
        next_frame_ -= previous_frame_;

        const double next_time = (current_step_ + 1) * parameters_.time_step;
        next_frame_[0] = left_boundary(next_time);
        next_frame_[static_cast<Eigen::Index>(point_count_ - 1)] = right_boundary(next_time);
    }

    previous_frame_.swap(current_frame_);
    current_frame_.swap(next_frame_);
    ++current_step_;
    notify_observers();
    return true;
}

const SimulationParameters &WaveSimulation::parameters() const noexcept
{
    return parameters_;
}

const Eigen::VectorXd &WaveSimulation::displacement() const noexcept
{
    return current_frame_;
}

std::size_t WaveSimulation::current_step() const noexcept
{
    return current_step_;
}

std::size_t WaveSimulation::step_count() const noexcept
{
    return step_count_;
}

std::size_t WaveSimulation::point_count() const noexcept
{
    return point_count_;
}

void WaveSimulation::initialize_operator()
{
    const double courant = parameters_.wave_speed * parameters_.time_step / parameters_.space_step;
    const double side = courant * courant;
    const double center = 2.0 * (1.0 - side);

    evolution_matrix_.resize(static_cast<Eigen::Index>(point_count_), static_cast<Eigen::Index>(point_count_));
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(3 * point_count_ - 4);
    triplets.emplace_back(0, 0, 1.0);
    triplets.emplace_back(point_count_ - 1, point_count_ - 1, 1.0);

    for (std::size_t i = 1; i + 1 < point_count_; ++i)
    {
        triplets.emplace_back(i, i - 1, side);
        triplets.emplace_back(i, i, center);
        triplets.emplace_back(i, i + 1, side);
    }

    evolution_matrix_.setFromTriplets(triplets.begin(), triplets.end());
    evolution_matrix_.makeCompressed();
}

void WaveSimulation::initialize_state()
{
    current_frame_.resize(static_cast<Eigen::Index>(point_count_));
    next_frame_.resize(static_cast<Eigen::Index>(point_count_));

    for (std::size_t i = 0; i < point_count_; ++i)
    {
        const double position = i * parameters_.space_step;
        current_frame_[static_cast<Eigen::Index>(i)] = initial_displacement(position);
        next_frame_[static_cast<Eigen::Index>(i)] =
            current_frame_[static_cast<Eigen::Index>(i)] + parameters_.time_step * initial_velocity(position);
    }

    current_frame_[static_cast<Eigen::Index>(point_count_ / 2)] = parameters_.initial_amplitude;
    next_frame_[static_cast<Eigen::Index>(point_count_ / 2)] = parameters_.initial_amplitude;
}

void WaveSimulation::notify_observers()
{
    const double time = current_step_ * parameters_.time_step;
    for (SimulationObserver *observer : observers_)
    {
        observer->on_simulation_step(current_step_, time, current_frame_);
    }
}

double WaveSimulation::initial_displacement(double) const
{
    return 0.0;
}

double WaveSimulation::initial_velocity(double) const
{
    return 0.0;
}

double WaveSimulation::left_boundary(double) const
{
    return 0.0;
}

double WaveSimulation::right_boundary(double) const
{
    return 0.0;
}

} // namespace string_wave
