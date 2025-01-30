#include "Window.h"
#include "core.h"
#include <Skeleton.h>
#include <Skin.h>

Skeleton* skel;
Skin* skin;
int mode;

void error_callback(int error, const char* description) {
    // Print error.
    std::cerr << description << std::endl;
}

void setup_callbacks(GLFWwindow* window) {
    // Set the error callback.
    glfwSetErrorCallback(error_callback);
    // Set the window resize callback.
    glfwSetWindowSizeCallback(window, Window::resizeCallback);

    // Set the key callback.
    glfwSetKeyCallback(window, Window::keyCallback);

    // Set the mouse and cursor callbacks
    glfwSetMouseButtonCallback(window, Window::mouse_callback);
    glfwSetCursorPosCallback(window, Window::cursor_callback);
}

void setup_opengl_settings() {
    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);
    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);
    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void print_versions() {
    // Get info of GPU and supported OpenGL version.
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
              << std::endl;

    // If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
    std::cout << "Supported GLSL version is: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

int main(int argc, char* argv[]) {
    // Create the GLFW window.
    GLFWwindow* window = Window::createWindow(800, 600);
    if (!window) exit(EXIT_FAILURE);

    // Print OpenGL and GLSL versions.
    print_versions();
    // Setup callbacks.
    setup_callbacks(window);
    // Setup OpenGL settings.
    setup_opengl_settings();

    // Initialize the shader program; exit if initialization fails.
    if (!Window::initializeProgram()) exit(EXIT_FAILURE);
    // Initialize objects/pointers for rendering; exit if initialization fails.
    if (!Window::initializeObjects()) exit(EXIT_FAILURE);

    mode = 0;

    //process command-line input
    if (argc == 1) {
        mode = 0;
        skel = new Skeleton();
        skel->Load("test.skel");
        skel->Update();
        skel->Draw();
    }
    else if (argc == 2) {
        char* input = argv[1];
        std::string inp = input;
        if (inp.find(std::string("skel")) != std::string::npos) {
            mode = 0;
            skel = new Skeleton();
            skel->Load(input);
            skel->Update();
            skel->Draw();
        }
        else if (inp.find(std::string("skin")) != std::string::npos) {
            mode = 1;
            skin = new Skin();
            skin->Load(input);
            skin->Update();
            skin->BeginDraw();
        }
    }
    else if (argc == 3) {
        mode = 2;
        char* skelput = argv[1];
        char* skinput = argv[2];

        skel = new Skeleton();
        skel->Load(skelput);

        skin = new Skin();
        skin->Load(skinput);

        skel->Update();
        skin->Update();

        skin->BeginDraw();
    }

    
    
    





    // Loop while GLFW window should stay open.
    while (!glfwWindowShouldClose(window)) {
        // Main render display callback. Rendering of objects is done here.
        Window::displayCallback(window);

        // Idle callback. Updating objects, etc. can be done here.
        Window::idleCallback();
    }

    Window::cleanUp();
    delete skel;
    // Destroy the window.
    glfwDestroyWindow(window);
    // Terminate GLFW.
    glfwTerminate();

    exit(EXIT_SUCCESS);
}