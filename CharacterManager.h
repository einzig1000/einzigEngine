#pragma once
#include "Charactor.h"
#include <vector>


class CharacterManager
{
public:
	CharacterManager();

	void AddCharactor(Charactor* add);


	void Sort_Power();


	// 全保有キャラのゲッター
	const std::vector<Charactor*>& GetAllCharactor() const { return AllCharactor; }

	// バトル場面に出てるキャラのゲッター
	const std::vector<Charactor*>& GetButtleCharactor() const { return ButtleCharactor; }

private:
	// 所持キャラの全て
	std::vector<Charactor*> AllCharactor;
	// バトル場面に出てるキャラ
	std::vector<Charactor*> ButtleCharactor;



};

