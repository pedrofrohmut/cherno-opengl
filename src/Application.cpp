#define GLEW_STATIC

#include <iostream>
#include <cstdint> // To use fixed size integers

// #INCLUDE <GL/gl.h>
#include <GL/glew.h> // Must be declared before GLFW/glfw3.h
#include <GLFW/glfw3.h>
// #include <GL/glut.h>

static uint32_t CompileShader(uint32_t type, const std::string& source)
{
    uint32_t id =  glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling for shader
    int32_t result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // query to check if anything went wrong
    if (result == GL_FALSE) {
        int32_t length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); // get length for the message
        char* message = (char*) alloca(length * sizeof(char)); // Alloc in the stack with a dynamic length
        glGetShaderInfoLog(id, length, &length, message); // Write the log the the message buffer
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n"
                  << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    uint32_t program = glCreateProgram();
    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f,
    };

    uint32_t buffer; // Stores an uint that represents the ID tho identify this buffer later
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    // Tells OpenGL what the layout of our buffer is
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main() {\n"
        "  gl_Position = position;\n"
        "}\n";
    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main() {\n"
        "  color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";
    uint32_t shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Clean up
    glDeleteProgram(shader);
    glfwTerminate();

    return 0;
}
