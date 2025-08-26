#pragma once
#include "Charactor.h"
#include <vector>


class CharacterManager
{
public:
	CharacterManager();

	// ガチャとかで獲得したキャラの追加
	void AddCharactorList(Charactor* add);

	// バトル前のキャラ配置でButtleCharactorリストに追加
	void AddButtleCharactorList(Charactor* add);

	// バトル前のキャラ配置でButtleCharactorリストから除外
	void SubButtleCharactorList(Charactor* add);

	// パワー順にソート
	void Sort_All_Power();
	// アクションディレイ順にソート
	void Sort_Buttle_ActionDelay();

	// csvから読み取る
	void LoadGetAllCharactor();

	// キャラクターステータスのデフォルト値を可変変数に適用
	void SetStatus();


	// 全保有キャラのゲッター
	const std::vector<Charactor*>& GetAllCharactor() const { return AllCharactor; }

	// 全エネミーのゲッター
	const std::vector<Charactor*>& GetAllEnemy() const { return AllEnemy; }

	// バトル場面に出てるキャラのゲッター
	const std::vector<Charactor*>& GetButtleCharactor() const { return ButtleCharactor; }


private:
	// 所持キャラの全て
	std::vector<Charactor*> AllCharactor;
	// バトル場面に出てるキャラ
	std::vector<Charactor*> ButtleCharactor;
	// 敵キャラの全て
	std::vector<Charactor*> AllEnemy;


	// Loadしたデータ置き場
	std::string buf;
};

