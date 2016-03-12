#ifndef __IMGUI_OGL_RENDER_SYSTEM_HPP__
#define __IMGUI_OGL_RENDER_SYSTEM_HPP__

#include "imgui.h"

void ImGui_OGL_InitResources();
void ImGui_OGL_RenderDrawLists(ImDrawData* _data);
void ImGui_OGL_FreeResources();


#endif // __IMGUI_OGL_RENDER_SYSTEM_HPP__