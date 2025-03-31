#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include <cstdio>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include "Dependencies.h"
#include "Model3D.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

using namespace std;

// moved these 3 to become global variables
// important in order for Model3D's draw function
GLuint shaderProgram;
std::vector<GLuint> mesh_indices;

// camera position modifier
glm::vec3 pos_mod = glm::vec3(0.0f, 0.0f, 0.0f);

float rx_mod = 0.0f;    // yaw
float ry_mod = 0.0f;    // pitch

// camera speed
float speed_mod = 0.2f;
float sens_mod = 0.5f;  // camera sensitivity

bool start = true;
float mouseX = 0.0f;
float mouseY = 0.0f;

float spawnMod = 10.0f; // spawn distance multiplier

float scale_mod = 1.0f; // scale modifier

float fov_mod = 60.0f;  // fov

// cooldown variables
double cooldownTime = 3.0;
double lastSpawnTime = -3.0;
double currentTime = 0.0;

// we store the instances of Model here
vector<Model3D*> vecModels;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mod) {
    // get cameraCenter aka where the camera is currently pointed at
    glm::vec3 cameraCenter;
    cameraCenter.x = cos(glm::radians(rx_mod)) * cos(glm::radians(ry_mod));
    cameraCenter.y = sin(glm::radians(ry_mod));
    cameraCenter.z = sin(glm::radians(rx_mod)) * cos(glm::radians(ry_mod));
    // get side of camera
    glm::vec3 cameraSide = glm::normalize(glm::cross(cameraCenter, glm::vec3(0.0f, 1.0f, 0.0f)));

    // forward
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        pos_mod += speed_mod * cameraCenter;
    }
    // backward
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        pos_mod -= speed_mod * cameraCenter;
    }
    // left
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        pos_mod -= speed_mod * cameraSide;
    }
    // right
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        pos_mod += speed_mod * cameraSide;
    }
    // spawn
    // won't activate if the 3 second cooldown isnt over, this is tracked by getting the current timestamp and subtracting with the previous spawn timestamp
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && (currentTime - lastSpawnTime) > cooldownTime) {
        lastSpawnTime = glfwGetTime();
        glm::vec3 ObjectPos = (spawnMod * cameraCenter) + pos_mod;

        //loading of mesh
        std::string path = "3D/kacsrr3.obj";
        std::vector<tinyobj::shape_t> shape;
        std::vector<tinyobj::material_t> material;
        std::string warning, error;

        tinyobj::attrib_t attributes;

        bool success = tinyobj::LoadObj(&attributes, &shape, &material, &warning, &error, path.c_str());

        for (int i = 0; i < shape[0].mesh.indices.size(); i++) {
            mesh_indices.push_back(shape[0].mesh.indices[i].vertex_index);
        }

        //creation of model
        Model3D* newModel = new Model3D(ObjectPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(scale_mod, scale_mod, scale_mod), attributes.vertices, mesh_indices);


    	vecModels.push_back(newModel);
    }
}

void Mouse_Callback(GLFWwindow* window, double mouseXNew, double mouseYNew) {
    // initialize mouse position
    if (start) {
        mouseX = mouseXNew;
        mouseY = mouseYNew;
        start = false;
    }
    // get difference in mouse position
    float offsetX = mouseXNew - mouseX;
    float offsetY = mouseY - mouseYNew;
    // set current position to the new position
    mouseX = mouseXNew;
    mouseY = mouseYNew;

    // multiply with sensitivity modifier
    rx_mod += offsetX * sens_mod;
    ry_mod += offsetY * sens_mod;

    // prevent upside-down
    if (ry_mod > 89.9f) {
        ry_mod = 89.9f;
    }
    else if (ry_mod < -89.9f) {
        ry_mod = -89.9f;
    }
}

int main(void) {
    std::cout << "Dear ImGui version: " << IMGUI_VERSION << std::endl;

    // from this point on, nothing differs from the code from previous lessons
    GLFWwindow* window;

    /* Initialize the library */
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, Mouse_Callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // shaders
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &v, NULL);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);

    glLinkProgram(shaderProgram);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        /* Render here */
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);

        // get current timestamp
        currentTime = glfwGetTime();

        // loop through vector and call draw function from each Model instances
        for (Model3D* Model : vecModels) {
            Model->draw(pos_mod, rx_mod, ry_mod, fov_mod, shaderProgram);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
