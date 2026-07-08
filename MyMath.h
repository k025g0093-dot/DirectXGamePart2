#pragma once

#include "KamataEngine.h"
#include <cmath>

static KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

static KamataEngine::Vector3 operator-(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

static KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float scalar) {
	return {v.x * scalar, v.y * scalar, v.z * scalar};
}

static KamataEngine::Vector3 operator*(float scalar, const KamataEngine::Vector3& v) {
	return v * scalar;
}

static KamataEngine::Vector3& operator+=(KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

static KamataEngine::Vector3& operator-=(KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

static KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix) {
	KamataEngine::Vector3 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}

	return result;
}

static KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	KamataEngine::Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j]; // 🌟通常の数学・列優先の掛け算
			}
		}
	}
	return result;
}

static KamataEngine::Matrix4x4 MakeScale(const KamataEngine::Vector3& scale) {
	KamataEngine::Matrix4x4 result{};
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;
	return result;
}

static KamataEngine::Matrix4x4 MakeRotateX(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);
	KamataEngine::Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = cos;
	result.m[1][2] = -sin;
	result.m[2][1] = sin;
	result.m[2][2] = cos;
	result.m[3][3] = 1.0f;
	return result;
}

static KamataEngine::Matrix4x4 MakeRotateY(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);
	KamataEngine::Matrix4x4 result{};
	result.m[0][0] = cos;
	result.m[0][2] = sin;
	result.m[1][1] = 1.0f;
	result.m[2][0] = -sin;
	result.m[2][2] = cos;
	result.m[3][3] = 1.0f;
	return result;
}

static KamataEngine::Matrix4x4 MakeRotateZ(float angle) {
	float sin = std::sin(angle);
	float cos = std::cos(angle);
	KamataEngine::Matrix4x4 result{};
	result.m[0][0] = cos;
	result.m[0][1] = sin;
	result.m[1][0] = -sin;
	result.m[1][1] = cos;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
}

static KamataEngine::Matrix4x4 MakeTranslate(const KamataEngine::Vector3& translate) {
	KamataEngine::Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;
	return result;
}

static KamataEngine::Matrix4x4 MakeAffineMatrix(
    const KamataEngine::Vector3& scale,
    const KamataEngine::Vector3& rotate,
    const KamataEngine::Vector3& translate) {
	KamataEngine::Matrix4x4 scaleMatrix = MakeScale(scale);
	KamataEngine::Matrix4x4 rotateXMatrix = MakeRotateX(rotate.x);
	KamataEngine::Matrix4x4 rotateYMatrix = MakeRotateY(rotate.y);
	KamataEngine::Matrix4x4 rotateZMatrix = MakeRotateZ(rotate.z);
	KamataEngine::Matrix4x4 translateMatrix = MakeTranslate(translate);
	KamataEngine::Matrix4x4 rotateMatrix = Multiply(Multiply(rotateXMatrix, rotateYMatrix), rotateZMatrix);
	return Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);
}

static KamataEngine::Vector3 TransformNolmar(const KamataEngine::Vector3& v, const KamataEngine::Matrix4x4& m) {

	KamataEngine::Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]
	};
	return result;
}


// 3x3行列式の計算（ヘルパー関数）
static float Det3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
	return m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20);
}

// 4x4行列式の計算
static float Determinant(const KamataEngine::Matrix4x4& m) {
	float det = m.m[0][0] * Det3x3(m.m[1][1], m.m[1][2], m.m[1][3], m.m[2][1], m.m[2][2], m.m[2][3], m.m[3][1], m.m[3][2], m.m[3][3]) -
	            m.m[0][1] * Det3x3(m.m[1][0], m.m[1][2], m.m[1][3], m.m[2][0], m.m[2][2], m.m[2][3], m.m[3][0], m.m[3][2], m.m[3][3]) +
	            m.m[0][2] * Det3x3(m.m[1][0], m.m[1][1], m.m[1][3], m.m[2][0], m.m[2][1], m.m[2][3], m.m[3][0], m.m[3][1], m.m[3][3]) -
	            m.m[0][3] * Det3x3(m.m[1][0], m.m[1][1], m.m[1][2], m.m[2][0], m.m[2][1], m.m[2][2], m.m[3][0], m.m[3][1], m.m[3][2]);
	return det;
}

// 4x4逆行列の計算
static KamataEngine::Matrix4x4 Inverse(const KamataEngine::Matrix4x4& m) {
	float det = Determinant(m);

	// 行列式がほぼ0の場合は単位行列を返す
	if (std::abs(det) < 1e-6f) {
		KamataEngine::Matrix4x4 identity{};
		identity.m[0][0] = identity.m[1][1] = identity.m[2][2] = identity.m[3][3] = 1.0f;
		return identity;
	}

	float invDet = 1.0f / det;
	KamataEngine::Matrix4x4 result{};

	result.m[0][0] = invDet * Det3x3(m.m[1][1], m.m[1][2], m.m[1][3], m.m[2][1], m.m[2][2], m.m[2][3], m.m[3][1], m.m[3][2], m.m[3][3]);
	result.m[0][1] = -invDet * Det3x3(m.m[0][1], m.m[0][2], m.m[0][3], m.m[2][1], m.m[2][2], m.m[2][3], m.m[3][1], m.m[3][2], m.m[3][3]);
	result.m[0][2] = invDet * Det3x3(m.m[0][1], m.m[0][2], m.m[0][3], m.m[1][1], m.m[1][2], m.m[1][3], m.m[3][1], m.m[3][2], m.m[3][3]);
	result.m[0][3] = -invDet * Det3x3(m.m[0][1], m.m[0][2], m.m[0][3], m.m[1][1], m.m[1][2], m.m[1][3], m.m[2][1], m.m[2][2], m.m[2][3]);

	result.m[1][0] = -invDet * Det3x3(m.m[1][0], m.m[1][2], m.m[1][3], m.m[2][0], m.m[2][2], m.m[2][3], m.m[3][0], m.m[3][2], m.m[3][3]);
	result.m[1][1] = invDet * Det3x3(m.m[0][0], m.m[0][2], m.m[0][3], m.m[2][0], m.m[2][2], m.m[2][3], m.m[3][0], m.m[3][2], m.m[3][3]);
	result.m[1][2] = -invDet * Det3x3(m.m[0][0], m.m[0][2], m.m[0][3], m.m[1][0], m.m[1][2], m.m[1][3], m.m[3][0], m.m[3][2], m.m[3][3]);
	result.m[1][3] = invDet * Det3x3(m.m[0][0], m.m[0][2], m.m[0][3], m.m[1][0], m.m[1][2], m.m[1][3], m.m[2][0], m.m[2][2], m.m[2][3]);

	result.m[2][0] = invDet * Det3x3(m.m[1][0], m.m[1][1], m.m[1][3], m.m[2][0], m.m[2][1], m.m[2][3], m.m[3][0], m.m[3][1], m.m[3][3]);
	result.m[2][1] = -invDet * Det3x3(m.m[0][0], m.m[0][1], m.m[0][3], m.m[2][0], m.m[2][1], m.m[2][3], m.m[3][0], m.m[3][1], m.m[3][3]);
	result.m[2][2] = invDet * Det3x3(m.m[0][0], m.m[0][1], m.m[0][3], m.m[1][0], m.m[1][1], m.m[1][3], m.m[3][0], m.m[3][1], m.m[3][3]);
	result.m[2][3] = -invDet * Det3x3(m.m[0][0], m.m[0][1], m.m[0][3], m.m[1][0], m.m[1][1], m.m[1][3], m.m[2][0], m.m[2][1], m.m[2][3]);

	result.m[3][0] = -invDet * Det3x3(m.m[1][0], m.m[1][1], m.m[1][2], m.m[2][0], m.m[2][1], m.m[2][2], m.m[3][0], m.m[3][1], m.m[3][2]);
	result.m[3][1] = invDet * Det3x3(m.m[0][0], m.m[0][1], m.m[0][2], m.m[2][0], m.m[2][1], m.m[2][2], m.m[3][0], m.m[3][1], m.m[3][2]);
	result.m[3][2] = -invDet * Det3x3(m.m[0][0], m.m[0][1], m.m[0][2], m.m[1][0], m.m[1][1], m.m[1][2], m.m[3][0], m.m[3][1], m.m[3][2]);
	result.m[3][3] = invDet * Det3x3(m.m[0][0], m.m[0][1], m.m[0][2], m.m[1][0], m.m[1][1], m.m[1][2], m.m[2][0], m.m[2][1], m.m[2][2]);

	return result;
}