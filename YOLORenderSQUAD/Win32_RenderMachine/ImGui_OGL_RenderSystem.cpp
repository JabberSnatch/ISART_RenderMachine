#include "ImGui_OGL_RenderSystem.hpp"

#include "OGL_Shader.hpp"
#include "glew\include\GL\glew.h"

static GLuint		g_VBO, g_IBO, g_VAO;
static GLuint		g_ShaderProgram;
static GLuint		g_FontsTexture;


void 
ImGui_OGL_InitResources()
{
	const GLchar* vertex_shader =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	{
		g_ShaderProgram = glCreateProgram();
		GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(hVertexShader, 1, &vertex_shader, 0);
		glShaderSource(hFragmentShader, 1, &fragment_shader, 0);
		glCompileShader(hVertexShader);
		glCompileShader(hFragmentShader);
		glAttachShader(g_ShaderProgram, hVertexShader);
		glAttachShader(g_ShaderProgram, hFragmentShader);
		glLinkProgram(g_ShaderProgram);
	}

	{
		glGenBuffers(1, &g_VBO);
		glGenBuffers(1, &g_IBO);
		glGenVertexArrays(1, &g_VAO);

		glBindVertexArray(g_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, g_VBO);

		GLuint PosLoc = glGetAttribLocation(g_ShaderProgram, "Position");
		GLuint UVLoc = glGetAttribLocation(g_ShaderProgram, "UV");
		GLuint ColorLoc = glGetAttribLocation(g_ShaderProgram, "Color");
		glEnableVertexAttribArray(PosLoc);
		glEnableVertexAttribArray(UVLoc);
		glEnableVertexAttribArray(ColorLoc);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		glVertexAttribPointer(PosLoc, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
		glVertexAttribPointer(UVLoc, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
		glVertexAttribPointer(ColorLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

		glBindVertexArray(0);
	}

	{
		// Build texture atlas
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

		// Upload texture to graphics system
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &g_FontsTexture);
		glBindTexture(GL_TEXTURE_2D, g_FontsTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)g_FontsTexture;
	}
}


void 
ImGui_OGL_RenderDrawLists(ImDrawData* _data)
{
	GLboolean	BlendWasEnabled = glIsEnabled(GL_BLEND);
	GLint		LastBlendEquation;	glGetIntegerv(GL_BLEND_EQUATION, &LastBlendEquation);
	GLint		LastBlendSrc;		glGetIntegerv(GL_BLEND_SRC, &LastBlendSrc);
	GLint		LastBlendDst;		glGetIntegerv(GL_BLEND_DST, &LastBlendDst);
	GLboolean	CullWasEnabled = glIsEnabled(GL_CULL_FACE);
	GLboolean	DepthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean	ScissorWasEnabled = glIsEnabled(GL_SCISSOR_TEST);
	GLint		LastTexture;		glGetIntegerv(GL_ACTIVE_TEXTURE, &LastTexture);


	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);


	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	static const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
		{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
		{ -1.0f,                  1.0f,                   0.0f, 1.0f },
	};
	glUseProgram(g_ShaderProgram);
	glUniform1i(glGetUniformLocation(g_ShaderProgram, "Texture"), 0);
	glUniformMatrix4fv(glGetUniformLocation(g_ShaderProgram, "ProjMtx"), 1, GL_FALSE, &ortho_projection[0][0]);
	glBindVertexArray(g_VAO);

	for (int n = 0; n < _data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = _data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(io.DisplaySize.y - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}


	if (!BlendWasEnabled) glDisable(GL_BLEND);
	glBlendEquation(LastBlendEquation);
	glBlendFunc(LastBlendSrc, LastBlendDst);
	if (CullWasEnabled) glEnable(GL_CULL_FACE);
	if (DepthWasEnabled) glEnable(GL_DEPTH_TEST);
	if (!ScissorWasEnabled) glDisable(GL_SCISSOR_TEST);
	glActiveTexture(LastTexture);
}


void
ImGui_OGL_FreeResources()
{
	glDeleteBuffers(1, &g_VBO);
	glDeleteBuffers(1, &g_IBO);
	glDeleteVertexArrays(1, &g_VAO);
	glDeleteTextures(1, &g_FontsTexture);
	glDeleteProgram(g_ShaderProgram);
}

