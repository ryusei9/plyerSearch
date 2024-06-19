#include <Novice.h>
#define _USE_MATH_DEFINES
#include <MATH.h>
#include <cassert>
#include <ImGuiManager.h>

const char kWindowTitle[] = "LE2B_13_サトウ_リュウセイ_TR1";


// 構造体ベクトルの宣言
struct Vector2 {
	float X;
	float Y;
};

struct Matrix3x3 {
	float m[3][3];
};

// 構造体ボールの宣言
struct Ball {
	Vector2 translation;
	Vector2 rotation;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 direction;
	float radius;
	unsigned int color;
};

// 構造体ライトの宣言
struct light {
	Vector2 scale;	
	Vector2 rotate;
	Vector2 translation;
	Vector2 direction;
	Matrix3x3 matWorld;
};

// 内積の関数
void Collision(Vector2 pos1, Vector2 pos2, Vector2 pos3, float& length);

// 3x3の逆行列
Matrix3x3 Inverse(Matrix3x3 matrix);

Vector2 Transform(Vector2 vector, Matrix3x3 matrix);

Vector2 TransformNormal(const Vector2 v, const Matrix3x3 m);

// 回転行列の作成関数
Matrix3x3 MakeRotateMatrix(float theta);

Vector2 Add(const Vector2 v1, const Vector2 v2);

Vector2 Subtract(const Vector2 v1, const Vector2 v2);

Matrix3x3 Multiply(Matrix3x3 matrix1, Matrix3x3 matrix2);

Matrix3x3 MakeAffineMatrix(Vector2 scale, Vector2 rotate, Vector2 tlanslate);

Vector2 homing(Vector2 targetPos, Vector2 movePos, Vector2 velocity, float moveSpeed);

Vector2 lightRotate(Vector2 targetPos, Vector2 movePos, Vector2 lightPos);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);


	// メンバ プレイヤーの宣言
	Ball player = {
		{300.0f,100.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		30.0f,
		0XFFFFFFFF
	};

	// メンバ サークルの宣言
	Ball circle = {
		{800.0f,100.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		30.0f,
		0xFF0000FF
	};
	// 視覚
	light searchLight[6] = {};

	for (int i = 0; i < 6; i++) {
		searchLight[i].scale = { 1.0f,1.0f };
	}

	unsigned int searchLightColor = 0xFFFF0066;

	//light searchLight[6] = {};

	/*for (int i = 0; i < 6; i++) {
		searchLight[i].scale = { 1.0f,1.0f };
	}*/

	
	// 速度
	float speed = 10.0f;

	// 移動速度
	const float enemyMoveSpeed = 4.0f;

	const float enemyFoundMoveSpeed = 2.0f;


	// ベクトルの長さ
	float length = 0.0f;

	// 距離
	float distance[8] = {};

	// 警戒のフラグ
	bool isFound = false;

	// 追跡のフラグ
	bool isAllert = false;

	// 警戒から追跡に変わるまでのタイマー
	int foundTimer = 120;

	// 追跡から通常に変わるまでのタイマー
	int allertTimer = 300;

	// 回転のフラグ
	bool isRotate = false;

	// 視覚の初期化
	bool isLightInitialize = false;

	// 回転行列
	Matrix3x3 rotateMatrix = {};

	// ワールド座標の宣言
	Vector2 WCS;
	WCS.X = 0.0f;
	WCS.Y = 500.0f;

	searchLight[0].translation.X = circle.translation.X + -200.0f;
	searchLight[0].translation.Y = circle.translation.Y + -100.0f;
	searchLight[1].translation.X = circle.translation.X + -200.0f;
	searchLight[1].translation.Y = circle.translation.Y + 100.0f;
	searchLight[2].translation.X = circle.translation.X + -175.0f;
	searchLight[2].translation.Y = circle.translation.Y + -65.0f;
	searchLight[3].translation.X = circle.translation.X + -175.0f;
	searchLight[3].translation.Y = circle.translation.Y + 65.0f;
	searchLight[4].translation.X = circle.translation.X + -200.0f;
	searchLight[4].translation.Y = circle.translation.Y;
	searchLight[5].translation.X = circle.translation.X + -175.0f;
	searchLight[5].translation.Y = circle.translation.Y;

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		ImGui::InputFloat2("translation", &searchLight[0].translation.X);
		//ImGui::InputFloat2("screenTranslation", &searchLight[0].translation.X);
		ImGui::Checkbox("lightInitialize", &isLightInitialize);
		ImGui::InputInt("foundTimer", &foundTimer);
		ImGui::InputInt("allertTimer", &allertTimer);
		ImGui::InputFloat2("enemy.translation", &circle.translation.X);
		ImGui::Checkbox("searchLightRotate", &isRotate);
		ImGui::Checkbox("isFound", &isAllert);
		// 初期化
		player.velocity.X = 0.0f;
		player.velocity.Y = 0.0f;
		// ベクトルによる移動
		player.velocity.Y += Novice::CheckHitKey(DIK_W);
		player.velocity.X -= Novice::CheckHitKey(DIK_A);
		player.velocity.Y -= Novice::CheckHitKey(DIK_S);
		player.velocity.X += Novice::CheckHitKey(DIK_D);
		// ベクトルの長さ
		length = sqrtf(player.velocity.X * player.velocity.X + player.velocity.Y * player.velocity.Y);
		// 正規化
		if (length != 0.0f) {
			player.velocity.X = player.velocity.X / length;
			player.velocity.Y = player.velocity.Y / length;
		}
		// 移動の処理
		player.translation.X += player.velocity.X * speed;
		player.translation.Y += player.velocity.Y * speed;

		circle.translation.X += circle.velocity.X;
		circle.translation.Y += circle.velocity.Y;

		// 当たり判定
		Collision(player.translation, circle.translation, searchLight[0].translation,distance[0]);
		Collision(player.translation, circle.translation, searchLight[1].translation, distance[1]);
		Collision(player.translation, searchLight[0].translation, searchLight[1].translation, distance[2]);
		Collision(player.translation, circle.translation, searchLight[2].translation, distance[3]);
		Collision(player.translation, circle.translation, searchLight[3].translation, distance[4]);
		Collision(player.translation, searchLight[2].translation, searchLight[3].translation, distance[5]);
		Collision(player.translation, circle.translation, searchLight[4].translation, distance[6]);
		Collision(player.translation, circle.translation, searchLight[5].translation, distance[7]);
		
		// 追跡
		if (distance[3] <= 1.0f + player.radius || distance[4] <= 1.0f + player.radius || distance[5] <= 1.0f + player.radius || distance[7] <= circle.radius + player.radius) {
			isAllert = true;
			allertTimer = 300;
		// 警戒
		}else if (distance[0] <= 1.0f + player.radius || distance[1] <= 1.0f + player.radius || distance[2] <= 1.0f + player.radius || distance[6] <= 1.0f + player.radius) {
			isFound = true;
			if (isAllert) {
				allertTimer = 300;
			}
		// 通常
		} else {
			searchLightColor = 0xFFFFFF66;
			isFound = false;
		}
		// 警戒される
		if (isFound) {
			foundTimer--;
			searchLightColor = 0xFFFF0066;
			circle.velocity = homing(player.translation, circle.translation, circle.velocity, enemyFoundMoveSpeed);
			// タイマーが0になると追跡状態になる
			if (foundTimer < 0) {
				isAllert = true;
			}
		} else {
			foundTimer = 120;
		}

		// 追跡される
		if (isAllert) {
			allertTimer--;
			searchLightColor = 0xFF000066;
			circle.velocity = homing(player.translation, circle.translation, circle.velocity,enemyMoveSpeed);
			for (int i = 0; i < 6; i++) {
				// プレイヤーが敵より左にいるとき
				// 視点が左に向く
				if (player.translation.X < circle.translation.X) {
					searchLight[0].translation.X = circle.translation.X + -200.0f;
					searchLight[0].translation.Y = circle.translation.Y + -100.0f;
					searchLight[1].translation.X = circle.translation.X + -200.0f;
					searchLight[1].translation.Y = circle.translation.Y + 100.0f;
					searchLight[2].translation.X = circle.translation.X + -175.0f;
					searchLight[2].translation.Y = circle.translation.Y + -65.0f;
					searchLight[3].translation.X = circle.translation.X + -175.0f;
					searchLight[3].translation.Y = circle.translation.Y + 65.0f;
					searchLight[4].translation.X = circle.translation.X + -200.0f;
					searchLight[4].translation.Y = circle.translation.Y;
					searchLight[5].translation.X = circle.translation.X + -175.0f;
					searchLight[5].translation.Y = circle.translation.Y;
					// プレイヤーが敵より右にいるとき
					// 視点が右に向く
				} else if (player.translation.X > circle.translation.X) {
					searchLight[0].translation.X = circle.translation.X + 200.0f;
					searchLight[0].translation.Y = circle.translation.Y + -100.0f;
					searchLight[1].translation.X = circle.translation.X + 200.0f;
					searchLight[1].translation.Y = circle.translation.Y + 100.0f;
					searchLight[2].translation.X = circle.translation.X + 175.0f;
					searchLight[2].translation.Y = circle.translation.Y + -65.0f;
					searchLight[3].translation.X = circle.translation.X + 175.0f;
					searchLight[3].translation.Y = circle.translation.Y + 65.0f;
					searchLight[4].translation.X = circle.translation.X + 200.0f;
					searchLight[4].translation.Y = circle.translation.Y;
					searchLight[5].translation.X = circle.translation.X + 175.0f;
					searchLight[5].translation.Y = circle.translation.Y;
					// プレイヤーが敵より下にいるとき
					// 視点が下に向く
				} else if (player.translation.Y > circle.translation.Y) {
					searchLight[0].translation.X = circle.translation.Y + 200.0f;
					searchLight[0].translation.Y = circle.translation.X + -100.0f;
					searchLight[1].translation.X = circle.translation.Y + 200.0f;
					searchLight[1].translation.Y = circle.translation.X + 100.0f;
					searchLight[2].translation.X = circle.translation.Y + 175.0f;
					searchLight[2].translation.Y = circle.translation.X + -65.0f;
					searchLight[3].translation.X = circle.translation.Y + 175.0f;
					searchLight[3].translation.Y = circle.translation.X + 65.0f;
					searchLight[4].translation.X = circle.translation.Y + 200.0f;
					searchLight[4].translation.Y = circle.translation.X;
					searchLight[5].translation.X = circle.translation.Y + 175.0f;
					searchLight[5].translation.Y = circle.translation.X;
					// プレイヤーが敵より上にいるとき
					// 視点が上に向く
				} else if (player.translation.Y > circle.translation.Y) {
					searchLight[0].translation.X = circle.translation.Y + -200.0f;
					searchLight[0].translation.Y = circle.translation.X + -100.0f;
					searchLight[1].translation.X = circle.translation.Y + -200.0f;
					searchLight[1].translation.Y = circle.translation.X + 100.0f;
					searchLight[2].translation.X = circle.translation.Y + -175.0f;
					searchLight[2].translation.Y = circle.translation.X + -65.0f;
					searchLight[3].translation.X = circle.translation.Y + -175.0f;
					searchLight[3].translation.Y = circle.translation.X + 65.0f;
					searchLight[4].translation.X = circle.translation.Y + -200.0f;
					searchLight[4].translation.Y = circle.translation.X;
					searchLight[5].translation.X = circle.translation.Y + -175.0f;
					searchLight[5].translation.Y = circle.translation.X;
				}
				//searchLight[i].translation = lightRotate(player.translation, circle.translation, searchLight[i].translation);
			}
			// タイマーが0になると通常状態になる
			if (allertTimer < 0) {
				isAllert = false;
			}
		} else {
			allertTimer = 300;
			circle.velocity = {};
		}

		// 視覚の初期化
		if (isLightInitialize) {
			searchLight[0].translation.X = circle.translation.X + -200.0f;
			searchLight[0].translation.Y = circle.translation.Y + -100.0f;
			searchLight[1].translation.X = circle.translation.X + -200.0f;
			searchLight[1].translation.Y = circle.translation.Y + 100.0f;
			searchLight[2].translation.X = circle.translation.X + -175.0f;
			searchLight[2].translation.Y = circle.translation.Y + -65.0f;
			searchLight[3].translation.X = circle.translation.X + -175.0f;
			searchLight[3].translation.Y = circle.translation.Y + 65.0f;
			searchLight[4].translation.X = circle.translation.X + -200.0f;
			searchLight[4].translation.Y = circle.translation.Y;
			searchLight[5].translation.X = circle.translation.X + -175.0f;
			searchLight[5].translation.Y = circle.translation.Y;
			for (int i = 0; i < 6; i++) {
				searchLight[i].rotate = {};
			}
		}

		for (int i = 0; i < 6; i++) {
			if (isRotate) {
				searchLight[i].rotate.X += 0.02f;
			}
			/*rotateMatrix = MakeRotateMatrix(searchLight[i].rotate.X);
			searchLight[i].translation = TransformNormal(searchLight[i].translation, rotateMatrix);*/
			//searchLight[i].matWorld = MakeAffineMatrix(searchLight[i].scale, searchLight[i].rotate, searchLight[i].translation);
			//searchLight[i].translation = Transform(searchLight[i].translation, searchLight[i].matWorld);
		}

		

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		// 自キャラ
		Novice::DrawEllipse(
			static_cast<int>(player.translation.X), static_cast<int>((player.translation.Y - WCS.Y) * -1),
			static_cast<int>(player.radius), static_cast<int>(player.radius),
			0.0f,
			player.color,
			kFillModeSolid);

		// 敵
		Novice::DrawEllipse(static_cast<int>(circle.translation.X), static_cast<int>((circle.translation.Y - WCS.Y) * -1),
			static_cast<int>(circle.radius), static_cast<int>(circle.radius),
			0.0f,
			circle.color,
			kFillModeSolid);

		// 大きい視界
		Novice::DrawTriangle(
			static_cast<int>(circle.translation.X), static_cast<int>((circle.translation.Y - WCS.Y) * -1),
			static_cast<int>(searchLight[0].translation.X), static_cast<int>((searchLight[0].translation.Y - WCS.Y) * -1),
			static_cast<int>(searchLight[1].translation.X), static_cast<int>((searchLight[1].translation.Y - WCS.Y) * -1),
			searchLightColor,
			kFillModeSolid
		);
		// 小さい視界
		Novice::DrawTriangle(
			static_cast<int>(circle.translation.X), static_cast<int>((circle.translation.Y - WCS.Y) * -1),
			static_cast<int>(searchLight[2].translation.X), static_cast<int>((searchLight[2].translation.Y - WCS.Y) * -1),
			static_cast<int>(searchLight[3].translation.X), static_cast<int>((searchLight[3].translation.Y - WCS.Y) * -1),
			searchLightColor,
			kFillModeSolid
		);
		
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
// 内積の関数
void Collision(Vector2 pos1, Vector2 pos2, Vector2 pos3, float& length) {
	Vector2 e = {};
	Vector2 d = {};

	float t = 0.0f;

	// ドットの初期化

	float dot = 0.0f;


	d.X = pos1.X - pos2.X;

	d.Y = pos1.Y - pos2.Y;


	e.X = pos3.X - pos2.X;

	e.Y = pos3.Y - pos2.Y;

	length = sqrtf(e.X * e.X + e.Y * e.Y);

	if (length != 0.0f) {

		e.X = e.X / length;

		e.Y = e.Y / length;

	}

	dot = d.X * e.X + d.Y * e.Y;

	d.X = pos3.X - pos2.X;

	d.Y = pos3.Y - pos2.Y;

	length = sqrtf(d.X * d.X + d.Y * d.Y);

	t = dot / length;

	if (t < 0.0f) {

		t = 0.0f;

	} else if (t > 1.0f) {

		t = 1.0f;

	}

	d.X = (1.0f - t) * pos2.X + t * pos3.X;

	d.Y = (1.0f - t) * pos2.Y + t * pos3.Y;

	d.X = pos1.X - d.X;

	d.Y = pos1.Y - d.Y;

	length = sqrtf(d.X * d.X + d.Y * d.Y);


}

// 3x3の逆行列
Matrix3x3 Inverse(Matrix3x3 matrix) {
	float A = matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2] + matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0] + matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1] - matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0] - matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2] - matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1];
	Matrix3x3 inverseM2 = {};
	inverseM2.m[0][0] = (matrix.m[1][1] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][1]) / A;
	inverseM2.m[0][1] = (-(matrix.m[0][1] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][1])) / A;
	inverseM2.m[0][2] = (matrix.m[0][1] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][1]) / A;
	inverseM2.m[1][0] = (-(matrix.m[1][0] * matrix.m[2][2] - matrix.m[1][2] * matrix.m[2][0])) / A;
	inverseM2.m[1][1] = (matrix.m[0][0] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[2][0]) / A;
	inverseM2.m[1][2] = (-(matrix.m[0][0] * matrix.m[1][2] - matrix.m[0][2] * matrix.m[1][0])) / A;
	inverseM2.m[2][0] = (matrix.m[1][0] * matrix.m[2][1] - matrix.m[1][1] * matrix.m[2][0]) / A;
	inverseM2.m[2][1] = (-(matrix.m[0][0] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[2][0])) / A;
	inverseM2.m[2][2] = (matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0]) / A;
	return inverseM2;
}

Vector2 Transform(Vector2 vector, Matrix3x3 matrix) {
	Vector2 result = {};
	result.X = vector.X * matrix.m[0][0] + vector.Y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	result.Y = vector.X * matrix.m[0][1] + vector.Y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
	float w = vector.X * matrix.m[0][2] + vector.Y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
	assert(w != 0.0f);
	result.X /= w;
	result.Y /= w;
	return result;
}

// 回転行列の作成関数
Matrix3x3 MakeRotateMatrix(float theta) {
	Matrix3x3 rotateMatrix = {};
	rotateMatrix.m[0][0] = cosf(theta);
	rotateMatrix.m[0][1] = sinf(theta);
	rotateMatrix.m[1][0] = -sinf(theta);
	rotateMatrix.m[1][1] = cosf(theta);
	rotateMatrix.m[2][2] = 1;
	return rotateMatrix;
}

Vector2 Add(const Vector2 v1, const Vector2 v2){
	return Vector2{ v1.X + v2.X,v1.Y + v2.Y };
}

Matrix3x3 Multiply(Matrix3x3 matrix1, Matrix3x3 matrix2) {
	Matrix3x3 resultMultiply = {};
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			resultMultiply.m[i][j] = matrix1.m[i][0] * matrix2.m[0][j] + matrix1.m[i][1] * matrix2.m[1][j] + matrix1.m[i][2] * matrix2.m[2][j];
		}
	}
	return resultMultiply;
}

Matrix3x3 MakeAffineMatrix(Vector2 scale, Vector2 rotate, Vector2 tlanslate) {
	Matrix3x3 result = {};
	result.m[0][0] = scale.X * cosf(rotate.X);
	result.m[0][1] = scale.X * sinf(rotate.X);
	result.m[0][2] = 0;
	result.m[1][0] = scale.Y * -sinf(rotate.Y);
	result.m[1][1] = scale.Y * cosf(rotate.Y);
	result.m[1][2] = 0;
	result.m[2][0] = tlanslate.X;
	result.m[2][1] = tlanslate.Y;
	result.m[2][2] = 1;
	return result;
}

Vector2 TransformNormal(const Vector2 v, const Matrix3x3 m) {
	Vector2 result{
		v.X * m.m[0][0] + v.Y * m.m[1][0] + 1.0f * m.m[2][0],
		v.X * m.m[0][1] + v.Y * m.m[1][1] + 1.0f * m.m[2][1],
	};
	return result;
}

Vector2 Subtract(const Vector2 v1, const Vector2 v2) {
	Vector2 SubtractResult = {};
	SubtractResult.X = v1.X - v2.X;
	SubtractResult.Y = v1.Y - v2.Y;
	return SubtractResult;
}

Vector2 Normalize(const Vector2 v) {
	Vector2 NormalizeResult = {};
	float LengthResult = {};
	LengthResult = sqrtf(v.X * v.X + v.Y * v.Y);
	NormalizeResult.X = v.X / LengthResult;
	NormalizeResult.Y = v.Y / LengthResult;
	return NormalizeResult;
}

Vector2 Multiply(const float scalar, const Vector2 v){
	Vector2 MultiplyResult = {};
	MultiplyResult.X = scalar * v.X;
	MultiplyResult.Y = scalar * v.Y;
	return MultiplyResult;
}

Vector2 Multiply(const Vector2 v1, const Vector2 v2) {
	Vector2 MultiplyResult = {};
	MultiplyResult.X = v1.X * v2.X;
	MultiplyResult.Y = v1.Y * v2.Y;
	return MultiplyResult;
}

Vector2 homing(Vector2 targetPos, Vector2 movePos,Vector2 velocity,float moveSpeed) {
	

	// 自キャラのワールド座標を取得する
	Vector2 playerPos = targetPos;

	// 敵キャラのワールド座標を取得する
	Vector2 enemyPos = movePos;

	// 敵キャラから自キャラへの差分ベクトルを求める
	Vector2 diff = Subtract(playerPos, enemyPos);

	// ベクトルの正規化
	diff = Normalize(diff);

	// ベクトルの長さを、速さに合わせる
	diff = Multiply(moveSpeed, diff);

	velocity = diff;
	return velocity;
}

Vector2 lightRotate(Vector2 targetPos, Vector2 movePos, Vector2 lightPos) {
	// 自キャラのワールド座標を取得する
	Vector2 playerPos = targetPos;

	// 敵キャラのワールド座標を取得する
	Vector2 enemyPos = movePos;

	// 敵キャラから自キャラへの差分ベクトルを求める
	Vector2 diff = Subtract(playerPos, enemyPos);

	// ベクトルの正規化
	diff = Normalize(diff);

	// ベクトルの長さを、速さに合わせる
	lightPos = Multiply(lightPos, diff);

	//lightPos = diff;
	return lightPos;

}