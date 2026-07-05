/*
 * main.cpp
 *
 *  Created on: Mar 12, 2019
 *      Author: gilg
 */

#include <cstdlib>
#include <iostream>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <GL/glut.h>

using namespace Eigen;
using namespace std;

typedef Triplet<double> TripD;

SparseMatrix<double> evolution_matrix;
VectorXd previous_frame;
VectorXd current_frame;
VectorXd next_frame;
vector<GLdouble> line;
int current_step = 0;

inline double f(double)
{
    return 0;
} // initial conditions
inline double f1(double)
{
    return 0;
} // initial conditions

inline double g(double)
{
    return 0;
} // boundary conditions
inline double g1(double)
{
    return 0;
} // boundary conditions

double T = 10;
double l = 0.5;
double c = 0.2;

double tay = 0.00025;
double h = 0.0005;

int N = l / h + 1;
int R = T / tay + 1;

void pressNormalKeys(unsigned char key, int, int)
{

    switch (key)
    {

    case 0x1b: {
        exit(0);
    }
    }
}

void display()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    glColor3ub(0, 0, 0);

    for (int i = 0; i < current_frame.size(); i++)
    {
        line[2 * i] = i * h;
        line[2 * i + 1] = current_frame[i];
    }

    glVertexPointer(2, GL_DOUBLE, 0, line.data());
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(current_frame.size()));

    glutSwapBuffers();
}

void advance_simulation(int)
{
    if (current_step >= R - 1)
    {
        return;
    }

    if (current_step > 0)
    {
        next_frame.noalias() = evolution_matrix * current_frame;
        next_frame -= previous_frame;

        const double next_time = (current_step + 1) * tay;
        next_frame[0] = g(next_time);
        next_frame[N - 1] = g1(next_time);
    }

    previous_frame.swap(current_frame);
    current_frame.swap(next_frame);
    current_step++;

    glutPostRedisplay();
    glutTimerFunc(10, advance_simulation, 0);
}

void opengl_init(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1920, 1080);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("graf");
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, l, -0.5, 0.5, -100, 100);
    glutDisplayFunc(display);
    glutTimerFunc(10, advance_simulation, 0);
    glMatrixMode(GL_MODELVIEW);

    glutKeyboardFunc(pressNormalKeys);

    glutMainLoop();
}

void solve_problem(int argc, char **argv)
{
    cout << "N= " << N << endl;
    cout << "r= " << R << endl;
    // Initial conditions
    VectorXd fi(N);
    for (int i = 0; i < N; i++)
    {
        fi[i] = f(h * i);
    }
    VectorXd f1i(N);
    for (int i = 0; i < N; i++)
    {
        f1i[i] = f1(h * i);
    }
    double a = c * c * tay * tay / h / h;
    double b = 2 * (h * h - c * c * tay * tay) / h / h;

    evolution_matrix.resize(N, N);
    vector<TripD> tripletList;
    tripletList.reserve(3 * N - 4);

    tripletList.push_back(TripD(0, 0, 1));
    tripletList.push_back(TripD(N - 1, N - 1, 1));

    for (int i = 1; i < (N - 1); i++)
    {
        tripletList.push_back(TripD(i, i - 1, a));
        tripletList.push_back(TripD(i, i, b));
        tripletList.push_back(TripD(i, i + 1, a));
    }
    evolution_matrix.setFromTriplets(tripletList.begin(), tripletList.end());
    evolution_matrix.makeCompressed();

    fi[N / 2] = 0.1;
    current_frame = fi;
    next_frame = f1i * tay + fi;
    line.resize(static_cast<size_t>(N) * 2);

    opengl_init(argc, argv);
}

int main(int argc, char **argv)
{

    solve_problem(argc, argv);

    return 0;
}
