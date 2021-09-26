#include "common.h"

namespace skrobot {

    int GetCardValue(int card) 
    {
        if(card == kTrumpHead || card == kTrumpVice)
        {
            return card;
        }
        return (card-1)/4;
    }

    /*根据牌面上的值，获取卡的编号*/
    int getCardIndexFromWord(int card)
    {
        if(card<=2)
        {
            return (card + 11)*4;
        }
        else
        {
            return (card-2)*4;
        }
    }

    int getCardValueFromWord(int card)
    {
        GetCardValue(getCardIndexFromWord(card));
    }

    int GetCardColor(int card) 
    {
        return card % 4;
    }

    int GetCardLogicValue (int card)
    {
        GetCardValue(card);
    }

    void SortByValue(int *hand, int len)
    {
        std::sort(hand, hand+len, [](int a, int b) {return GetCardValue(a) <= GetCardValue(b);});
    }

    void SortByLogicValue(int *hand, int len, bool isAscending)
    {
        std::sort(hand, hand+len, 
                [isAscending](int a, int b) {
                    if (isAscending)
                        return GetCardLogicValue(a) <= GetCardLogicValue(b);
                    else 
                        return GetCardLogicValue(a) >= GetCardLogicValue(b);
                });
    }

    int NumTrump(int *hand, int len)
    {
        return std::count_if(hand, hand+len, [](int card) {return card == 0x4E || card == 0x4F;});
    }

    void NumTrump(int *hand, int len, TrumpDescriptor *trump_desc)
    {
        int vice_trump_num = std::count(hand, hand+len, 0x4E);
        int trump_num = std::count(hand, hand+len, 0x4F);
        int total_num = vice_trump_num + trump_num;

        trump_desc->total_num = total_num;

        if (total_num == 0)
            return;
        trump_desc->vice_trump_num = vice_trump_num;
        trump_desc->trump_num = trump_num;
        return;
    }

    int NumCardByValue(int *hand, int len, int value)
    {
        return std::count_if(hand, hand+len, [value](int card) {return GetCardValue(card) == value;});
    }
    int NumCardByLogic (int *hand, int len, int value)
    {
        return std::count_if(hand, hand+len, [value](int card) {return GetCardValue(card) == value;});
    }
} //namespace skrobot



