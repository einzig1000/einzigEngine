#pragma once
#include "definition/definition.h"
#include "Item/Item.h"

struct CraftRecipe
{
    // 3×3 のレシピ（None は空）
    std::vector<std::array<std::array<ItemID, 3>, 3>> pattern;

    // 結果
    ItemID resultID;
    int resultCount;

    Vector2int min;
    Vector2int max;
};


class CraftRecipeList
{
public:
	static void InitializeRecipes();

    static std::vector<CraftRecipe>& GetRecipeList() { return recipeList_; }

private:
	static std::vector<CraftRecipe> recipeList_;

};

