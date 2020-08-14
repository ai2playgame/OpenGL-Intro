#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
private:
	// ウィンドウのハンドル
	GLFWwindow* const _window;
	 
	// ウィンドウのサイズ
	GLfloat _size[2];

	// ワールド座標系に対するデバイス座標系の拡大率
	GLfloat _scaleWorldToDev;
	
	GLfloat _location[2];

public:
	Window(int width = 640, int height = 480, const char* title = "Hello OpenGL!")
		: _window(glfwCreateWindow(width, height, title, nullptr, nullptr))
		, _scaleWorldToDev(100.0f)
	{
		_location[0] = _location[1] = 0.0f;

		if (_window == nullptr) {
			std::cerr << "Can't create GLFW window." << std::endl;
			exit(1);
		}

		// 作成したウィンドウをOpenGLの処理対象とする
		glfwMakeContextCurrent(_window);

		// GLEWの初期化
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cerr << "Can't initialize GLEW." << std::endl;
			exit(1);
		}

		// 垂直同期のタイミングを待つ
		glfwSwapInterval(1);

		// ウィンドウのサイズ変更時に呼び出す処理の登録
		glfwSetWindowSizeCallback(_window, resize);

		// このインスタンスのthisポインタを記録しておく
		glfwSetWindowUserPointer(_window, this);

		resize(_window, width, height);
	}

	virtual ~Window() {
		glfwDestroyWindow(_window);
	}

	// ウィンドウを閉じるべきかの判定
	int shoudClose() const {
		return glfwWindowShouldClose(_window);
	}

	// カラーバッファを入れ替えて、イベントも取り出す
	void swapBuffers() {
		glfwSwapBuffers(_window);
		glfwWaitEvents();

		// マウスの左ボタンが押されていればマウスカーソルの位置をlocationに代入する
		if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
			// マウスカーソルの位置を取得
			double mouseX, mouseY;
			glfwGetCursorPos(_window, &mouseX, &mouseY);

			// マウスカーソルの正規化デバイス座標系での位置を計算
			_location[0] = static_cast<GLfloat>(mouseX) * 2.0f / _size[0] - 1.0f;
			_location[1] = 1.0f - static_cast<GLfloat>(mouseY) * 2.0f / _size[1];
		}

	}

	const GLfloat* getSize() const { return _size; }
	const GLfloat* getLocation() const { return _location; }

	GLfloat getScaleWorldToDev() const { return _scaleWorldToDev; }

	// ウィンドウのサイズ変更
	static void resize(GLFWwindow* const window, int width, int height) {
		glViewport(0, 0, width, height);

		Window* const instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));

		if (instance != nullptr) {
			instance->_size[0] = static_cast<GLfloat>(width);
			instance->_size[1] = static_cast<GLfloat>(height);
		}
	}

};
