#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shaderLoader.hpp"
#include "camera.hpp"

#include "yaol.hpp"


// Functions signatures
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processHoldingKeyInput(GLFWwindow *window);
static void errorCallback(int error, const char* description);
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Global variables, Note is experimental version of cgpg, later this gonna change
const unsigned int SCR_WIDTH = 104 * 16;
const unsigned int SCR_HEIGHT = 104 * 9;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main(){
    float znear = 1.0f, zfar = 3000.0f;

    std::cout << "Compiled against GLFW: " <<  GLFW_VERSION_MAJOR << "."
    << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << "\n";

    //set the callback to show glfw errors
    glfwSetErrorCallback(errorCallback);

    // Initialize GLFW and OpenGL version of choice
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // only for MacOS machines
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = 
    glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cgpg", NULL, NULL);
    if (!window){
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // Set callbacks for GLFWwindow
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD\n";
        exit(EXIT_FAILURE);
    }

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    glfwSetWindowUserPointer(window, &camera);
    bool show_demo_window = true;

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // ImGui::StyleColorsLight();

    auto font_default = io.Fonts->AddFontDefault();

    RendableObject cubeojb("../assets/models/cube.in");
    RendableObject gisele("../assets/models/cow_up.in");


    // Load, compile and link shaders 
    Shader shaderObj("../assets/shaders/simple.vert", "../assets/shaders/simple.frag");
    Shader loadObjectShader("../assets/shaders/loadtest.vert", "../assets/shaders/loadtest.frag");

    bool drawCubeFlag = true;
    bool drawCowGiseleFlag = true;
    bool drawAwesomeContainers = true;
    ImVec4 objImColors = ImVec4(0.7f, 0.7f, 0.7f, 1.0f); //Clear color


    int cscr_w=SCR_WIDTH, cscr_h=SCR_HEIGHT;
    bool showMainMenu = true;

    bool no_titlebar = false;
    bool no_scrollbar = false;
    bool no_menu = true;
    bool no_move = false;
    bool no_resize = false;
    bool no_collapse = false;
    bool no_close = true;
    bool no_nav = false;
    bool no_background = false;
    bool no_bring_to_front = false;
    bool *p_open = &showMainMenu;
    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_close)           p_open = NULL;
    
    float vertices[] = {
        // positions       // colors          // texture coords
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // top left      2
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right  1
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  // top right    3

         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // top left      2
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom left   0
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // bottom right  1


         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // bottom left   4
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right  5
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // top left      6
        
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // top left      6
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // bottom right  5
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // top right     7


         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom left   0
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // bottom left   4
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // top left      2

         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // top left      2
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // bottom left   4
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // top left      6


        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right  1
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // bottom right  5
        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // top right     3

        -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // top right     3
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // bottom right  5
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // top right     7


        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // bottom left   0
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // bottom right  1
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // bottom left   4

         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // bottom left   4
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // bottom right  1
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // bottom right  5


         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // top left      2
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // top right     3
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // top left      6

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // top left      6
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f, // top right     3
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f  // top right     7
    };

    //Cubes positions
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  8.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); 
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texure coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // load and create a texture 
    // -------------------------
    // texture 1
    // ---------
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); 
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("../assets/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture1" << std::endl;
    }
    stbi_image_free(data);

    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("../assets/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data){
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture2" << std::endl;
    }
    stbi_image_free(data);

    shaderObj.use(); // don't forget to activate/use the shader before setting uniforms!
    shaderObj.setInt("texture1", 0);
    shaderObj.setInt("texture2", 1);


    // Imgui variables
    ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f); //Clear color

    // OpenGL primitives options
    //{GL_FRONT_AND_BACK, GL_FRONT, GL_BACK}, {GL_POINT, GL_LINE, GL_FILL}
    GLuint drawPrimitive = GL_FILL;
    GLuint faceCulling = GL_BACK;
    GLuint orientationMode = GL_CCW;
    glPolygonMode(GL_FRONT, drawPrimitive);
    // Render loop
    while (!glfwWindowShouldClose(window)){
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        // Input
        processHoldingKeyInput(window);
        
        // Render
        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        
        glCullFace(faceCulling);
        glFrontFace(orientationMode);


        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        // Active shader
        shaderObj.use();

        // create transformations
        glm::mat4 model; // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);

        glfwGetWindowSize(window, &cscr_w, &cscr_h);//essa função é threadsafe
        projection = glm::perspective(glm::radians(camera.Zoom), (float)cscr_w / (float)cscr_h, znear, zfar);

        view = camera.GetViewMatrix();

        // get matrix's uniform location and set matrix
        shaderObj.setMat4("view", view);
        shaderObj.setMat4("projection", projection);


        glPolygonMode(GL_FRONT_AND_BACK, drawPrimitive);
        //  Draw Cals

        if(drawAwesomeContainers){
            glBindVertexArray(VAO);
            // glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
            for(unsigned int i = 0; i < 10; i++){
                model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * (i +1); 
                // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                shaderObj.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
            }
            glBindVertexArray(0);
        }


        loadObjectShader.use();
        loadObjectShader.setMat4("view", view);
        loadObjectShader.setMat4("projection", projection);
        glm::vec3 colorObejects(objImColors.x, objImColors.y, objImColors.z);//   objImColors
        loadObjectShader.setVec3("uColor", colorObejects);
        
        
        float scalasetNorm;

        if(drawCubeFlag){
            scalasetNorm = 1.5f/cubeojb.getMaxOffsetsize();
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(scalasetNorm , scalasetNorm , scalasetNorm));
            model = glm::translate(model, cubeojb.getCenterBBoxOCS() * -1.0f);
            loadObjectShader.setMat4("model", model);

            glBindVertexArray(cubeojb.VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }


        if(drawCowGiseleFlag){
            scalasetNorm = 1.5f/gisele.getMaxOffsetsize();
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(scalasetNorm, scalasetNorm, scalasetNorm));
            model = glm::translate(model, gisele.getCenterBBoxOCS() * -1.0f);
            loadObjectShader.setMat4("model", model);
            glBindVertexArray(gisele.VAO);
            glDrawArrays(GL_TRIANGLES, 0, gisele.vertices.size());
            glBindVertexArray(0);
        }
        

        // if (show_demo_window)
        // {
        //     ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        //     ImGui::ShowDemoWindow(&show_demo_window);
        // }

        
        if (showMainMenu){
        ImGui::SetNextWindowPos(ImVec2(1200, 4), ImGuiCond_Once); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::SetNextWindowSize(ImVec2(450, 900), ImGuiCond_Once);

            ImGui::Begin("Main Menu", p_open, window_flags);
            if (ImGui::CollapsingHeader("Models")){
                ImGui::Text("Loaded Cube:");
                ImGui::Checkbox("DrawCube", &drawCubeFlag);
                ImGui::Text("Loaded Cow:");
                ImGui::Checkbox("Draw Cow", &drawCowGiseleFlag);

                ImGui::Text("Inbuild Awesome Containers:");
                ImGui::Checkbox("Draw Awesome Containers", &drawAwesomeContainers);

                ImGui::Text("Objects color:");
                ImGui::Indent();{
                    ImGui::ColorEdit3("objects color", (float*)&objImColors);
                }
                ImGui::Unindent();
            }
            if (ImGui::CollapsingHeader("General")){
                // {GL_POINT, GL_LINE, GL_FILL}  drawPrimitive
                ImGui::BulletText("Change Primitive");
                ImGui::Indent();{
                    if (ImGui::RadioButton("GL_POINT", drawPrimitive == GL_POINT)) { drawPrimitive = GL_POINT; }
                    if (ImGui::RadioButton("GL_LINE", drawPrimitive == GL_LINE)) { drawPrimitive = GL_LINE; }
                    if (ImGui::RadioButton("GL_FILL", drawPrimitive == GL_FILL)) { drawPrimitive = GL_FILL; }
                }
                ImGui::Unindent();

                // faceCulling
                ImGui::BulletText("Change Culling Option");
                ImGui::Indent();{
                    if (ImGui::RadioButton("GL_FRONT", faceCulling == GL_FRONT)) { faceCulling = GL_FRONT; }
                    if (ImGui::RadioButton("GL_BACK", faceCulling == GL_BACK)) { faceCulling = GL_BACK; }
                    if (ImGui::RadioButton("GL_FRONT_AND_BACK", faceCulling == GL_FRONT_AND_BACK)) { faceCulling = GL_FRONT_AND_BACK; }
                }
                ImGui::Unindent();

                // faceCulling
                ImGui::BulletText("Change Orientation Mode");
                ImGui::Indent();{
                    if (ImGui::RadioButton("GL_CCW", orientationMode == GL_CCW)) { orientationMode = GL_CCW; }
                    if (ImGui::RadioButton("GL_CW", orientationMode == GL_CW)) { orientationMode = GL_CW; }
                }
                ImGui::Unindent();

                ImGui::BulletText("Cull Face");
                ImGui::Indent();{
                    if (ImGui::Button("Enable bf")) {
                        glEnable(GL_CULL_FACE);
                    } ImGui::SameLine();
                    if (ImGui::Button("Disable bf")) {
                        glDisable(GL_CULL_FACE);
                    }
                }
                ImGui::Unindent();


                ImGui::BulletText("Depth test");
                ImGui::Indent();{
                    if (ImGui::Button("Enable")) {
                        glEnable(GL_DEPTH_TEST);
                        std::cout << "Ativando depth teset\n";
                    } ImGui::SameLine();
                    if (ImGui::Button("Disable")) {
                        glDisable(GL_DEPTH_TEST);
                        std::cout << "Desativando depth teset\n";
                    }
                }
                ImGui::Unindent();

                ImGui::BulletText("Clear color");
                ImGui::Indent();{
                    ImGui::ColorEdit3("clear color", (float*)&clear_color);
                }
                ImGui::Unindent();
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            if (ImGui::CollapsingHeader("Camera")){
                ImGui::Separator();
                ImGui::Text("Camera Vectors:");
                ImGui::Columns(4, "mycolumns"); // 4-ways, with border
                ImGui::Separator();
                ImGui::Text("vector"); ImGui::NextColumn();
                ImGui::Text("x"); ImGui::NextColumn();
                ImGui::Text("y"); ImGui::NextColumn();
                ImGui::Text("z"); ImGui::NextColumn();
                ImGui::Separator();

                ImGui::Text("Position");ImGui::NextColumn();
                ImGui::Text("%f", camera.Position.x); ImGui::NextColumn();
                ImGui::Text("%f", camera.Position.y); ImGui::NextColumn();
                ImGui::Text("%f", camera.Position.z); ImGui::NextColumn();

                ImGui::Text("Front");ImGui::NextColumn();
                ImGui::Text("%f", camera.Front.x); ImGui::NextColumn();
                ImGui::Text("%f", camera.Front.y); ImGui::NextColumn();
                ImGui::Text("%f", camera.Front.z); ImGui::NextColumn();

                ImGui::Text("Up");ImGui::NextColumn();
                ImGui::Text("%f", camera.Up.x); ImGui::NextColumn();
                ImGui::Text("%f", camera.Up.y); ImGui::NextColumn();
                ImGui::Text("%f", camera.Up.z); ImGui::NextColumn();

                ImGui::Text("Right");ImGui::NextColumn();
                ImGui::Text("%f", camera.Right.x); ImGui::NextColumn();
                ImGui::Text("%f", camera.Right.y); ImGui::NextColumn();
                ImGui::Text("%f", camera.Right.z); ImGui::NextColumn();
                
                ImGui::Columns(1);

                ImGui::SliderFloat("Zoom", &camera.Zoom, 0.01f, 89.0f);

                ImGui::Indent();{
                    ImGui::Text("Reset Camera position and orientation");
                    if (ImGui::Button("ResetCam")) {
                        camera.reset();
                    } 
                }
                ImGui::Unindent();


                ImGui::BulletText("Change Camera Navigation Mode");
                ImGui::Indent();{
                    if (ImGui::RadioButton("FPS", camera.acMode == FPS)) { camera.acMode = FPS; }
                    if (ImGui::RadioButton("SEEK", camera.acMode == SEEK)) { camera.acMode = SEEK; }
                    // if (ImGui::RadioButton("GL_FRONT_AND_BACK", faceCulling == GL_FRONT_AND_BACK)) { faceCulling = GL_FRONT_AND_BACK; }
                }
                ImGui::Unindent();
            }
            if (ImGui::CollapsingHeader("Projection")){
                ImGui::Indent();{
                    ImGui::SliderFloat("Z near", &znear, 0.01f, zfar);
                    ImGui::SliderFloat("Z far", &zfar, znear, 6000.0f);
                    ImGui::BulletText("FoV is in Camera section, as Zoom");
                }
                ImGui::Unindent();
            }
            // if (ImGui::Button("Close Me")){
            //     showMainMenu = false;
            // }
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &cubeojb.VAO);
    glDeleteBuffers(1, &cubeojb.VBO);
    
    glDeleteVertexArrays(1, &gisele.VAO);
    glDeleteBuffers(1, &gisele.VBO);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    std::cout << "Alert, window size changed to " << width << " x " << height << "\n";
    glViewport(0, 0, width, height);
}

void processHoldingKeyInput(GLFWwindow *window){
    Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
    float cameraSpeed = 2.5f * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera->ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        camera->ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        camera->ProcessKeyboard(DOWN, deltaTime);
    }
}

// GLFW: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    int toTest = glfwGetInputMode(window, GLFW_CURSOR);//return GLFW_CURSOR_DISABLED or GLFW_CURSOR_NORMAL
    if(toTest == GLFW_CURSOR_DISABLED){
        Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    int toTest = glfwGetInputMode(window, GLFW_CURSOR);//return GLFW_CURSOR_DISABLED or GLFW_CURSOR_NORMAL
    if(toTest == GLFW_CURSOR_DISABLED){
        Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->ProcessMouseScroll(yoffset);
    }
}

static void errorCallback(int error, const char* description){
    std::cout << "Error: " << description << "\n";
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){//input callBack
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){//close window
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS){// Change the way to interact with mouse
        int toTest = glfwGetInputMode(window, GLFW_CURSOR);//return GLFW_CURSOR_DISABLED or GLFW_CURSOR_NORMAL
        if(toTest == GLFW_CURSOR_DISABLED){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    
}
