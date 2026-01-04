#include "Item/CraftRecipe/CraftRecipe.h"

std::vector<CraftRecipe> CraftRecipeList::recipeList_;

void CraftRecipeList::InitializeRecipes()
{
    CraftRecipe r;
    r.resultID = ItemID::木材ブロック;
    r.resultCount = 4;

    r.pattern.push_back({ {
        { ItemID::原木ブロック, ItemID::None, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None }
    } });
    recipeList_.push_back(r);


    r.resultID = ItemID::棒;
    r.resultCount = 4;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None }
    } });
    recipeList_.push_back(r);

    r.resultID = ItemID::作業台ブロック;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::None },
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None }
    } });
    recipeList_.push_back(r);


    r.resultID = ItemID::木の剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
    } });
    recipeList_.push_back(r);

    r.resultID = ItemID::石の剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::石ブロック, ItemID::None, ItemID::None },
        { ItemID::石ブロック, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
    } });
    recipeList_.push_back(r);

    r.resultID = ItemID::鉄の剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::None, ItemID::None },
        { ItemID::鉄インゴット, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
        } });
    recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::None, ItemID::None },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
        } });
    recipeList_.push_back(r);


    r.resultID = ItemID::木のツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::木材ブロック },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    recipeList_.push_back(r);

    r.resultID = ItemID::石のツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::石ブロック, ItemID::石ブロック, ItemID::石ブロック },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    recipeList_.push_back(r);

    r.resultID = ItemID::鉄のツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤのツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    recipeList_.push_back(r);


    r.resultID = ItemID::木の斧;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::None },
        { ItemID::木材ブロック, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    r.pattern.push_back({ {
        { ItemID::None, ItemID::木材ブロック, ItemID::木材ブロック },
        { ItemID::None, ItemID::棒, ItemID::木材ブロック },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    recipeList_.push_back(r);

    r.resultID = ItemID::石の斧;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::石ブロック, ItemID::石ブロック, ItemID::None },
        { ItemID::石ブロック, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    r.pattern.push_back({ {
        { ItemID::None, ItemID::石ブロック, ItemID::石ブロック },
        { ItemID::None, ItemID::棒, ItemID::石ブロック },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    recipeList_.push_back(r);

    r.resultID = ItemID::鉄の斧;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::None },
        { ItemID::鉄インゴット, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    r.pattern.push_back({ {
        { ItemID::None, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::None, ItemID::棒, ItemID::鉄インゴット },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの斧; 
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::None },
        { ItemID::ダイヤモンド, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    r.pattern.push_back({ {
        { ItemID::None, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
        { ItemID::None, ItemID::棒, ItemID::ダイヤモンド },
        { ItemID::None, ItemID::棒, ItemID::None }
		} });
    recipeList_.push_back(r);


    r.resultID = ItemID::鉄の頭;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
        { ItemID::None, ItemID::None, ItemID::None }
		} });
	recipeList_.push_back(r);

    r.resultID = ItemID::鉄の胴;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
		{ ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット }
		} });
	recipeList_.push_back(r);
    
    r.resultID = ItemID::鉄の脚;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
		{ ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット }
		} });
	recipeList_.push_back(r);

    r.resultID = ItemID::鉄の靴;
    r.pattern.push_back({ {
        { ItemID::None, ItemID::None, ItemID::None },
		{ ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
		{ ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット }
		} });
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの頭;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
		{ ItemID::None, ItemID::None, ItemID::None }
		} });
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの胴;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド }
		} });
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの脚;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド }
		} });
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの靴;
    r.pattern.push_back({ {
		{ ItemID::None, ItemID::None, ItemID::None },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド }
		} });
	recipeList_.push_back(r);


}
