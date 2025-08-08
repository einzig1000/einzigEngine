#include "CharacterManager.h"
#include <algorithm>
#include <fstream>
#include <cassert>

CharacterManager::CharacterManager()
{
	LoadGetAllCharactor();
	//Charactor* newChar = new Charactor(TEXTURE::Charactor_King);
	//ButtleCharactor.push_back(newChar);
	//AllCharactor.push_back(newChar);

	//Charactor* newChar1 = new Charactor(TEXTURE::Charactor_Queen);
	//AllCharactor.push_back(newChar1);
	//Charactor* newChar2 = new Charactor(TEXTURE::Charactor_Bishop);
	//AllCharactor.push_back(newChar2);
	//Charactor* newChar3 = new Charactor(TEXTURE::Charactor_Knight);
	//AllCharactor.push_back(newChar3);
	//Charactor* newChar4 = new Charactor(TEXTURE::Charactor_Rook);
	//AllCharactor.push_back(newChar4);
	//Charactor* newChar5 = new Charactor(TEXTURE::Charactor_Pawn);
	//AllCharactor.push_back(newChar5);
	//Charactor* newChar6 = new Charactor(TEXTURE::Charactor_King);
	//AllCharactor.push_back(newChar6);
	//Charactor* newChar7 = new Charactor(TEXTURE::Charactor_Queen);
	//AllCharactor.push_back(newChar7);

	//Charactor* newChar8 = new Charactor(TEXTURE::Charactor_Bishop);
	//AllCharactor.push_back(newChar8);
	//Charactor* newChar9 = new Charactor(TEXTURE::Charactor_Knight);
	//AllCharactor.push_back(newChar9);
	//Charactor* newChar11 = new Charactor(TEXTURE::Charactor_Rook);
	//AllCharactor.push_back(newChar11);
	//Charactor* newChar12 = new Charactor(TEXTURE::Charactor_Pawn);
	//AllCharactor.push_back(newChar12);
	//Charactor* newChar13 = new Charactor(TEXTURE::Charactor_King);
	//AllCharactor.push_back(newChar13);
	//Charactor* newChar14 = new Charactor(TEXTURE::Charactor_Queen);
	//AllCharactor.push_back(newChar14);
}

void CharacterManager::AddCharactorList(Charactor* add)
{
	AllCharactor.push_back(add);
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
				add->power_ = std::stoi(word);
			}
			// Hpの設定
			else if (i == 2)
			{
				add->power_ = std::stoi(word);
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

void CharacterManager::Sort_Power()
{
	std::sort(AllCharactor.begin(), AllCharactor.end(), [](Charactor* a, Charactor* b)
		{
			return a->power_ > b->power_;
		});
}
