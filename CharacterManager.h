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

	// パワー順にソート
	void Sort_Power();

	// csvから読み取る
	void LoadGetAllCharactor();


	// 全保有キャラのゲッター
	const std::vector<Charactor*>& GetAllCharactor() const { return AllCharactor; }

	// バトル場面に出てるキャラのゲッター
	const std::vector<Charactor*>& GetButtleCharactor() const { return ButtleCharactor; }

private:
	// 所持キャラの全て
	std::vector<Charactor*> AllCharactor;
	// バトル場面に出てるキャラ
	std::vector<Charactor*> ButtleCharactor;


	// Loadしたデータ置き場
	std::string buf;
};

