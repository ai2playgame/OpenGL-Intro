#include <iostream>
#include <string>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
