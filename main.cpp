#include <Novice.h>
#define _USE_MATH_DEFINES
#include <MATH.h>

const char kWindowTitle[] = "LE2B_13_サトウ_リュウセイ_TR1";


// 構造体ベクトルの宣言
struct Vector2 {
	float X;
	float Y;
};

// 構造体ボールの宣言
struct Ball {
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 direction;
	float radius;
	unsigned int color;
};

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
		30.0f,
		0XFFFFFFFF
	};

	// メンバ サークルの宣言
	Ball circle = {
		{800.0f,100.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		30.0f,
		0xFF0000FF
	};
	// 視覚
	Vector2 searchLight[6] = {};

	unsigned int searchLightColor = 0xFFFF0066;
	// 速度
	float speed = 10.0f;

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

	// ワールド座標の宣言
	Vector2 WCS;
	WCS.X = 0.0f;
	WCS.Y = 500.0f;

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
		player.position.X += player.velocity.X * speed;
		player.position.Y += player.velocity.Y * speed;

		// 当たり判定
		Collision(player.position, circle.position, searchLight[0],distance[0]);
		Collision(player.position, circle.position, searchLight[1], distance[1]);
		Collision(player.position, searchLight[0], searchLight[1], distance[2]);
		Collision(player.position, circle.position, searchLight[2], distance[3]);
		Collision(player.position, circle.position, searchLight[3], distance[4]);
		Collision(player.position, searchLight[2], searchLight[3], distance[5]);
		Collision(player.position, circle.position, searchLight[4], distance[6]);
		Collision(player.position, circle.position, searchLight[5], distance[7]);
		
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
			// タイマーが0になると通常状態になる
			if (allertTimer < 0) {
				isAllert = false;
			}
		} else {
			allertTimer = 300;
		}

		circle.direction.X = -1;
		circle.direction.Y = 0;
		searchLight[0].X = circle.position.X + -400.0f;
		searchLight[0].Y = circle.position.Y + -200.0f;
		searchLight[1].X = circle.position.X + -400.0f;
		searchLight[1].Y = circle.position.Y + 200.0f;
		searchLight[2].X = circle.position.X + -350.0f;
		searchLight[2].Y = circle.position.Y + -130.0f;
		searchLight[3].X = circle.position.X + -350.0f;
		searchLight[3].Y = circle.position.Y + 130.0f;
		searchLight[4].X = circle.position.X + -400.0f;
		searchLight[4].Y = circle.position.Y;
		searchLight[5].X = circle.position.X + -350.0f;
		searchLight[5].Y = circle.position.Y;

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		// 自キャラ
		Novice::DrawEllipse(
			static_cast<int>(player.position.X), static_cast<int>((player.position.Y - WCS.Y) * -1),
			static_cast<int>(player.radius), static_cast<int>(player.radius),
			0.0f,
			player.color,
			kFillModeSolid);

		// 敵
		Novice::DrawEllipse(static_cast<int>(circle.position.X), static_cast<int>((circle.position.Y - WCS.Y) * -1),
			static_cast<int>(circle.radius), static_cast<int>(circle.radius),
			0.0f,
			circle.color,
			kFillModeSolid);

		// 大きい視界
		Novice::DrawTriangle(
			static_cast<int>(circle.position.X), static_cast<int>((circle.position.Y - WCS.Y) * -1),
			static_cast<int>(searchLight[0].X), static_cast<int>((searchLight[0].Y - WCS.Y) * -1),
			static_cast<int>(searchLight[1].X), static_cast<int>((searchLight[1].Y - WCS.Y) * -1),
			searchLightColor,
			kFillModeSolid
		);
		// 小さい視界
		Novice::DrawTriangle(
			static_cast<int>(circle.position.X), static_cast<int>((circle.position.Y - WCS.Y) * -1),
			static_cast<int>(searchLight[2].X), static_cast<int>((searchLight[2].Y - WCS.Y) * -1),
			static_cast<int>(searchLight[3].X), static_cast<int>((searchLight[3].Y - WCS.Y) * -1),
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
