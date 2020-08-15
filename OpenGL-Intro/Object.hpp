#pragma once
#include <GL/glew.h>

class Object {
private:
	// 頂点配列オブジェクト
	GLuint _vao;
	// 頂点バッファオブジェクト
	GLuint _vbo;

	// 頂点インデックスバッファオブジェクト
	GLuint _ibo;

	// UnCopiable
	Object(const Object& o) = delete;
	Object& operator=(const Object& rhs) = delete;

public:
	// 頂点属性
	struct Vertex {
		GLfloat position[3]; // 3次元座標 
		GLfloat color[3];	 // 色
	};

	Object(GLint size, GLsizei vertexCount, const Vertex* vertex, GLsizei indexCount = 0, const GLuint* index = nullptr) {
		// 頂点配列オブジェクト作成
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// 頂点バッファオブジェクト（GPU側のメモリ）
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertex, GL_STATIC_DRAW);

		// 結合済みの頂点バッファオブジェクトをシェーダのin変数から参照できるようにする
		glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<char*>(0) + sizeof(vertex->position));
		glEnableVertexAttribArray(1);

		// 頂点インデックスバッファオブジェクト作成
		glGenBuffers(1, &_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), index, GL_STATIC_DRAW);
	}

	virtual ~Object() {
		// 頂点配列オブジェクトを削除
		glDeleteBuffers(1, &_vbo);
		// 頂点バッファオブジェクトを削除
		glDeleteBuffers(1, &_vao);
		// 頂点インデックスバッファオブジェクト削除
		glDeleteBuffers(1, &_ibo);
	}

	void bind() const {
		glBindVertexArray(_vao);
	}
};