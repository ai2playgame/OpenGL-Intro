#pragma once
#include "ShapeIndex.hpp"

class SolidShapeIndex : public ShapeIndex {
private:

public:
	SolidShapeIndex(GLsizei size, GLsizei vertexCount, const Object::Vertex* vertex, GLsizei indexCount, const GLuint* index) 
		: ShapeIndex(size, vertexCount, vertex, indexCount, index)
	{

	}

	virtual void execute() const {
		glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
	}
};
