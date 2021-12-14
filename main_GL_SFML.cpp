#include <cstdio>
#include <iostream>
#include <SFML/window.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "Game.h"

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


int main()
{
    //------------------ setting up a window ----------------------
    sfVideoMode mode = {1024, 768, 32};
    sfWindowStyle style = sfDefaultStyle;
    sfContextSettings settings;
    settings.antialiasingLevel = 0;
    settings.attributeFlags = 0;
    settings.depthBits = 24;
    settings.sRgbCapable = sfTrue;
    settings.stencilBits = 0;
    settings.minorVersion = 0;
    settings.majorVersion = 3;
    //create window
    sfWindow* wnd = sfWindow_create(mode, "Game", style, &settings);
    sfWindow_setFramerateLimit(wnd, 0);
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
    Game::KeyCode KeyMap[sfKeyCount];
    //init map
    KeyMap[sfKeyEscape] = Game::KEY_ESCAPE;
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
    sfClock *clock = sfClock_create();
    while (sfWindow_isOpen(wnd))
    {
        // fill input data
        sfTime process_time = sfClock_getElapsedTime(clock);
        input.ProcessTimeInSec = sfTime_asSeconds(process_time);
        //event handling
        sfEvent event;
        if (sfWindow_pollEvent(wnd, &event))
        {
            if (event.type == sfEvtClosed)
                sfWindow_close(wnd);
            if (event.type == sfEvtKeyPressed)
            {
                auto PressedKey = KeyMap[event.key.code];
                input.PressedKeys[PressedKey] = true;
            }
        }

        //tick game
        Game::OutputData output = game->Tick(input);
        //handle commands
        if (output.Commands.size() > 0)
            for(auto command : output.Commands)
                if (command == Game::COMMAND_CLOSE_GAME)
                    sfWindow_close(wnd);

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

        sfWindow_display(wnd);
    }

    //free game resources
    delete game;
    sfWindow_destroy(wnd);

    return 0;
}
