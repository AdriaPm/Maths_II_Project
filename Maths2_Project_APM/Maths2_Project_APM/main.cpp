/**********************************************************/
/*					ADRIA PONS MENSA					  */
/**********************************************************/

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <initializer_list>
#include <cmath>
#include <optional>
#include <time.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <GLFW/glfw3.h>

using namespace std;
using namespace Eigen;

#define M_PI 3.14159265358979323846
#define DEGS_TO_RADS 0.01745329252
#define RADS_TO_DEGS 57.295779513


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Maths 2 Project", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

