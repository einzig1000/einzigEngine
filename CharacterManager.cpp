#include "CharacterManager.h"
#include <algorithm>

CharacterManager::CharacterManager()
{
	Charactor* newChar = new Charactor(TEXTURE::Charactor_King);
	ButtleCharactor.push_back(newChar);
	AllCharactor.push_back(newChar);

	Charactor* newChar1 = new Charactor(TEXTURE::Charactor_Queen);
	AllCharactor.push_back(newChar1);
	Charactor* newChar2 = new Charactor(TEXTURE::Charactor_Bishop);
	AllCharactor.push_back(newChar2);
	Charactor* newChar3 = new Charactor(TEXTURE::Charactor_Knight);
	AllCharactor.push_back(newChar3);
	Charactor* newChar4 = new Charactor(TEXTURE::Charactor_Rook);
	AllCharactor.push_back(newChar4);
	Charactor* newChar5 = new Charactor(TEXTURE::Charactor_Pawn);
	AllCharactor.push_back(newChar5);
	Charactor* newChar6 = new Charactor(TEXTURE::Charactor_King);
	AllCharactor.push_back(newChar6);
	Charactor* newChar7 = new Charactor(TEXTURE::Charactor_Queen);
	AllCharactor.push_back(newChar7);

	Charactor* newChar8 = new Charactor(TEXTURE::Charactor_Bishop);
	AllCharactor.push_back(newChar8);
	Charactor* newChar9 = new Charactor(TEXTURE::Charactor_Knight);
	AllCharactor.push_back(newChar9);
	Charactor* newChar11 = new Charactor(TEXTURE::Charactor_Rook);
	AllCharactor.push_back(newChar11);
	Charactor* newChar12 = new Charactor(TEXTURE::Charactor_Pawn);
	AllCharactor.push_back(newChar12);
	Charactor* newChar13 = new Charactor(TEXTURE::Charactor_King);
	AllCharactor.push_back(newChar13);
	Charactor* newChar14 = new Charactor(TEXTURE::Charactor_Queen);
	AllCharactor.push_back(newChar14);
}

void CharacterManager::AddCharactor(Charactor* add)
{
	AllCharactor.push_back(add);
}

void CharacterManager::Sort_Power()
{
	std::sort(AllCharactor.begin(), AllCharactor.end(), [](Charactor* a, Charactor* b)
		{
			return a->power_ > b->power_;
		});
}
