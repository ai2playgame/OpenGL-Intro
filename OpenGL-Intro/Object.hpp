#pragma once
#include <GL/glew.h>

class Object {
private:
	// 頂点配列オブジェクト
	GLuint _vao;
	// 頂点バッファオブジェクト
	GLuint _vbo;

	// UnCopiable
	Object(const Object& o) = delete;
	Object& operator=(const Object& rhs) = delete;

public:
	// 頂点属性
	struct Vertex {
		GLfloat position[2]; // 位置
	};

	Object(GLint size, GLsizei vertexCount, const Vertex* vertex) {
		// 頂点配列オブジェクト作成
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// 頂点バッファオブジェクト（GPU側のメモリ）
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertex, GL_STATIC_DRAW);

		// 結合済みの頂点バッファオブジェクトをシェーダのin変数から参照できるようにする
		glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}

	virtual ~Object() {
		// 頂点配列オブジェクトを削除
		glDeleteBuffers(1, &_vbo);
		// 頂点バッファオブジェクトを削除
		glDeleteBuffers(1, &_vao);
	}

	void bind() const {
		glBindVertexArray(_vao);
	}
};