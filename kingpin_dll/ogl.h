#pragma once

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")

#include "common.h"
#include "globals.h"
#include "kingpin_hooked.h"
#include <gl/GL.h>
#include <gl/GLU.h>

using glbegin_t = void(__stdcall*)(GLenum);
using glend_t = void(__stdcall*)();
using glclear_t = void(__stdcall*)(GLbitfield);
using glenable_t = void(__stdcall*)(GLenum);

namespace original
{
	extern glenable_t o_glenable;
}

namespace cheat
{
	extern void WINAPI hooked_GLEnable(GLenum);
}

namespace ogl
{
	extern void draw_box(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
	extern void draw_outlined_box(GLfloat x, GLfloat y, GLfloat width, GLfloat height);
}