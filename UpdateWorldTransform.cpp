#include "UpdateWorldTransform.h"
#include "MyMath.h"

using namespace KamataEngine;

void UpdateWorldTransform(
	WorldTransform& worldTransform
) {

	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	worldTransform.TransferMatrix();

}
