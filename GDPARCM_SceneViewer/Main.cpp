#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include <cstdio>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "iostream"
#include "math.h"
#include "vector"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <imgui/imgui_impl_opengl3.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

using namespace std;

// moved these 3 to become global variables
// important in order for Model3D's draw function
GLuint shaderProgram;
std::vector<GLuint> mesh_indices;
GLuint VAO, VBO, EBO;

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

// Model3D class
class Model3D {
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

public:
    Model3D(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

    // code was ripped from the while() loop in main() and placed into draw() function
    void draw() {

        // axis
        float axis_x = 0.0;
        float axis_y = 1.0;
        float axis_z = 0.0;

        // camera stuff
        glm::mat4 cameraPosMatrix = glm::translate(
            glm::mat4(1.0f),
            pos_mod * -1.0f
        );

        cameraPosMatrix = glm::rotate(cameraPosMatrix, glm::radians(rx_mod), glm::normalize(glm::vec3(axis_x, axis_y, axis_z)));
        cameraPosMatrix = glm::rotate(cameraPosMatrix, glm::radians(ry_mod), glm::normalize(glm::vec3(axis_y, axis_x, axis_z)));

        glm::vec3 worldUp = glm::normalize(glm::vec3(0, 1.0f, 0));

        glm::vec3 cameraCenter;
        cameraCenter.x = cos(glm::radians(rx_mod)) * cos(glm::radians(ry_mod));
        cameraCenter.y = sin(glm::radians(ry_mod));
        cameraCenter.z = sin(glm::radians(rx_mod)) * cos(glm::radians(ry_mod));
        cameraCenter = pos_mod + cameraCenter;

        glm::mat4 viewMatrix = glm::lookAt(pos_mod, cameraCenter, worldUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov_mod), 600.0f / 600.0f, 0.1f, 100.0f);
        // end of camera stuff

        // applying of model transformation
        // translate -> rotate -> scale
        glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), this->position);

        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(this->rotation.x), glm::normalize(glm::vec3(axis_x, axis_y, axis_z)));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(this->rotation.y), glm::normalize(glm::vec3(axis_y, axis_x, axis_z)));

        transformation_matrix = glm::scale(transformation_matrix, this->scale);
        // end of applying of model transformation 

        // sending transformation matrix to shader program
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //set color
        glm::vec4 FragColorIn = glm::vec4(0.0, 1.0, 0.0, 1.0);
        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "FragColorIn");
        glUniform4fv(colorLoc, 1, glm::value_ptr(FragColorIn));
        // end of sending transformation matrix to shader program

        // rendering
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(
            GL_TRIANGLES,
            mesh_indices.size(),
            GL_UNSIGNED_INT,
            0
        );
        // end
    }

    void setPosition(glm::vec3 position) {
        this->position = position;
    }
    void setRotation(glm::vec3 rotation) {
        this->rotation = rotation;
    }
    void setScale(glm::vec3 scale) {
        this->scale = scale;
    }
};

// we store the instances of Model3D here
vector<Model3D*> vecModels;

void Key_Callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mod
) {

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
        Model3D* newModel = new Model3D(ObjectPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(scale_mod, scale_mod, scale_mod));
        vecModels.push_back(newModel);
    }
}

void Mouse_Callback(
    GLFWwindow* window,
    double mouseXNew,
    double mouseYNew
) {
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

int main(void)
{
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
    if (!window)
    {
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


    //shaders

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

    std::string path = "3D/kacsrr3.obj";
    std::vector<tinyobj::shape_t> shape;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shape,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    for (int i = 0; i < shape[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shape[0].mesh.indices[i].vertex_index
        );
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //text
    glBindVertexArray(VAO);
    //text
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * attributes.vertices.size(),
        &attributes.vertices[0],
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3, //xyz
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GL_FLOAT),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
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

        //get current timestamp
        currentTime = glfwGetTime();

        //loop through vector and call draw function from each Model3D instances
        for (Model3D* Model : vecModels) {
            Model->draw();
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}