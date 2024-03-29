#ifndef IMGUIC2GL_UTILS
#define IMGUIC2GL_UTILS

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void tabularGlmMat4(const char* title, glm::mat4 matrix){
    ImGui::Separator();
    ImGui::Text("%s:", title);
    ImGui::Columns(4, "mycolumns"); // 4-ways, with border
    ImGui::Separator();
    for(unsigned int ai = 0; ai < 4; ai++){
        ImGui::Text("%f", matrix[0][ai]); ImGui::NextColumn();
        ImGui::Text("%f", matrix[1][ai]); ImGui::NextColumn();
        ImGui::Text("%f", matrix[2][ai]); ImGui::NextColumn();
        ImGui::Text("%f", matrix[3][ai]); ImGui::NextColumn();
        ImGui::Separator();
    }
    ImGui::Columns(1);
}

#endif