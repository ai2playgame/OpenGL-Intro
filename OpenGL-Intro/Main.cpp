#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"
#include "Matrix.hpp"
#include "SolidShapeIndex.hpp"
#include "Vector.hpp"
#include "Uniform.hpp"
#include "Material.hpp"

// ---------------------------------------------------------------- //
//	Prototype declaration
// ---------------------------------------------------------------- //
GLuint createProgram(const char* vsrc, const char* fsrc);
GLboolean printShaderInfoLog(GLuint shader, const char* str);
GLboolean printProgramInfoLog(GLuint program);
bool readShaderSource(const char* name, std::vector<GLchar>& buffer);
GLuint loadProgram(const char* vert, const char* frag);

// ---------------------------------------------------------------- //
//	Global variables
// ---------------------------------------------------------------- //


// 六面体の面を塗りつぶす三角形の頂点のインデックス
constexpr GLuint solidCubeIndex[] =
{
   0,  1,  2,  3,  4,  5, // 左
   6,  7,  8,  9, 10, 11, // 裏
  12, 13, 14, 15, 16, 17, // 下
  18, 19, 20, 21, 22, 23, // 右
  24, 25, 26, 27, 28, 29, // 上
  30, 31, 32, 33, 34, 35  // 前
};

// ---------------------------------------------------------------- //
//	Function definition
// ---------------------------------------------------------------- //
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

	// 背面カリングを有効化
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// デプスバッファを有効化
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// シェーダプログラムオブジェクトを作成
	const GLuint program(loadProgram("point.vert", "point.frag"));

	// uniform変数の場所を取得
	const GLint modelViewLocation(glGetUniformLocation(program, "modelView"));
	const GLint projectionLocation(glGetUniformLocation(program, "projection"));
	const GLint normalMatrixLocation(glGetUniformLocation(program, "normalMatrix"));
	const GLint LposLocation(glGetUniformLocation(program, "Lpos"));
	const GLint LambLocation(glGetUniformLocation(program, "Lamb"));
	const GLint LdiffLocation(glGetUniformLocation(program, "Ldiff"));
	const GLint LspecLocation(glGetUniformLocation(program, "Lspec"));

	// uniform blockの場所を取得する
	const GLint materialLocation(glGetUniformBlockIndex(program, "Material"));

	// uniform blockの場所を0版の結合ポイントに結びつける
	glUniformBlockBinding(program, materialLocation, 0);

	// 球の分割数を設定
	const int slices(16), stacks(8);

	// 頂点属性（頂点座標・法線）を作る
	std::vector<Object::Vertex> solidSphereVertex;

	for (int j = 0; j <= stacks; ++j) {
		const float t(static_cast<float>(j) / static_cast<float>(stacks));
		const float y(cos(3.141593f * t)), r(sin(3.141593f * t));
		for (int i = 0; i <= slices; ++i) {
			const float s(static_cast<float>(i) / static_cast<float>(slices));
			const float z(r * cos(6.283185f * s)), x(r * sin(6.283185f * s));

			const Object::Vertex v = { x, y, z, x, y, z };
			solidSphereVertex.emplace_back(v);
		}
	}

	// 頂点インデックス配列を作成
	std::vector<GLuint> solidSphereIndex;
	for (int j = 0; j < stacks; ++j) {
		const int k((slices + 1) * j);
		for (int i = 0; i < slices; ++i) {
			const GLuint k0(k + i);
			const GLuint k1(k0 + 1);
			const GLuint k2(k1 + slices);
			const GLuint k3(k2 + 1);
			
			// 左下の三角形のインデックス
			solidSphereIndex.push_back(k0);
			solidSphereIndex.push_back(k2);
			solidSphereIndex.push_back(k3);
			// 左上の三角形のインデックス
			solidSphereIndex.push_back(k0);
			solidSphereIndex.push_back(k3);
			solidSphereIndex.push_back(k1);
		}
	}

	// 図形データを作成
	std::unique_ptr<const Shape> shapePtr(new SolidShapeIndex(3,
		static_cast<GLsizei>(solidSphereVertex.size()), solidSphereVertex.data(),
		static_cast<GLsizei>(solidSphereIndex.size()), solidSphereIndex.data()));

	// 光源情報
	static constexpr int Lcount(2);
	static constexpr Vector Lpos[] = { 0.0f, 0.0f, 5.0f, 1.0f, 8.0f, 0.0f, 0.0f, 1.0f };
	static constexpr GLfloat Lamb[] = { 0.2f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f };
	static constexpr GLfloat Ldiff[] = { 1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f };
	static constexpr GLfloat Lspec[] = { 1.0f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f };

	// マテリアル情報
	static constexpr Material color[] =
	{
		{ 0.6f, 0.6f, 0.2f,  0.6f, 0.6f, 0.2f,  0.3f, 0.3f, 0.3f,  30.0f },
		{ 0.1f, 0.1f, 0.5f,  0.1f, 0.1f, 0.5f,  0.4f, 0.4f, 0.4f,  60.0f }
	};

	const Uniform<Material> material[] = { &color[0], &color[1] };

	// タイマーを0に設定
	glfwSetTime(0.0);

	// メインループ
	while (window.shoudClose() == GL_FALSE)
	{
		// ウィンドウを消去
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// シェーダプログラムを使用する
		glUseProgram(program);

		// 透視投影変換行列を求める

		const GLfloat* const size(window.getSize());
		// const GLfloat scale(window.getScaleWorldToDev() * 2.0f);
		// const GLfloat w(size[0] / scale), h(size[1] / scale);
		// 直行投影変換行列
		// const Matrix projection(Matrix::orthogonal(-w, w, -h, h, 1.0f, 10.0f));
		// 透視投影変換行列
		// const Matrix projection(Matrix::frustum(-w, w, -h, h, 1.0f, 10.0f));
	
		// 拡大縮小の変換行列を求める
		// const Matrix scaling(Matrix::scale(scale / size[0], scale / size[1], 1.0f));

		// 平行移動の変換行列を求める
		// const GLfloat* const position(window.getLocation());
		// const Matrix translation(Matrix::translate(position[0], position[1], 0.0f));

		const GLfloat fovy(window.getScaleWorldToDev() * 0.01f);
		const GLfloat aspect(size[0] / size[1]);
		const Matrix projection(Matrix::perspective(fovy, aspect, 1.0f, 10.0f));

		// モデル変換行列を求める
		const GLfloat* const location(window.getLocation());
		const Matrix r(Matrix::rotate(static_cast<GLfloat>(glfwGetTime()), 0.0f, 1.0f, 0.0f));
		const Matrix model(Matrix::translate(location[0], location[1], 0.0f) * r);

		// ビュー変換行列を求める
		const Matrix view(Matrix::lookat(
			3.0f, 4.0f, 5.0f,		// 視点座標
			0.0f, 0.0f, 0.0f,		// 注視点座標
			0.0f, 1.0f, 0.0f		// 上方向のベクトル
		));

		// 法線ベクトル変換行列の格納先
		GLfloat normalMatrix[9];

		// モデルビュー変換行列を求める
		const Matrix modelView(view * model);

		// 法線ベクトル変換行列を求める
		modelView.getNormalMatrix(normalMatrix);

		// uniform変数に変換行列を設定
		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView.data());
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection.data());
		glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE, normalMatrix);
		for (int i = 0; i < Lcount; ++i) {
			glUniform4fv(LposLocation + i, 1, (view * Lpos[i]).data());
		}
		glUniform3fv(LambLocation, Lcount, Lamb);
		glUniform3fv(LdiffLocation, Lcount, Ldiff);
		glUniform3fv(LspecLocation, Lcount, Lspec);
		// ここで描画処理
		material[0].select();
		shapePtr->draw();
		
		// 2つ目のモデルビュー行列を作成
		const Matrix modelView1(modelView * Matrix::translate(0.0f, 0.0f, 2.5f));

		// 2つめの法線ベクトル変換行列を求める
		modelView1.getNormalMatrix(normalMatrix);

		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView1.data());
		glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE, normalMatrix);
		
		// 2つめのShapeの描画
		material[1].select();
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
	glBindAttribLocation(program, 1, "normal");
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
