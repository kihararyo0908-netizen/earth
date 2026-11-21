#include "Earth.h"
#include "DxLib.h"
#include <cmath>

// コンストラクタ
// 地球の位置・大きさ・HPを初期値に設定する
Earth::Earth() : m_pos{ 0.0, 0.0 }, m_size(0.0), m_hp(5) {

}

// 初期化
// 地球の位置・大きさ・HPを設定し、画像を読み込む
void Earth::Initialize(Vector2 pos, double size, int hp) {
	// 引数で受け取った地球の情報を保存する
	m_pos = pos;
	m_size = size;
	m_hp = hp;

	// 地球の表面に貼り付ける画像を読み込む
	// 読み込みに成功すると画像ハンドルが返される
	m_earthTex = LoadGraph("world_map.jpg");

	// 画像の読み込みに失敗した場合はメッセージを表示
	if (m_earthTex == -1) {
		printfDx("地球の画像を読み込めませんでした\n");
	}

	// 地球の回転角度を初期状態に戻す
	m_rotation = 0.0f;
}

// 更新処理
// 毎フレーム地球を回転させて描画する
void Earth::Update() {
	// 回転角度を少しずつ減らして地球を回転させる
	m_rotation -= 0.01f;

	// 3D描画で奥行きの判定を有効にする
	SetUseZBuffer3D(TRUE);
	// 描画した物体の奥行き情報をZバッファに書き込む
	SetWriteZBuffer3D(TRUE);

	// 地球をテクスチャ付きの3D球体として描画する
	// m_pos     ：地球の中心座標
	// m_size    ：地球の半径
	// 32        ：球体の分割数
	// m_earthTex：地球の画像ハンドル
	// m_rotation：現在の回転角度
	DrawTexturedSphere(
		VGet(static_cast<float>(m_pos.x), static_cast<float>(m_pos.y), 0.0f),
		static_cast<float>(m_size), 32, m_earthTex, m_rotation
	);
}

// 地球の座標を取得
Vector2 Earth::GetPos() const {
	return m_pos;
}

// 地球の大きさを取得
double Earth::GetSize() const {
	return m_size;
}

// 地球の現在のHPを取得する
int Earth::GetHp() const {
	return m_hp;
}

// 被弾処理
// 隕石が地球に衝突した際にHPを減らす
void Earth::OnHit(int damage) {
	// ダメージが0以下の場合は処理しない
	if (damage <= 0) {
		return;
	}

	// 指定されたダメージ分だけHPを減らす
	m_hp -= damage;

	// HPがマイナスにならないように0で止める
	if (m_hp < 0) {
		m_hp = 0;
	}
}

// テクスチャ付き3D球体の描画処理
// 球体を小さな四角形に分割して描画する
void Earth::DrawTexturedSphere(
	VECTOR center,
	float r,
	int divNum,
	int grHandle,
	float rotY
) {
	// 四角形1枚分の頂点データを用意する
	VERTEX3D vertex[4];
	// 四角形を2つの三角形に分割するための頂点番号
	unsigned short index[6] = { 0, 1, 2, 2, 1, 3 };

	// マテリアル（材質）の設定
	// 光の反射や明るさを設定して立体感を出す
	MATERIALPARAM material;
	// 拡散反射光：光が当たった部分の基本的な明るさ
	material.Diffuse = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);
	// 鏡面反射光：表面にできる光の反射
	material.Specular = GetColorF(0.2f, 0.2f, 0.2f, 1.0f);
	// 環境光：直接光が当たらない部分の明るさ
	material.Ambient = GetColorF(0.3f, 0.3f, 0.3f, 1.0f);
	// 自己発光：地球自体は発光しない設定
	material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	// 光沢の強さを設定する
	material.Power = 10.0f;

	// 設定した材質を3D描画に反映する
	SetMaterialParam(material);

	
	// 球体の頂点データを生成する

	 // 縦方向（北極から南極方向）に分割する
	for (int i = 0; i < divNum; i++) {
		// 縦方向の分割位置を0～1の範囲で計算
		float v1 = (float)i / divNum;
		float v2 = (float)(i + 1) / divNum;

		// 縦方向の角度を計算する
		float theta1 = DX_PI_F * v1;
		float theta2 = DX_PI_F * v2;

		// 横方向（地球の周囲）に分割する
		for (int j = 0; j < divNum * 2; j++) {
			
			// 横方向の分割位置を0～1の範囲で計算
			float u1 = (float)j / (divNum * 2);
			float u2 = (float)(j + 1) / (divNum * 2);

			// 横方向の角度に回転角度を加える
			float phi1 = DX_TWO_PI_F * u1 + rotY;
			float phi2 = DX_TWO_PI_F * u2 + rotY;

			// 四角形を構成する4頂点の角度を設定
			float p[4][2] = {
				{ theta1, phi1 },
				{ theta1, phi2 },
				{ theta2, phi1 },
				{ theta2, phi2 }
			};

			// 各頂点に対応する画像のUV座標を設定
			// Uは画像の横方向、Vは画像の縦方向
			float uv[4][2] = {
				{ u1, v1 },
				{ u2, v1 },
				{ u1, v2 },
				{ u2, v2 }
			};

			// 四角形の4頂点を計算する
			for (int k = 0; k < 4; k++) {

				// 球面上の方向ベクトルを計算する
			   // 三角関数を使って球体の形を作る
				float nx = sinf(p[k][0]) * cosf(p[k][1]);
				float ny = cosf(p[k][0]);
				float nz = sinf(p[k][0]) * sinf(p[k][1]);

				// 球体の中心座標と半径から頂点位置を計算
				vertex[k].pos = VAdd(center, VGet(nx * r, ny * r, nz * r));
				// 頂点の法線ベクトルを設定
				// 光の当たり方を計算するために使用する
				vertex[k].norm = VGet(nx, ny, nz);
				// 頂点の拡散反射色を白色に設定
				vertex[k].dif = GetColorU8(255, 255, 255, 255);
				// 頂点の鏡面反射色を設定
				vertex[k].spc = GetColorU8(0, 0, 0, 0);
				// 頂点に対応する画像の横方向の座標
				vertex[k].u = uv[k][0];
				// 頂点に対応する画像の縦方向の座標
				vertex[k].v = uv[k][1];
			}
			// 作成した四角形を描画する

			 // 4頂点を使って2つの三角形を描画する
			// grHandleに指定された地球画像を貼り付ける
			DrawPolygonIndexed3D(vertex, 4, index, 2, grHandle, TRUE);
		}
	}
}

// 地球の破壊判定
// HPが0以下になった場合はtrueを返す
bool Earth::IsDestroyed() const {
	return m_hp <= 0;
}