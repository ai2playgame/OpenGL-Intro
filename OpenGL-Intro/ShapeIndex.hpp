#pragma once
#include "Shape.hpp"

class ShapeIndex : public Shape {
protected:
	const GLsizei _indexCount;

public:
	ShapeIndex(GLint size, GLsizei vertexCount, const Object::Vertex* vertex,
		GLsizei indexCount, const GLuint* index) 
		: Shape(size, vertexCount, vertex, indexCount, index)
		, _indexCount(indexCount)
	{
	}

	virtual void execute() const {
		glDrawElements(GL_LINES, _indexCount, GL_UNSIGNED_INT, 0);
	}
};
