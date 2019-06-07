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
#include "globalUtils.hpp"

#include "yaol.hpp"
#include "imguiUtil.hpp"
#include "LCRenderer.hpp"

// Functions signatures
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processHoldingKeyInput(GLFWwindow *window);
static void errorCallback(int error, const char* description);
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
TextureStruct loadTextureFile(const char* path);
TextureStruct genBindTexture(TextureStruct st);
TextureStruct refreshTexture(TextureStruct st, GLenum f);

// Global variables, Note is experimental version of cgpg, later this gonna change
const unsigned int SCR_WIDTH = 104 * 16;
const unsigned int SCR_HEIGHT = 104 * 9;

// const unsigned int SCR_WIDTH = 1920;
// const unsigned int SCR_HEIGHT = 1020;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main(){
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

    int cscr_w=SCR_WIDTH, cscr_h=SCR_HEIGHT;
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    RendererLayer mRenderer = RendererLayer();
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 projectionnonsymmetric;
    glm::mat4 projectionsymmetric;
    glm::mat4 projectionClose2GL;
    glm::mat4 mvp;
    glm::mat4 viewPortMatrix;
    bool show_demo_window = false;
    Renderer arc = OPENGL;
    bool useLight = false;
    bool isGouraud = false;
    bool gouraudSpecular = true;
    glm::vec3 lightPosition = glm::vec3(2.0f, 2.0f, 2.0f);
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool perspecCorection = false;
    bool useTexturesFlag = false;
    TextureFiltering filteringOfChoice = NEARESTNEIGHBOR;

    glm::vec3 nCowPosition = glm::vec3(0.0, 0.0, 0.0);
    glfwSetWindowUserPointer(window, &camera);

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
    // ImGui::StyleColorsClassic();
    // ImGui::StyleColorsLight();

    auto font_default = io.Fonts->AddFontDefault();


    // Invert texture load y orientation
    stbi_set_flip_vertically_on_load(true);
    // Load Textures
    TextureStruct mandrilTexture = loadTextureFile("../assets/textures/checker_8x8.jpg");
    // TextureStruct mandrilTexture = loadTextureFile("../assets/textures/mandrill_256.jpg");

    mandrilTexture.addres = GL_TEXTURE0;
    
    mandrilTexture = genBindTexture(mandrilTexture);


    // Load objects(meshes) file
    RenderableObject cubeojb("../assets/models/cube_text.in");
    RenderableObject gisele("../assets/models/cow_up.in");


    // Load, compile and link shaders 
    Shader loadObjectShader("../assets/shaders/loadtest.vert", "../assets/shaders/loadtest.frag");
    Shader close2GLShader("../assets/shaders/close2gl.vert", "../assets/shaders/close2gl.frag");
    Shader c2GLRShader("../assets/shaders/c2glr.vert", "../assets/shaders/c2glr.frag");


    // Bind textures addres to Shader uniform
    loadObjectShader.use(); // don't forget to activate/use the shader before setting uniforms!
    loadObjectShader.setInt("mandrilTexture",  mandrilTexture.addres);


    bool drawCubeFlag = true;
    bool drawCowGiseleFlag = true;
    glm::vec4 objImColors = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    glm::vec4 clear_color = glm::vec4(0.2f, 0.2f, 0.2f, 1.00f); //Clear color

    C2GL::Close2GlRender c2glr = C2GL::Close2GlRender(c2GLRShader, cscr_w, cscr_h, clear_color, objImColors, mandrilTexture);

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

    // OpenGL primitives options
    //{GL_FRONT_AND_BACK, GL_FRONT, GL_BACK}, {GL_POINT, GL_LINE, GL_FILL}
    GLuint drawPrimitive = GL_FILL;
    GLuint faceCulling = GL_BACK;
    GLuint orientationMode = GL_CCW;
    bool backFaceCullingFlag = false;
    glPolygonMode(GL_FRONT, drawPrimitive);
    // Render loop
    glfwSwapInterval(0);
    double previousTime = glfwGetTime();
    int frameCount = 0;
    int gfps = 0;

    while (!glfwWindowShouldClose(window)){
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        double currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        if ( currentTime - previousTime >= 1.0 ){
            gfps = frameCount;

            frameCount = 0;
            previousTime = currentTime;
        }

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

        // create transformations
        glfwGetWindowSize(window, &cscr_w, &cscr_h);// This function is threadsafe
        projectionsymmetric = glm::perspective(glm::radians(camera.Zoom), (float)cscr_w / (float)cscr_h, mRenderer.znear, mRenderer.zfar);
        float h = glm::tan(glm::radians(camera.Zoom) * .5f) * mRenderer.znear;
        float w = h * ((float)cscr_w / (float)cscr_h);
        float frustumLeft = -w;
        float frustumRight = w;
        float frustumBottom = -h;
        float frustumTop = h;
        projectionnonsymmetric = glm::frustum(frustumLeft, frustumRight, frustumBottom, frustumTop, mRenderer.znear, mRenderer.zfar);
        projectionClose2GL = C2GL::frustum(frustumLeft, frustumRight, frustumBottom, frustumTop, mRenderer.znear, mRenderer.zfar);
        // std::cout << "l r b t" << frustumLeft << ", " << frustumRight << ", " << frustumBottom << ", " << frustumTop << "\n";

        


        glPolygonMode(GL_FRONT_AND_BACK, drawPrimitive);
        
        glm::vec3 colorObejects(objImColors.x, objImColors.y, objImColors.z);//   objImColors
        
        //  Draw Cals
        if(arc ==  OPENGL){
            if(backFaceCullingFlag){
                glEnable(GL_CULL_FACE);
            }else{
                glDisable(GL_CULL_FACE);
            }
            projection = projectionsymmetric;
            view = camera.GetViewMatrix();
            loadObjectShader.use();
            loadObjectShader.setMat4("view", view);
            loadObjectShader.setMat4("projection", projection);
            loadObjectShader.setVec3("uColor", colorObejects);

            loadObjectShader.setVec3("lcolor", lightColor);
            loadObjectShader.setVec3("lightPos", lightPosition);
            // loadObjectShader.setVec3("viewPos", camera.Position);
            loadObjectShader.setBool("useLight", useLight);
            loadObjectShader.setBool("isGouraud", isGouraud);
            loadObjectShader.setBool("gouraudSpecular", gouraudSpecular);
            loadObjectShader.setBool("useTexturesFlag", useTexturesFlag);

            // bind textures on corresponding texture units
            glActiveTexture(mandrilTexture.addres);
            glBindTexture(GL_TEXTURE_2D, mandrilTexture.ID);
            
            

            if(drawCubeFlag){
                loadObjectShader.setBool("hasTexCoords", cubeojb.hasTexCoords);
                loadObjectShader.setMat4("model", cubeojb.modelMatrix);
                mvp = projection * view * cubeojb.modelMatrix;
                loadObjectShader.setMat4("mvp", mvp);

                cubeojb.drawTriangles();
            }


            if(drawCowGiseleFlag){
                loadObjectShader.setBool("hasTexCoords", gisele.hasTexCoords);
                loadObjectShader.setVec3("uColor" , glm::vec3(1.0f, 0.0f, 0.0f));
                loadObjectShader.setMat4("model", glm::translate(gisele.modelMatrix, nCowPosition * 100.0f));
                mvp = projection * view * glm::translate(gisele.modelMatrix, nCowPosition * 100.0f);
                loadObjectShader.setMat4("mvp", mvp);

                gisele.drawTriangles();
            }
        }else if(arc ==  CLOSE2GL){
            projection = projectionClose2GL;
            view = camera.lookAtClose2GL();
            close2GLShader.use();
            close2GLShader.setVec3("uColor", colorObejects);
            
            
            if(drawCubeFlag){
                mvp = projection * view * cubeojb.modelClose2GL;
                cubeojb.updateClose2GLVertices(mvp);
                cubeojb.updateClose2GLBuffers();
                cubeojb.drawTrianglesClose2GL();
            }

            if(drawCowGiseleFlag){
                mvp = projection * view * gisele.modelClose2GL;
                gisele.updateClose2GLVertices(mvp);
                gisele.updateClose2GLBuffers();
                gisele.drawTrianglesClose2GL();
            }

        }else{
            glDisable(GL_CULL_FACE);
            //getViewportMatrix
            viewPortMatrix = C2GL::getViewPortMatrix(cscr_w, cscr_h);

            // Update ClearColor
            c2glr.updateClearColor(clear_color);
            // TestResizeBuffer
            c2glr.testAndResizeBuffers(cscr_w, cscr_h);

            c2glr.faceCulling = faceCulling;
            c2glr.orientationMode = orientationMode;
            c2glr.backFaceCullingFlag = backFaceCullingFlag;



            //set the OpenGL primitive to GL_FILL and pass the primitive variable to rasterizer
            c2glr.setPrimitiveToRasterize(drawPrimitive);
            c2glr.updateObjectColor(objImColors);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            projection = projectionClose2GL;
            view = camera.lookAtClose2GL();
            
            //Clear Buffers
            c2glr.clearTextureColor();
            c2glr.clearZBufferC2GL();

            // Set uniform equivalent options for close2GL
            c2glr.useLight = useLight;
            c2glr.isGouraud = isGouraud;
            c2glr.view = view;
            c2glr.lightColor = lightColor;
            c2glr.lightPosition = lightPosition;
            c2glr.gouraudSpecular = gouraudSpecular;
            c2glr.perspecCorection = perspecCorection;
            c2glr.projection = projection;
            c2glr.useTexturesFlag = useTexturesFlag;

            if(drawCubeFlag){
                mvp = projection * view * cubeojb.modelClose2GL;
                cubeojb.updateClose2GLVertices(mvp, viewPortMatrix, cubeojb.modelClose2GL, view);
                c2glr.model = cubeojb.modelClose2GL;
                c2glr.rasterize(cubeojb.C2GLRasVert, cubeojb.hasTexCoords);

            }
            
            if (drawCowGiseleFlag){
                c2glr.updateObjectColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                mvp = projection * view * glm::translate(gisele.modelClose2GL, nCowPosition * 100.0f);
                // gisele.updateClose2GLVertices(mvp, viewPortMatrix, gisele.modelClose2GL, view); To-Do: Use this
                gisele.updateClose2GLVertices(mvp, viewPortMatrix, glm::translate(gisele.modelClose2GL, nCowPosition * 100.0f), view);
                // c2glr.model = gisele.modelClose2GL; To-Do: Use this line
                c2glr.model = glm::translate(gisele.modelClose2GL, nCowPosition * 100.0f);
                c2glr.rasterize(gisele.C2GLRasVert, gisele.hasTexCoords);

            }
            
            c2glr.updateTextureInGPU();
            c2glr.draw();
        }
        

        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        
        if (showMainMenu){
        ImGui::SetNextWindowPos(ImVec2(1200, 4), ImGuiCond_Once); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::SetNextWindowSize(ImVec2(450, 900), ImGuiCond_Once);

            ImGui::Begin("Main Menu", p_open, window_flags);
            if (ImGui::CollapsingHeader("Models")){
                ImGui::Text("Loaded Cube:");
                ImGui::Checkbox("DrawCube", &drawCubeFlag);
                ImGui::Text("Loaded Cow:");
                ImGui::Checkbox("Draw Cow", &drawCowGiseleFlag);
                ImGui::Indent();{
                    ImGui::SliderFloat("X", &nCowPosition.x, -20.0f, 20.0f);
                    ImGui::SliderFloat("Y", &nCowPosition.y, -20.0f, 20.0f);
                    ImGui::SliderFloat("Z", &nCowPosition.z, -20.0f, 20.0f);
                }
                ImGui::Unindent();

                ImGui::Text("Objects color:");
                ImGui::Indent();{
                    ImGui::ColorEdit3("objects color", (float*)&objImColors);
                }
                ImGui::Unindent();
            }
            if(ImGui::CollapsingHeader("Textures")){

                ImGui::BulletText("Use Textures");
                ImGui::Indent();{
                    ImGui::Checkbox("Use Textures Flag", &useTexturesFlag);
                }
                ImGui::Unindent();

                ImGui::BulletText("Texture Filtering");
                ImGui::Indent();{
                    if (ImGui::RadioButton("NEARESTNEIGHBOR", filteringOfChoice == NEARESTNEIGHBOR)) {
                        filteringOfChoice = NEARESTNEIGHBOR;
                        mandrilTexture = refreshTexture(mandrilTexture, GL_NEAREST);
                    }
                    if (ImGui::RadioButton("BILINEAR", filteringOfChoice == BILINEAR)) {
                        filteringOfChoice = BILINEAR;
                        mandrilTexture = refreshTexture(mandrilTexture, GL_LINEAR);
                    }
                    if (ImGui::RadioButton("TRILINEAR", filteringOfChoice == TRILINEAR)) {
                        filteringOfChoice = TRILINEAR;
                        mandrilTexture = refreshTexture(mandrilTexture, GL_LINEAR_MIPMAP_LINEAR);
                    }
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

                ImGui::BulletText("BackFace Culling");
                ImGui::Indent();{
                    // if (ImGui::Button("Enable BFCulling")) {
                    //     // glEnable(GL_CULL_FACE);
                    //     backFaceCullingFlag = true;
                    // } ImGui::SameLine();
                    // if (ImGui::Button("Disable BFCulling")) {
                    //     // glDisable(GL_CULL_FACE);
                    //     backFaceCullingFlag = false;
                    // }
                    ImGui::Checkbox("BackFaceCulling Flag", &backFaceCullingFlag);
                }
                ImGui::Unindent();

                ImGui::BulletText("Perspective Corection");
                ImGui::Indent();{
                    ImGui::Checkbox("Perspective Corection Flag", &perspecCorection);
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
                ImGui::Text("Application (%d FPS)", gfps);
                ImGui::Text("Imgui average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            if (ImGui::CollapsingHeader("Render")){

                ImGui::BulletText("Change Render mode");
                ImGui::Indent();{
                    if (ImGui::RadioButton("OPENGL", arc == OPENGL)) { arc = OPENGL; }
                    if (ImGui::RadioButton("CLOSE2GL", arc == CLOSE2GL)) { arc = CLOSE2GL; }
                    if (ImGui::RadioButton("CLOSE2GLRASTERIZATION", arc == CLOSE2GLRASTERIZATION)) { arc = CLOSE2GLRASTERIZATION; }
                }
                ImGui::Unindent();

                tabularGlmMat4("Projection", projection);
                tabularGlmMat4("View", view);
                tabularGlmMat4("MVP", mvp);
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
                    ImGui::SliderFloat("Z near", &mRenderer.znear, 0.01f, mRenderer.zfar);
                    ImGui::SliderFloat("Z far", &mRenderer.zfar, mRenderer.znear, 60.0f);
                    ImGui::BulletText("FoV is in Camera section, as Zoom");
                }
                ImGui::Unindent();
            }
            if (ImGui::CollapsingHeader("Light")){
                ImGui::BulletText("on/off");
                ImGui::Indent();{
                    if (ImGui::RadioButton("ON", useLight)) { useLight = true; }
                    if (ImGui::RadioButton("OFF", !useLight)) { useLight = false; }
                }
                ImGui::Unindent();

                ImGui::BulletText("Light shading type");
                ImGui::Indent();{
                    if (ImGui::RadioButton("Gouraud", isGouraud)) { isGouraud = true; }
                    if (ImGui::RadioButton("Phong", !isGouraud)) { isGouraud = false; }
                }
                ImGui::Unindent();

                ImGui::BulletText("Gouraud style");
                ImGui::Indent();{
                    if (ImGui::RadioButton("GouraudADS", gouraudSpecular)) { gouraudSpecular = true; }
                    if (ImGui::RadioButton("GouraudAD", !gouraudSpecular)) { gouraudSpecular = false; }
                }
                ImGui::Unindent();

                ImGui::ColorEdit3("Light color", (float*)&lightColor);
                ImGui::BulletText("Light position");
                ImGui::Indent();{
                    ImGui::SliderFloat("X##lp", &lightPosition.x, -20.0f, 20.0f);
                    ImGui::SliderFloat("Y##lp", &lightPosition.y, -20.0f, 20.0f);
                    ImGui::SliderFloat("Z##lp", &lightPosition.z, -20.0f, 20.0f);
                }
                ImGui::Unindent();
            }
            ImGui::End();
        }
        // Imgui Render Calls
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


    glDeleteVertexArrays(1, &cubeojb.VAO);
    glDeleteBuffers(1, &cubeojb.VBO);
    
    glDeleteVertexArrays(1, &gisele.VAO);
    glDeleteBuffers(1, &gisele.VBO);

    glDeleteVertexArrays(1, &cubeojb.C2GLVAO);
    glDeleteBuffers(1, &cubeojb.C2GLVBO);
    
    glDeleteVertexArrays(1, &gisele.C2GLVAO);
    glDeleteBuffers(1, &gisele.C2GLVBO);


    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    std::cout << "Alert: window size changed to " << width << " x " << height << "\n";
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
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    int toTest = glfwGetInputMode(window, GLFW_CURSOR);// Return GLFW_CURSOR_DISABLED or GLFW_CURSOR_NORMAL
    if(toTest == GLFW_CURSOR_DISABLED){
        Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    int toTest = glfwGetInputMode(window, GLFW_CURSOR);// Return GLFW_CURSOR_DISABLED or GLFW_CURSOR_NORMAL
    if(toTest == GLFW_CURSOR_DISABLED){
        Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
        camera->ProcessMouseScroll(yoffset);
    }
}

static void errorCallback(int error, const char* description){
    std::cout << "Error: " << description << "\n";
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){// Input callBack
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){// Close window
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS){// Change the way to interact with mouse
        int toTest = glfwGetInputMode(window, GLFW_CURSOR);// Return GLFW_CURSOR_DISABLED or GLFW_CURSOR_NORMAL
        if(toTest == GLFW_CURSOR_DISABLED){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }else{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    
}

TextureStruct loadTextureFile(const char* path){
    TextureStruct st;
    // int width, height, nrChannels;
    st.data = stbi_load(path, &st.width, &st.height, &st.channels, 0);
    if (st.data){
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, st.width, st.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture loaded,  (width, height, channels): (" 
        << st.width << ", " << st.height << ", " << st.channels << ")\n\n";
    }
    else{
        std::cout << "Failed to load in " << path << std::endl;
        exit(EXIT_FAILURE);
    }
    return st;
}

TextureStruct genBindTexture(TextureStruct st){
    //to-do selectables Filter options
    glGenTextures(1, &st.ID);
    glBindTexture(GL_TEXTURE_2D, st.ID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // stbi_set_flip_vertically_on_load(true);
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    if (st.data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, st.width, st.height, 0, GL_RGB, GL_UNSIGNED_BYTE, st.data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    return st;
}

TextureStruct refreshTexture(TextureStruct st, GLenum f){
    //to-do selectables Filter options
    glGenTextures(1, &st.ID);
    glBindTexture(GL_TEXTURE_2D, st.ID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, f);
    
    // stbi_set_flip_vertically_on_load(true);
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    if (st.data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, st.width, st.height, 0, GL_RGB, GL_UNSIGNED_BYTE, st.data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    return st;

}