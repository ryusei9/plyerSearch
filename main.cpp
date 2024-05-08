#include <Novice.h>
#include <MATH.h>

const char kWindowTitle[] = "LC1C_17_サトウ_リュウセイ_タイトル";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

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
		0XFFFFFFFF
	};

	// 速度
	float speed = 10.0f;

	// 敵が向いている方向を変えるタイマー
	int directionChengeTimer = 400;

	// 視覚
	float searchLight = 200.0f;

	Vector2 distance1;

	Vector2 distance2;

	float t = 0.0f;
	// ドットの初期化
	float dot = 0.0f;

	float length = 0.0f;

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

		// 内積を求める
		
		distance1.X = player.position.X - circle.position.X;
		distance1.Y = player.position.Y - circle.position.Y;

		distance2.X = (circle.position.X + circle.direction.X * searchLight) - circle.position.X;
		distance2.Y = (circle.position.Y - circle.direction.Y * searchLight) - circle.position.Y;

		length = sqrtf(distance2.X * distance2.X + distance2.Y * distance2.Y);

		if (length != 0.0f) {
			distance2.X = distance2.X / length;
			distance2.Y = distance2.Y / length;
		}

		dot = distance1.X * distance2.X + distance1.Y * distance2.Y;

		distance1.X = (circle.position.X + circle.direction.X * searchLight) - circle.position.X;
		distance1.Y = (circle.position.Y - circle.direction.Y * searchLight) - circle.position.Y;

		length = sqrtf(distance1.X * distance1.X + distance1.Y * distance1.Y);

		t = dot / length;

		if (t < 0.0f) {
			t = 0.0f;
		} else if (t > 1.0f) {
			t = 1.0f;
		}

		distance1.X = (1.0f - t) * circle.position.X + t * (circle.position.X + circle.direction.X * searchLight);
		distance1.Y = (1.0f - t) * circle.position.Y + t * (circle.position.Y - circle.direction.Y * searchLight);

		distance1.X = player.position.X - distance1.X;
		distance1.Y = player.position.Y - distance1.Y;

		// 当たり判定
		length = sqrtf(distance1.X * distance1.X + distance1.Y * distance1.Y);

		// 色を変える
		if (length <= circle.radius + player.radius) {
			circle.color = 0xFF0000FF;
		} else {
			circle.color = 0xFFFFFFFF;
		}

		// 一定時間ごとに向きが変わる
		directionChengeTimer--;
		if (directionChengeTimer < 400 && directionChengeTimer >= 300) {
			// 右向き
			circle.direction.X = 1;
			circle.direction.Y = 0;
		}else if (directionChengeTimer < 300 && directionChengeTimer >= 200) {
			// 下向き
			circle.direction.X = 0;
			circle.direction.Y = 1;
		} else if (directionChengeTimer < 200 && directionChengeTimer >= 100) {
			// 左向き
			circle.direction.X = -1;
			circle.direction.Y = 0;
		} else if (directionChengeTimer < 100 && directionChengeTimer >= 0) {
			// 上向き
			circle.direction.X = 0;
			circle.direction.Y = -1;
		} else if (directionChengeTimer < 0) {
			directionChengeTimer = 400;
		}

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

		// 敵の視覚
		Novice::DrawLine(
			static_cast<int>(circle.position.X), static_cast<int>((circle.position.Y - WCS.Y) * -1),
			static_cast<int>(circle.position.X + circle.direction.X * searchLight), static_cast<int>(((circle.position.Y - WCS.Y) * -1) + circle.direction.Y * searchLight),
			WHITE);
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
