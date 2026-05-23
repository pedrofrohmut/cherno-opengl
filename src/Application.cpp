#define GLEW_STATIC

#include <iostream>

// #INCLUDE <GL/gl.h>
#include <GL/glew.h> // Must be declared before GLFW/glfw3.h
#include <GLFW/glfw3.h>
// #include <GL/glut.h>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) { return -1; }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "opengl_app", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Glew Docs: First you need to create a valid OpenGL rendering context and call glewInit()
    if (glewInit() != GLEW_OK) {
        std::cout << "Error to init glew\n";
        return -1;
    }

    std::cout <<  "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
