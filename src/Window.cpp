#include "Window.h"
#include <Skin.h>
#include <Skeleton.h>
#include <AnimClip.h>

extern Skin* skin;
extern Skeleton* skel;
extern AnimClip* clip;
extern int mode;

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Model Environment";

// Objects to render
//Cube* Window::cube;
std::vector<Cube*> Window::cubes;
Skin* Window::skin = nullptr;
bool Window::skinFlag = false;
int Window::currentDOF = 0;
float Window::currentTime = 0;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// Constructors and desctructors
bool Window::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    return true;
}

bool Window::initializeObjects() {
    // Create a cube
    // cube = new Cube();
    cubes = std::vector<Cube*>();
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

    return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    //delete cube;
    for (int i = 0; i < cubes.size(); i++) {
        delete cubes[i];
    }

    // Delete the shader program.
    glDeleteProgram(shaderProgram);
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewInit();

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.
    Cam->Update();

    //cube->update();
}

void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    //cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    if (!Window::skinFlag) {
        for (int i = 0; i < cubes.size(); i++) {
            cubes[i]->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
        }
    }
    else {
        skin->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    }
    

    // Gets events, including input such as keyboard and mouse or window resizing.
    glfwPollEvents();
    // Swap buffers.
    glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_R:
                resetCamera();
                break;

            case GLFW_KEY_A:
                if (mode != 1) {
                    if (currentDOF > 0) {
                        currentDOF--;
                        std::cout << "New DOF: " << Skeleton::DOFs[currentDOF]->GetName() << std::endl;
                    }
                }
                break;

            case GLFW_KEY_D:
                if (mode != 1) {
                    if (currentDOF < Skeleton::DOFs.size() - 1) {
                        currentDOF++;
                        std::cout << "New DOF: " << Skeleton::DOFs[currentDOF]->GetName() << std::endl;
                    }
                }
                break;

            case GLFW_KEY_W:
                if (mode != 1) {
                    Skeleton::DOFs[currentDOF]->SetValue(Skeleton::DOFs[currentDOF]->GetValue() + 0.1);
                    std::cout << "New DOF Value: " << Skeleton::DOFs[currentDOF]->GetValue() << std::endl;
                    skel->Update();
                    if (mode == 0) {
                        skel->Draw();
                    }
                    else {
                        skin->Update();
                        skin->BeginDraw();
                    }
                }
                break;

            case GLFW_KEY_S:
                if (mode != 1) {
                    Skeleton::DOFs[currentDOF]->SetValue(Skeleton::DOFs[currentDOF]->GetValue() - 0.1);
                    std::cout << "New DOF Value: " << Skeleton::DOFs[currentDOF]->GetValue() << std::endl;
                    skel->Update();
                    if (mode == 0) {
                        skel->Draw();
                    }
                    else {
                        skin->Update();
                        skin->BeginDraw();
                    }
                }
                break;

            case GLFW_KEY_O:
                if (mode != 1) {
                    currentTime -= 0.1;
                    clip->Evaluate(currentTime);
                    std::cout << "t: " << currentTime << std::endl;
                    skel->Update();
                    if (mode == 0) {
                        skel->Draw();
                    }
                    else {
                        skin->Update();
                        skin->BeginDraw();
                    }
                }
                break;

            case GLFW_KEY_P:
                if (mode != 1) {
                    currentTime += 0.1;
                    clip->Evaluate(currentTime);
                    std::cout << "t: " << currentTime << std::endl;
                    skel->Update();
                    if (mode == 0) {
                        skel->Draw();
                    }
                    else {
                        skin->Update();
                        skin->BeginDraw();
                    }
                }
                break;

            default:
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown) {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown) {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}