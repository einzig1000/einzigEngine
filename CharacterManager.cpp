#include "CharacterManager.h"
#include <algorithm>
#include <fstream>
#include <cassert>

CharacterManager::CharacterManager()
{
	LoadGetAllCharactor();
	LoadGetAllEnemy();
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

void CharacterManager::AddEnemyList(Charactor* add)
{
	AllEnemy.push_back(add);
}


void CharacterManager::LoadGetAllCharactor()
{
	// ファイルをひらく
	std::ifstream file("resources/csv/charactor/Charactor.csv");
	assert(file.is_open());

	// ファイルの内容を丸ごとコピー
	std::ostringstream buffer;
	buffer << file.rdbuf();
	buf = buffer.str();
	file.close();

	// 1行ずつ
	std::string line;
	// ここで毎回新しいstringstreamを作る
	std::istringstream mapStream(buf);
	// キャラID
	int serialNumber = 0;



	while (getline(mapStream, line))
	{
		if (line.empty() || line.find("//") == 0)
		{
			continue;
		}

		std::istringstream line_stream(line);
		std::vector<std::string> tokens;
		std::string token;
		while (std::getline(line_stream, token, ','))
		{
			tokens.push_back(token);
		}

		Charactor* add = CreateCharactorByName(tokens[0]);
		add->states_default_.power = std::stof(tokens[1]);
		add->states_default_.hp_max = std::stof(tokens[2]);
		add->states_default_.hp = std::stof(tokens[2]);
		add->states_default_.speed = std::stof(tokens[3]);
		add->states_default_.movePoint = std::stoi(tokens[4]);
		add->skill.type = ParseSkillType(tokens[5]);
		add->skill.range = std::stoi(tokens[6]);
		add->skill.target = ParseSkillTarget(tokens[7]);
		add->skill.areaShape = ParseSkillAreaShape(tokens[8]);
		add->skill.multiplier = std::stof(tokens[9]);
		add->skill.passWall = std::stoi(tokens[10]);
		add->skill.passHeight = std::stoi(tokens[11]);
		add->skill.buffTargetStates = ParseBuffTarget(tokens[12]);

		if (tokens[13].find("EOF") == 0)
		{
			add->serialNumber_ = serialNumber;
			serialNumber++;
			AddCharactorList(add);
		}
	}
}

void CharacterManager::LoadGetAllEnemy()
{
	// ファイルをひらく
	std::ifstream file("resources/csv/charactor/enemy.csv");
	assert(file.is_open());

	// ファイルの内容を丸ごとコピー
	std::ostringstream buffer;
	buffer << file.rdbuf();
	enemybuf = buffer.str();
	file.close();

	// 1行ずつ
	std::string line;
	// ここで毎回新しいstringstreamを作る
	std::istringstream mapStream(enemybuf);
	// キャラID
	int serialNumber = 0;


	while (getline(mapStream, line))
	{
		if (line.empty() || line.find("//") == 0)
		{
			continue;
		}

		std::istringstream line_stream(line);
		std::vector<std::string> tokens;
		std::string token;
		while (std::getline(line_stream, token, ','))
		{
			tokens.push_back(token);
		}

		Charactor* add = CreateCharactorByName(tokens[0]);
		add->states_default_.power = std::stof(tokens[1]);
		add->states_default_.hp_max = std::stof(tokens[2]);
		add->states_default_.hp = std::stof(tokens[2]);
		add->states_default_.speed = std::stof(tokens[3]);
		add->states_default_.movePoint = std::stoi(tokens[4]);
		add->skill.type = ParseSkillType(tokens[5]);
		add->skill.range = std::stoi(tokens[6]);
		add->skill.target = ParseSkillTarget(tokens[7]);
		add->skill.areaShape = ParseSkillAreaShape(tokens[8]);
		add->skill.multiplier = std::stof(tokens[9]);
		add->skill.passWall = std::stoi(tokens[10]);
		add->skill.passHeight = std::stoi(tokens[11]);
		add->skill.buffTargetStates = ParseBuffTarget(tokens[12]);

		if (tokens[13].find("EOF") == 0)
		{
			add->serialNumber_ = serialNumber;
			add->EnemyOrPlayer_ = false;
			add->data.color = 0xFF1111FF;
			add->targetdata.color = 0xFF1111FF;

			AddEnemyList(add);
		}
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


Charactor* CharacterManager::CreateCharactorByName(const std::string& name)
{
	if (name.find("King") == 0)   return new Charactor(TEXTURE::Charactor_King);
	if (name.find("Queen") == 0)  return new Charactor(TEXTURE::Charactor_Queen);
	if (name.find("Bishop") == 0) return new Charactor(TEXTURE::Charactor_Bishop);
	if (name.find("Knight") == 0) return new Charactor(TEXTURE::Charactor_Knight);
	if (name.find("Rook") == 0)   return new Charactor(TEXTURE::Charactor_Rook);
	if (name.find("Pawn") == 0)   return new Charactor(TEXTURE::Charactor_Pawn);
	return nullptr;
}
SkillType CharacterManager::ParseSkillType(const std::string& s)
{
	if (s.find("攻撃") == 0) return SkillType::攻撃;
	if (s.find("回復") == 0) return SkillType::回復;
	if (s.find("バフ") == 0) return SkillType::バフ;
	return SkillType::攻撃; // デフォルト
}
SkillTarget CharacterManager::ParseSkillTarget(const std::string& s)
{
	if (s.find("単体") == 0) return SkillTarget::単体;
	if (s.find("全体") == 0) return SkillTarget::全体;
	if (s.find("自身") == 0) return SkillTarget::自身;
	return SkillTarget::単体; // デフォルト
}
SkillAreaShape CharacterManager::ParseSkillAreaShape(const std::string& s)
{
	if (s.find("円") == 0)           return SkillAreaShape::円;
	if (s.find("直線") == 0)         return SkillAreaShape::直線;
	if (s.find("十字") == 0)         return SkillAreaShape::十字;
	if (s.find("正方形") == 0)       return SkillAreaShape::正方形;
	if (s.find("前方正方形") == 0)   return SkillAreaShape::前方正方形;
	if (s.find("例外") == 0)         return SkillAreaShape::例外;
	return SkillAreaShape::円; // デフォルト
}
AllStates CharacterManager::ParseBuffTarget(const std::string& s)
{
	if (s.find("power") == 0)       return AllStates::power;
	if (s.find("hp_max") == 0)      return AllStates::hp_max;
	if (s.find("speed") == 0)       return AllStates::speed;
	if (s.find("moveRenge") == 0)   return AllStates::moveRenge;
	if (s.find("例外") == 0)        return AllStates::例外;
	return AllStates::例外; // デフォルト
}