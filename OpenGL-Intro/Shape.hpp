#pragma once
#include "Object.hpp"
#include <memory>

class Shape {
private:
	std::shared_ptr<const Object> _object;

protected:
	const GLsizei _vertexCount;
public:
	Shape(GLint size, GLsizei vertexCount, const Object::Vertex* vertex) 
		: _object(new Object(size, vertexCount, vertex))
		, _vertexCount(vertexCount)
	{
	}

	virtual void draw() const final {
		// 頂点配列オブジェクトを結合する
		_object->bind();

		execute();
	}

	virtual void execute() const {
		// 折れ線として描画
		glDrawArrays(GL_LINE_LOOP, 0, _vertexCount);
	}
};
