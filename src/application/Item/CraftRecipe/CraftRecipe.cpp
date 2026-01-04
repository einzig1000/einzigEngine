#include "Item/CraftRecipe/CraftRecipe.h"

std::vector<CraftRecipe> CraftRecipeList::recipeList_;

void ComputeBounds(
    const std::array<std::array<ItemID, 3>, 3>& pattern,
    Vector2int& min,
    Vector2int& max)
{
    int minX = 3, minY = 3;
    int maxX = -1, maxY = -1;

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            if (pattern[y][x] != ItemID::None)
            {
                minX = my_min(minX, x);
                minY = my_min(minY, y);
                maxX = my_max(maxX, x);
                maxY = my_max(maxY, y);
            }
        }
    }

    min = { minX, minY };
    max = { maxX, maxY };
}


void CraftRecipeList::InitializeRecipes()
{
	recipeList_.clear();

    CraftRecipe r;
    r.resultID = ItemID::木材ブロック;
    r.resultCount = 4;

    r.pattern.push_back({ {
        { ItemID::原木ブロック, ItemID::None, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None }
    } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);


    r.resultID = ItemID::棒;
    r.resultCount = 4;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None }
    } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::作業台ブロック;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::None },
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::None },
        { ItemID::None, ItemID::None, ItemID::None }
    } });
	ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::木の剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::木材ブロック, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
    } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::石の剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::石ブロック, ItemID::None, ItemID::None },
        { ItemID::石ブロック, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
    } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::鉄の剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::None, ItemID::None },
        { ItemID::鉄インゴット, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの剣;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::None, ItemID::None },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::None },
        { ItemID::棒, ItemID::None, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);


    r.resultID = ItemID::木のツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::木材ブロック },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::石のツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::石ブロック, ItemID::石ブロック, ItemID::石ブロック },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::鉄のツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤのツルハシ;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
        { ItemID::None, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);


    r.resultID = ItemID::木の斧;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::木材ブロック, ItemID::木材ブロック, ItemID::None },
        { ItemID::木材ブロック, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    r.pattern.push_back({ {
        { ItemID::None, ItemID::木材ブロック, ItemID::木材ブロック },
        { ItemID::None, ItemID::棒, ItemID::木材ブロック },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::石の斧;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::石ブロック, ItemID::石ブロック, ItemID::None },
        { ItemID::石ブロック, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    r.pattern.push_back({ {
        { ItemID::None, ItemID::石ブロック, ItemID::石ブロック },
        { ItemID::None, ItemID::棒, ItemID::石ブロック },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::鉄の斧;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::None },
        { ItemID::鉄インゴット, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    r.pattern.push_back({ {
        { ItemID::None, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::None, ItemID::棒, ItemID::鉄インゴット },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの斧; 
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::None },
        { ItemID::ダイヤモンド, ItemID::棒, ItemID::None },
        { ItemID::None, ItemID::棒, ItemID::None }
        } });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    r.pattern.push_back({ {
        { ItemID::None, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
        { ItemID::None, ItemID::棒, ItemID::ダイヤモンド },
        { ItemID::None, ItemID::棒, ItemID::None }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
    recipeList_.push_back(r);


    r.resultID = ItemID::鉄の頭;
    r.resultCount = 1;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
        { ItemID::None, ItemID::None, ItemID::None }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);

    r.resultID = ItemID::鉄の胴;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
		{ ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);
    
    r.resultID = ItemID::鉄の脚;
    r.pattern.push_back({ {
        { ItemID::鉄インゴット, ItemID::鉄インゴット, ItemID::鉄インゴット },
        { ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
		{ ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);

    r.resultID = ItemID::鉄の靴;
    r.pattern.push_back({ {
        { ItemID::None, ItemID::None, ItemID::None },
		{ ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット },
		{ ItemID::鉄インゴット, ItemID::None, ItemID::鉄インゴット }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの頭;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
		{ ItemID::None, ItemID::None, ItemID::None }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの胴;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの脚;
    r.pattern.push_back({ {
        { ItemID::ダイヤモンド, ItemID::ダイヤモンド, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
		{ ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);

    r.resultID = ItemID::ダイヤの靴;
    r.pattern.push_back({ {
		{ ItemID::None, ItemID::None, ItemID::None },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド },
        { ItemID::ダイヤモンド, ItemID::None, ItemID::ダイヤモンド }
		} });
    ComputeBounds(r.pattern.back(), r.min, r.max);
	recipeList_.push_back(r);
}
