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


	Vector2 e;

	Vector2 d;

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
		
		d.X = player.position.X - circle.position.X;
		d.Y = player.position.Y - circle.position.Y;

		e.X = (circle.position.X + circle.direction.X * searchLight) - circle.position.X;
		e.Y = (circle.position.Y - circle.direction.Y * searchLight) - circle.position.Y;

		length = sqrtf(e.X * e.X + e.Y * e.Y);

		if (length != 0.0f) {
			e.X = e.X / length;
			e.Y = e.Y / length;
		}

		dot = d.X * e.X + d.Y * e.Y;

		d.X = (circle.position.X + circle.direction.X * searchLight) - circle.position.X;
		d.Y = (circle.position.Y - circle.direction.Y * searchLight) - circle.position.Y;

		length = sqrtf(d.X * d.X + d.Y * d.Y);

		t = dot / length;

		if (t < 0.0f) {
			t = 0.0f;
		} else if (t > 1.0f) {
			t = 1.0f;
		}

		d.X = (1.0f - t) * circle.position.X + t * (circle.position.X + circle.direction.X * searchLight);
		d.Y = (1.0f - t) * circle.position.Y + t * (circle.position.Y - circle.direction.Y * searchLight);

		d.X = player.position.X - d.X;
		d.Y = player.position.Y - d.Y;



		length = sqrtf(d.X * d.X + d.Y * d.Y);

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
		/*if (player.velocity.X != 0.0f) {
			player.direction.X = player.velocity.X;
		}

		if (circle.position.X >= player.position.X) {
			circle.direction.X = -1;
		} else {
			circle.direction.X = 1;
		}

		dot = player.direction.X * circle.direction.X + player.direction.Y * circle.direction.Y;*/

		/*if (dot < 0.0f) {
			circle.color = 0x0000FFFF;
		} else {
			circle.color = 0xFFFFFFFF;
		}*/


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		Novice::DrawEllipse(
			static_cast<int>(player.position.X), static_cast<int>((player.position.Y - WCS.Y) * -1),
			static_cast<int>(player.radius), static_cast<int>(player.radius),
			0.0f,
			player.color,
			kFillModeSolid);

		Novice::DrawLine(
			static_cast<int>(player.position.X), static_cast<int>((player.position.Y - WCS.Y) * -1),
			static_cast<int>(player.position.X + player.direction.X * searchLight), static_cast<int>((player.position.Y - WCS.Y) * -1),
			WHITE);

		Novice::DrawEllipse(static_cast<int>(circle.position.X), static_cast<int>((circle.position.Y - WCS.Y) * -1),
			static_cast<int>(circle.radius), static_cast<int>(circle.radius),
			0.0f,
			circle.color,
			kFillModeSolid);

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
