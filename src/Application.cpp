#define GLEW_STATIC

#include <iostream>
#include <cstdint> // To use fixed size integers
#include <fstream> // To read shader files
#include <string>
#include <sstream>
#include <cassert> // Assert shader type

// #INCLUDE <GL/gl.h>
#include <GL/glew.h> // Must be declared before GLFW/glfw3.h
#include <GLFW/glfw3.h>
// #include <GL/glut.h>

#define CWD_ (std::filesystem::current_path()).string()

// Using a macro here make it a lot easier.
// With a function you would need to pass file and line to it
#ifdef WIN32
  #define ASSERT_(x) if(!(x)) __debugBreak();
#else
  #include <signal.h>
  #define ASSERT_(x) if(!(x)) raise(SIGTRAP);
#endif

#define GlCall_(x) ClearAllGlErrors();\
    x;\
    ASSERT_(LogGlCall(#x, __FILE__, __LINE__))

static void ClearAllGlErrors()
{
    while (glGetError() != GL_NO_ERROR)
    {
    }
}

static bool LogGlCall(const char *function, const char *file, int line)
{
    bool success = true;
    while (GLenum error = glGetError())
    {
        success = false;
        std::string errorStr;
        switch (error)
        {
        case GL_INVALID_ENUM:
            errorStr = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE:
            errorStr = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:
            errorStr = "GL_INVALID_OPERATION"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        case GL_OUT_OF_MEMORY:
            errorStr = "GL_OUT_OF_MEMORY"; break;
        case GL_STACK_UNDERFLOW:
            errorStr = "GL_STACK_UNDERFLOW"; break;
        case GL_STACK_OVERFLOW:
            errorStr = "GL_STACK_OVERFLOW"; break;
        }
        std::cout << "[OpenGL Error] (" << errorStr << "): " << function << " " << file << ":" << line << "\n";
    }
    return success;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    if (!stream.is_open()) {
        std::cerr << "ERROR: Failed to open shader file: " << filepath << '\n';
        return ShaderProgramSource{};
    }

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::string line;
    std::stringstream ss[2]; // 1 for the vertex shader and 1 for the fragment shader
    ShaderType shaderType = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        { // Read the type
            if (line.find("vertex") != std::string::npos)
                shaderType = ShaderType::VERTEX;    // Set Mode to vertex
            else if (line.find("fragment") != std::string::npos)
                shaderType = ShaderType::FRAGMENT;  // Set Mode to fragment
        }
        else
        { // Read content
            if (shaderType != ShaderType::NONE)
                ss[(uint8_t)shaderType] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

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

    float positions[] = {
        //  x,     y,
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    uint32_t indices[] = {  // This is an index buffer
        0, 1, 2, // First triangle
        0, 2, 3, // Second triangle
    };

    uint32_t buffer; // Stores an uint that represents the ID tho identify this buffer later
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);

    // Tells OpenGL what the layout of our buffer is
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    uint32_t ibo; // Index buffer object
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    //std::cout << "Vertex: \n" << source.VertexSource << "\n\nFragment:\n" << source.FragmentSource << std::endl;
    uint32_t shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // ClearAllGlErrors();
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // ASSERT_(LogGlCall());

        GlCall_(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));

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
