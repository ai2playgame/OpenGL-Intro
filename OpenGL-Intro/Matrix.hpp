#pragma once
#include <algorithm>
#include <cmath>
#include <GL/glew.h>

class Matrix {
private:
	GLfloat _matrix[16];

public:
	Matrix() {
		std::fill(_matrix, _matrix + 16, 0.0f);
	}
	
	Matrix(const GLfloat* rhs) {
		std::copy(rhs, rhs + 16, _matrix);
	}

	const GLfloat* data() const {
		return _matrix;
	}

	// 単位行列を作成
	// | 1 0 0 0 |
	// | 0 1 0 0 |
	// | 0 0 1 0 |
	// | 0 0 0 1 |
	static Matrix identity() {
		Matrix t;
		std::fill(t._matrix, t._matrix + 16, 0.0f);
		t._matrix[0] = t._matrix[5] = t._matrix[10] = t._matrix[15] = 1.0f;

		return t;
	}

	// (x, y, z)平行移動する変換行列を作成
	// | 1 0 0 x |
	// | 0 1 0 y |
	// | 0 0 1 z |
	// | 0 0 0 1 |
	static Matrix translate(GLfloat x, GLfloat y, GLfloat z) {
		Matrix t(identity());

		t._matrix[12] = x;
		t._matrix[13] = y;
		t._matrix[14] = z;

		return t;
	}

	// (x, y, z)倍に拡大縮小する変換行列を作成する
	// | x 0 0 0 |
	// | 0 y 0 0 |
	// | 0 0 z 0 |
	// | 0 0 0 1 |
	static Matrix scale(GLfloat x, GLfloat y, GLfloat z) {
		Matrix t(identity());

		t._matrix[0] = x;
		t._matrix[5] = y;
		t._matrix[10] = z;

		return t;
	}
	
	// (x, y, z)を軸にa回転する変換行列を作成
	static Matrix rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z) {
		Matrix t(identity());
		const GLfloat d(sqrt(x * x + y * y + z * z));

		if (d > 0.0f) {
			// (x, y, z)ベクトルを正規化（長さを1とする）
			const GLfloat l(x / d), m(y / d), n(z / d);

			const GLfloat l2(l * l), m2(m * m), n2(n * n);
			const GLfloat lm(l * m), mn(m * n), nl(n * l);
			const GLfloat c(cos(a)), c1(1.0f - c), s(sin(a));

			t._matrix[0] = (1.0f - l2) * c + l2;
			t._matrix[1] = lm * c1 + n * s;
			t._matrix[2] = nl * c1 - m * s;
			t._matrix[4] = lm * c1 - n * s;
			t._matrix[5] = (1.0f - m2) * c + m2;
			t._matrix[6] = mn * c1 + l * s;
			t._matrix[8] = nl + c1 + m * s;
			t._matrix[9] = mn * c1 - l * s;
			t._matrix[10] = (1.0f - n2) * c + n2;
		}

		return t;
	}

	static Matrix lookat(
		GLfloat ex, GLfloat ey, GLfloat ez, // 視点の座標 e
		GLfloat gx, GLfloat gy, GLfloat gz, // 注視点の座標 g
		GLfloat ux, GLfloat uy, GLfloat uz  // 上方向のベクトル u
		)
	{
		// 視点を(0, 0, 0)に平行移動する変換行列
		const Matrix tv(translate(-ex, -ey, -ez));

		// 視点座標系のz軸（t） = e - g
		const GLfloat tx(ex - gx);
		const GLfloat ty(ey - gy);
		const GLfloat tz(ez - gz);

		// 視点座標系のx軸（r） = u X t軸
		const GLfloat rx(uy * tz - uz * ty);
		const GLfloat ry(uz * tx - ux * tz);
		const GLfloat rz(ux * ty - uy * tx);

		// 視点座標系のy軸（s） = t軸 X r軸
		const GLfloat sx(ty * rz - tz * ry);
		const GLfloat sy(tz * rx - tx * rz);
		const GLfloat sz(tx * ry - ty * rx);

		// s軸の長さが0.0ではないことを確認
		const GLfloat s2(sx * sx + sy * sy + sz * sz);
		// もしs軸の長さが0なら平行移動行列を返す
		if (s2 == 0.0f) { return tv; }

		// (0, 0, 0)に平行移動した視点座標系をxyz座標系に揃える回転行列
		Matrix rv(identity());

		// r軸を正規化して配列変数に格納
		const GLfloat r(sqrt(rx * rx + ry * ry + rz * rz));
		rv._matrix[0] = rx / r;
		rv._matrix[4] = ry / r;
		rv._matrix[8] = rz / r;

		// s軸を正規化して配列変数に格納
		const GLfloat s(sqrt(sx * sx + sy * sy + sz * sz));
		rv._matrix[1] = sx / s;
		rv._matrix[5] = sy / s;
		rv._matrix[9] = sz / s;

		// t軸を正規化して配列変数に格納
		const GLfloat t(sqrt(tx * tx + ty * ty + tz * tz));
		rv._matrix[2] = tx / t;
		rv._matrix[6] = ty / t;
		rv._matrix[10] = tz / t;

		return rv * tv;
	}

	static Matrix orthogonal(GLfloat left, GLfloat right,
							 GLfloat bottom, GLfloat top,
							 GLfloat zNear, GLfloat zFar) {
		Matrix t(identity());
		const GLfloat dx(right - left);
		const GLfloat dy(top - bottom);
		const GLfloat dz(zFar - zNear);

		if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
			t._matrix[0] = 2.0f / dx;
			t._matrix[5] = 2.0f / dy;
			t._matrix[10] = -2.0f / dz;
			t._matrix[12] = -(right + left) / dx;
			t._matrix[13] = -(top + bottom) / dy;
			t._matrix[14] = -(zFar + zNear) / dz;
		}
		
		return t;
	}

	static Matrix frustum(GLfloat left, GLfloat right,
		GLfloat bottom, GLfloat top,
		GLfloat zNear, GLfloat zFar) {
		Matrix t(identity());
		const GLfloat dx(right - left);
		const GLfloat dy(top - bottom);
		const GLfloat dz(zFar - zNear);

		if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
			t._matrix[0] = 2.0f * zNear / dx;
			t._matrix[5] = 2.0f * zNear / dy;
			t._matrix[8] = (right + left) / dx;
			t._matrix[9] = (top + bottom) / dy;
			t._matrix[10] = -(zFar + zNear) / dz;
			t._matrix[11] = -1.0f;
			t._matrix[14] = -2.0f * zFar * zNear / dz;
			t._matrix[15] = 0.0f;
		}

		return t;
	}

	static Matrix perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
		Matrix t(identity());
		const GLfloat dz(zFar - zNear);

		if (dz != 0.0f) {
			t._matrix[5] = 1.0f / tan(fovy * 0.5f);
			t._matrix[0] = t._matrix[5] / aspect;
			t._matrix[10] = -(zFar + zNear) / dz;
			t._matrix[11] = -1.0f;
			t._matrix[14] = -2.0f * zFar * zNear / dz;
			t._matrix[15] = 0.0f;
		}
		return t;
	}

	Matrix operator*(const Matrix& rhs) const {
		Matrix t;
		for (int i = 0; i < 16; ++i) {
			const int j(i & 3), k(i & ~3);

			t._matrix[i] =
				_matrix[0 + j] * rhs._matrix[k + 0] +
				_matrix[4 + j] * rhs._matrix[k + 1] +
				_matrix[8 + j] * rhs._matrix[k + 2] +
				_matrix[12 + j] * rhs._matrix[k + 3];
		}

		return t;
	}
};
