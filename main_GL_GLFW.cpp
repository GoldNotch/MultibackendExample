#include <cstdio>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game/Game.h"

// Shaders
const char* vertexShaderSource = "#version 330 core\n"
                                "layout (location = 0) in vec3 position;\n"
                                "layout (location = 1) in vec3 color;\n"
                                "out vec4 vertexColor;\n"
                                "void main()\n"
                                "{\n"
                                "gl_Position = vec4(position, 1.0);\n"
                                "vertexColor = vec4(color, 1.0);\n"
                                "}\0";
const char* fragmentShaderSource = "#version 330 core\n"
                                    "in vec4 vertexColor;\n"
                                    "void main()\n"
                                    "{\n"
                                    "gl_FragColor = vertexColor;\n"
                                    "}\n\0";

bool PressedKeys[GLFW_KEY_LAST];

void OnKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(action == GLFW_PRESS)
    	PressedKeys[key] = true;
    if (action == GLFW_RELEASE)
        PressedKeys[key] = false;
}

int main()
{
    //------------------ setting up a window ----------------------
    //Инициализация GLFW
	glfwInit();
	//Настройка GLFW
	//Задается минимальная требуемая версия OpenGL.
	//Мажорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Установка профайла для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Выключение возможности изменения размера окна
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //open window
	GLFWwindow* window = glfwCreateWindow(1024, 768, "GLFW", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, OnKeyCallback);
    // ------------------ init GraphicAPI ---------------------------------
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    glViewport(0, 0, 1024, 768);

    // ------------- Build and compile our shader program -----------------
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --------------------- Create buffers -----------------------
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // -------------------- Create map of key codes ------------------

    //map<SFML_KeyCode, Game::KeyCode> to define which key was pressed.
    //it needs because key codes in library could be not coincide
    Game::KeyCode KeyMap[GLFW_KEY_LAST];
    //init map
    KeyMap[GLFW_KEY_ESCAPE] = Game::KEY_ESCAPE;
    //...

    // -------------------- create Game -------------------------
    IGame<Game::InputData, Game::OutputData>* game = new Game::Game();
    Game::InputData input;
    input.ProcessTimeInSec = 0.0f;
    for(int i = 0; i < Game::KEYS_TOTAL; ++i)
        input.PressedKeys[i] = false;
    input.CursorPos_x = 0.0f;
    input.CursorPos_y = 0.0f;

    // --------------------- main loop -----------------------------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // fill input data
        input.ProcessTimeInSec = glfwGetTime();

        //tick game
        Game::OutputData output = game->Tick(input);
        //handle commands
        if (output.Commands.size() > 0)
            for(auto command : output.Commands)
                if (command == Game::COMMAND_CLOSE_GAME)
                    glfwSetWindowShouldClose(window, true);

        //draw triangle
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.15f, 0.1f, 0.3f, 1.0f);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, output.VertexBuffer.size() * sizeof(float), output.VertexBuffer.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, output.VerticesCount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    //free game resources
    delete game;
    glfwTerminate();

    return 0;
}
