#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Object.hpp"
#include "Shape.hpp"
#include "Window.hpp"

// ---------------------------------------------------------------- //
//	Prototype declaration
// ---------------------------------------------------------------- //
GLuint createProgram(const char* vsrc, const char* fsrc);
GLboolean printShaderInfoLog(GLuint shader, const char* str);
GLboolean printProgramInfoLog(GLuint program);
bool readShaderSource(const char* name, std::vector<GLchar>& buffer);
GLuint loadProgram(const char* vert, const char* frag);

void dprintGLcharVec(const std::vector<GLchar>& buf, const std::string& prefix = "") {
	std::cout << '\n' << prefix << '\n';
	for (const auto& c : buf) {
		std::cout << c;
	}
	std::cout << std::endl;
}

// ---------------------------------------------------------------- //
//	Global variables
// ---------------------------------------------------------------- //

// 描画する矩形の頂点座標
constexpr Object::Vertex rectangleVertices[] =
{
	{ -0.5f, -0.5f },
	{  0.5f, -0.5f },
	{  0.5f,  0.5f },
	{ -0.5f,  0.5f },
};

int main() {
	// GLFWの初期化
	if (glfwInit() == GL_FALSE) {
		std::cerr << "Can't initialize GLFW." << std::endl;
		return 1;
	}

	// プログラム終了時のコールバック関数を登録
	atexit(glfwTerminate);

	// OpenGL v3.2 Core Profileを選択
	// Core ProfileはOpenGLの古い機能をサポートしていない
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // select OpenGL v3.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); // select OpenGL vx.2
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // OpenGL v3.0以前の古い機能を使用しない前方互換プロファイル
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // OpenGL CoreProfileを使用

	// ウィンドウ作成
	Window window;

	// 背景色を指定
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// シェーダプログラムオブジェクトを作成
	const GLuint program(loadProgram("point.vert", "point.frag"));

	// uniform変数の場所を取得
	const GLint sizeLocation(glGetUniformLocation(program, "size"));
	const GLint scaleLocation(glGetUniformLocation(program, "scale"));
	const GLint locationLocation(glGetUniformLocation(program, "location"));

	// 図形データを作成
	std::unique_ptr<const Shape> shapePtr(new Shape(2, 4, rectangleVertices));

	// メインループ
	while (window.shoudClose() == GL_FALSE)
	{
		// ウィンドウを消去
		glClear(GL_COLOR_BUFFER_BIT);

		// シェーダプログラムを使用する
		glUseProgram(program);
	
		// uniform変数の値を設定
		glUniform2fv(sizeLocation, 1, window.getSize());
		glUniform1f(scaleLocation, window.getScaleWorldToDev());
		glUniform2fv(locationLocation, 1, window.getLocation());

		// ここで描画処理
		shapePtr->draw();
		
		window.swapBuffers();
	}
}

/// <summary>
/// シェーダのプログラムオブジェクトを作成する
/// </summary>
/// <param name="vsrc">頂点シェーダのソースコード</param>
/// <param name="fsrc">フラグメントシェーダのソースコード</param>
/// <returns></returns>
GLuint createProgram(const char* vsrc, const char* fsrc)
{
	// 空のプログラムオブジェクトを作成する
	const GLuint program(glCreateProgram());

	if (vsrc != nullptr)
	{
	// バーテックスシェーダのシェーダオブジェクトを作成する
	const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
	glShaderSource(vobj, 1, &vsrc, NULL);
	glCompileShader(vobj);

	// バーテックスシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
	if (printShaderInfoLog(vobj, "vertex shader"))
	  glAttachShader(program, vobj);
	glDeleteShader(vobj);
	}

	if (fsrc != nullptr)
	{
	// フラグメントシェーダのシェーダオブジェクトを作成する
	const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
	glShaderSource(fobj, 1, &fsrc, NULL);
	glCompileShader(fobj);

	// フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
	if (printShaderInfoLog(fobj, "fragment shader"))
	  glAttachShader(program, fobj);
	glDeleteShader(fobj);
	}

	// プログラムオブジェクトをリンクする
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);

	// 作成したプログラムオブジェクトを返す
	if (printProgramInfoLog(program))
	return program;

	// プログラムオブジェクトが作成できなければ 0 を返す
	glDeleteProgram(program);
	return 0;
}

/// <summary>
/// シェーダオブジェクトのコンパイル結果を表示する
/// </summary>
GLboolean printShaderInfoLog(GLuint shader, const char* str)
{
	// コンパイル結果を取得
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) { std::cerr << "Compile error in " << str << std::endl; }

	// シェーダコンパイル時のログの長さを取得
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// シェーダコンパイル時のログの内容を取得する
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

/// <summary>
/// プログラムオブジェクトのリンク結果を表示する
/// </summary>
GLboolean printProgramInfoLog(GLuint program)
{
	// リンク結果を取得する
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) { std::cerr << "Link error." << std::endl; }

	// シェーダのリンク時のログ長を取得
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		// シェーダリンク時のログの内容を出力
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

bool readShaderSource(const char* name, std::vector<GLchar>& buffer)
{
	if (name == nullptr) { return false; }

	// シェーダソース・ファイルを開く
	std::ifstream file(name, std::ios::binary);
	if (file.fail()) {
		std::cerr << "Error: Can't open source file: " << name << std::endl;
		return false;
	}
	// ファイルサイズを取得
	file.seekg(0L, std::ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());

	// ファイルサイズ分のメモリを確保
	buffer.resize(length + 1LL);

	// ファイルを読み出してバッファに書き込む
	file.seekg(0L, std::ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail()) {
		std::cerr << "Error: Could not erad source file: " << name << std::endl;
		file.close();
		return false;
	}

	file.close();
	return true;
}

/// <summary>
/// シェーダのソースファイルを読み込んでプログラムオブジェクトを作成
/// </summary>
/// <param name="vert">頂点シェーダのファイルパス</param>
/// <param name="frag">フラグメントシェーダのファイルパス</param>
/// <returns></returns>
GLuint loadProgram(const char* vert, const char* frag)
{
	std::vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	std::vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));
	
	// プログラムオブジェクトを作る
	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}
