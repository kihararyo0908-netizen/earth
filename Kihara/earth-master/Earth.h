#pragma once
#include "DxLib.h"

#include "../../Matsunaga/Vector/Vector.h"

// 地球クラス
class Earth {
private:
	Vector2 m_pos; // 位置（地球が存在する場所の管理）
	double m_size; // 大きさ（地球の大きさを管理）
	int m_hp;      // 体力（地球の耐久状況の管理）
	int m_earthTex = -1; // 地球のテクスチャ
	float m_rotation = 0.0f; // 地球の回転角度

public:
	// コンストラクタ
	Earth();

	// 初期化（位置・大きさ、体力の初期化）
	void Initialize(Vector2 pos, double size, int hp = 5);

	// アップデート（毎フレームの処理：地球の描画処理など）
	void Update();

	// ゲッター（地球の座標を取得）
	Vector2 GetPos() const;

	void DrawTexturedSphere(
		VECTOR center,
		float radius,
		int divNum,
		int grHandle,
		float rotY
	);

	// ゲッター（地球の大きさを取得）
	double GetSize() const;

	// ゲッター（現在のHPを取得）
	int GetHp() const;

	// 被弾（隕石に当たった時の処理）
	void OnHit(int damage = 1);

	// 地球が0になったかを判定する
	bool IsDestroyed() const;
};