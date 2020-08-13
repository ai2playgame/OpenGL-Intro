#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint createProgram(const char* vsrc, const char* fsrc);
GLboolean printShaderInfoLog(GLuint shader, const char* str);
GLboolean printProgramInfoLog(GLuint program);
bool readShaderSource(const char* name, std::vector<GLchar>& buffer);
GLuint loadProgram(const char* vert, const char* frag);

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
	GLFWwindow* const window(glfwCreateWindow(640, 480, "Hello OpenGL!", nullptr, nullptr));
	if (window == nullptr) {
		std::cerr << "Can't create GLFW window." << std::endl;
		return 1;
	}
	// 作ったウィンドウをOpenGLの処理対象に変える
	glfwMakeContextCurrent(window);

	// GLEWを初期化する
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Can't initialize GLEW." << std::endl;
		return 1;
	}

	// 垂直同期のタイミングを待つ
	glfwSwapInterval(1);

	// 背景色を指定
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	const GLuint program(loadProgram("point.vert", "point.frag"));

	// メインループ
	while (glfwWindowShouldClose(window) == GL_FALSE)
	{
		// ウィンドウを消去
		glClear(GL_COLOR_BUFFER_BIT);

		// ここで描画処理
		
		// カラーバッファを入れ替える
		glfwSwapBuffers(window);

		// イベントを取り出す
		glfwWaitEvents();
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
	// 空のプログラムオブジェクトを作成
	const GLuint program(glCreateProgram());
	if (vsrc != nullptr) {
		// 頂点シェーダのシェーダオブジェクトを作成
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, nullptr);
		glCompileShader(vobj);

		// 頂点シェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(vobj, "vertex shader")) {
			glAttachShader(program, vobj);
		}
		glDeleteShader(vobj);
	}

	if (fsrc != nullptr) {
		// フラグメントシェーダのシェーダオブジェクトを作成
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, nullptr);
		glCompileShader(fobj);

		// フラグメントシェーダのシェーダオブジェクトをプログラムオブジェクトに組み込む
		if (printShaderInfoLog(fobj, "fragment shader")) {
			glAttachShader(program, fobj);
		}
		glDeleteShader(fobj);
	}
	
	// プログラムオブジェクトをリンク
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragmnet");
	glLinkProgram(program);

	if (printProgramInfoLog(program)) {
		return program;
	}
	
	// プログラムオブジェクトの作成に失敗した場合0を返す
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
