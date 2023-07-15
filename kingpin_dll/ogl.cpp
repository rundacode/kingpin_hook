#include "ogl.h"

namespace original
{
	glenable_t o_glenable;
}

namespace ogl
{
	void draw_box(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height)
	{
		glPushAttrib(GL_CURRENT_BIT);
		glColor3f(200, 255, 0);

		glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x + width, y, z);
		glVertex3f(x + width, y + height, z);
		glVertex3f(x, y + height, z);
		glEnd();

		glPopAttrib();
	}

	void draw_outlined_box(GLfloat x, GLfloat y, GLfloat width, GLfloat height)
	{
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBegin(GL_POLYGON);
		glVertex2d(50, 90);
		glVertex2d(100, 90);
		glVertex2d(100, 150);
		glVertex2d(50, 150);
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glFlush();
	}

	void draw_snapline(GLfloat x, GLfloat y, GLfloat x1, GLfloat y1)
	{

	}
}

namespace cheat
{
	void WINAPI hooked_GLEnable(GLenum bit)
	{
		


		original::o_glenable(bit);
	}
}