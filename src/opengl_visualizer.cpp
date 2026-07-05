#include "string_wave/opengl_visualizer.hpp"

#include <cstdlib>
#include <stdexcept>

#include <GL/glut.h>

namespace string_wave
{

OpenGLVisualizer *OpenGLVisualizer::active_instance_ = nullptr;

void OpenGLVisualizer::run(int argc, char **argv, WaveSimulation &simulation)
{
    if (active_instance_ != nullptr)
    {
        throw std::logic_error("Only one OpenGL visualizer can be active.");
    }

    active_instance_ = this;
    simulation_ = &simulation;
    line_.resize(simulation.point_count() * 2);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1920, 1080);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("String wave");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, simulation.parameters().length, -0.5, 0.5, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display_callback);
    glutKeyboardFunc(keyboard_callback);
    glutTimerFunc(10, timer_callback, 0);

    simulation.add_observer(*this);
    glutMainLoop();
}

void OpenGLVisualizer::on_simulation_step(std::size_t, double, const Eigen::VectorXd &displacement)
{
    const double space_step = simulation_->parameters().space_step;
    for (Eigen::Index i = 0; i < displacement.size(); ++i)
    {
        line_[static_cast<std::size_t>(2 * i)] = i * space_step;
        line_[static_cast<std::size_t>(2 * i + 1)] = displacement[i];
    }

    glutPostRedisplay();
}

void OpenGLVisualizer::display_callback()
{
    active_instance_->display();
}

void OpenGLVisualizer::keyboard_callback(unsigned char key, int, int)
{
    if (key == 0x1b)
    {
        std::exit(0);
    }
}

void OpenGLVisualizer::timer_callback(int)
{
    active_instance_->advance();
}

void OpenGLVisualizer::display()
{
    glClearColor(1.0F, 1.0F, 1.0F, 0.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glColor3ub(0, 0, 0);
    glVertexPointer(2, GL_DOUBLE, 0, line_.data());
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(line_.size() / 2));
    glutSwapBuffers();
}

void OpenGLVisualizer::advance()
{
    if (simulation_->advance())
    {
        glutTimerFunc(10, timer_callback, 0);
    }
}

} // namespace string_wave
