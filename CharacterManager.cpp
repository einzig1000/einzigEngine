#include "CharacterManager.h"
#include <algorithm>
#include <fstream>
#include <cassert>

CharacterManager::CharacterManager()
{
	LoadGetAllCharactor();
}

void CharacterManager::AddCharactorList(Charactor* add)
{
	AllCharactor.push_back(add);
}

void CharacterManager::AddButtleCharactorList(Charactor* add)
{
	ButtleCharactor.push_back(add);
}

void CharacterManager::SubButtleCharactorList(Charactor* add)
{
	auto it = std::find(ButtleCharactor.begin(), ButtleCharactor.end(), add);
	if (it != ButtleCharactor.end())
	{
		ButtleCharactor.erase(it);
	}
}

void CharacterManager::LoadGetAllCharactor()
{
	// ファイルをひらく
	std::ifstream file("resources/csv/charactor/charactor.csv");
	assert(file.is_open());

	// ファイルの内容を丸ごとコピー
	std::ostringstream buffer;
	buffer << file.rdbuf();
	buf = buffer.str();

	// ファイルを閉じる
	file.close();
	// 1行ずつ
	std::string line;
	// ここで毎回新しいstringstreamを作る
	std::istringstream mapStream(buf);


	// ブロックタイプ適用
	int serialNumber = 0;
	while (getline(mapStream, line))
	{
		std::istringstream line_stream(line);
		std::string word;
		int i = 0;

		if (line.find("//") == 0)
		{
			continue;
		}

		Charactor* add{};

		while (getline(line_stream, word, ','))
		{
			// 駒タイプの設定
			if (i == 0)
			{
				if (word.find("Queen") == 0)
				{
					add = new Charactor(TEXTURE::Charactor_Queen);
				}
				else if (word.find("Bishop") == 0)
				{
					add = new Charactor(TEXTURE::Charactor_Bishop);
				}
				else if (word.find("Knight") == 0)
				{
					add = new Charactor(TEXTURE::Charactor_Knight);
				}
				else if (word.find("Rook") == 0)
				{
					add = new Charactor(TEXTURE::Charactor_Rook);
				}
				else if (word.find("Pawn") == 0)
				{
					add = new Charactor(TEXTURE::Charactor_Pawn);
				}
				else
				{
					add = new Charactor(TEXTURE::Charactor_King);
				}
			}
			// 攻撃力の設定
			else if (i == 1)
			{
				add->states_default_.power = std::stof(word);
			}
			// Hpの設定
			else if (i == 2)
			{
				add->states_default_.hp = std::stof(word);
			}
			// 素早さの設定
			else if (i == 3)
			{
				add->states_default_.speed = std::stof(word);
			}
			// 移動距離の設定
			else if (i == 4)
			{
				add->states_default_.movePoint = std::stoi(word);
			}
			// 設定終わり
			else
			{
				add->serialNumber_ = serialNumber;

				AddCharactorList(add);
			}


			i++;
		}
		serialNumber++;
	}
}

void CharacterManager::SetStatus()
{
	for (int i = 0; i < AllCharactor.size(); ++i)
	{
		AllCharactor[i]->states_buttle_ = AllCharactor[i]->states_default_;
	}
}

// ユニットオーバーレイの時だから全キャラ更新
void CharacterManager::Sort_All_Power()
{
	std::sort(AllCharactor.begin(), AllCharactor.end(), [](Charactor* a, Charactor* b)
		{
			return a->states_default_.power > b->states_default_.power;
		});
}

// バトル前更新用だからバトルキャラのみ更新
void CharacterManager::Sort_Buttle_ActionDelay()
{
	std::sort(ButtleCharactor.begin(), ButtleCharactor.end(), [](Charactor* a, Charactor* b)
		{
			return a->actionDelay < b->actionDelay;
		});
}
