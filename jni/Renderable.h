// Renderable.h

#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "tango-gl-renderer/drawable_object.h"
#include "tango-gl-renderer/gl_util.h"

class Renderable {
	public:
		Renderable();
		virtual void Render(glm::mat4, glm::mat4);

	protected:
		GLuint vertex_buffer_;
		GLuint shader_program_;
		GLuint attrib_vertices_;
		GLuint uniform_mvp_mat_;
};

#endif
