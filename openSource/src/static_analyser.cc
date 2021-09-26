#include "static_analyser.h"
#include <algorithm>
namespace skrobot {

bool StaticAnalyserC::IsDouble(int *hand, int len) 
{
    if (len != 2)
        return false;

    /*红桃A不是对子，是炸*/
    if(hand[0] == kHeartA)
        return false;
    
    /*两王，是炸*/
    if(hand[0] == kTrumpHead || hand[0] == kTrumpVice)
        return false;

    std::sort(hand, hand+len);

    if(GetCardValue(hand[0]) == GetCardValue(hand[1]))
    {
        FillInDescriptor(kHandTypeDouble,hand[0], 2, 1);
        return true;
    }
    return false;
}

bool StaticAnalyserC::IsTriple(int *hand, int len)
{
    if (len != 3)
        return false;
    /*红桃A不是对子，是炸*/
    if(hand[0] == kHeartA)
        return false;

    std::sort(hand, hand+len);

    if(GetCardValue(hand[0]) == GetCardValue(hand[1]) && GetCardValue(hand[1]) == GetCardValue(hand[2]))
    {
        FillInDescriptor(kHandTypeTriple, hand[0],3,1);
        return true;
    }
    return false;

}

bool StaticAnalyserC::IsShunZi (int *hand, int len)
{
    if (len < 5 || len > kMaxHandLength)
        return false;

    std::sort(hand, hand+len);

    // 2不能出现在对连中
    int twoNum = NumCardByLogic(hand, len, GetCardLogicValue(kHeartTwo));
    if (twoNum != 0)
        return false;

    for(int i = 0; i<len-1 ; i++)
    {
        if(GetCardValue(hand[i+1])-GetCardValue(hand[i])!=1)
            return false;
    }

    FillInDescriptor(kHandTypeShunzi, hand[0], 1, len);
    return true;
}

bool StaticAnalyserC::IsDoubleLink(int *hand, int len)
{
    if (len < 6 || len % 2 != 0 )
        return false;

    std::sort(hand, hand+len);
    
    // 2不能出现在对连中
    int twoNum = NumCardByLogic(hand, len, GetCardLogicValue(kHeartTwo));
    if (twoNum != 0)
        return false;

    //红桃A不能出现
    int ANum = NumCardByLogic(hand, len, GetCardLogicValue(kHeartA));
    if (ANum != 0)
        return false;

    iVector distribution[2];
    this->DistributionByLogicValue(hand, len, distribution);

    int num_of_gap = NumberOfGap(distribution);
    int num_trump_needed = TrumpNeededForXLink(2, distribution);
    if (num_of_gap == 0 && num_trump_needed == 0)
    {
        FillInDescriptor(kHandTypeDoubleLink, hand[0], 2, len/2);
        return true;
    }
    return false;
}

bool StaticAnalyserC::IsTripleLink(int *hand, int len)
{
    if (len < 9 || len % 3 != 0)
        return false;

    std::sort(hand, hand+len);

    // 2不能出现在三连中
    int twoNum = NumCardByLogic(hand, len, GetCardLogicValue(kHeartTwo));
    if (twoNum != 0)
        return false;


    iVector distribution[2];
    this->DistributionByValue(hand, len, distribution);

    int num_of_gap = NumberOfGap(distribution);
    int num_trump_needed = TrumpNeededForXLink(3, distribution);

    if (num_of_gap == 0 && num_trump_needed == 0)
    {
        FillInDescriptor(kHandTypeTripleLink, hand[0], 3, len/3);
        return true;
    }
    return false;
}


bool StaticAnalyserC::Is510K(int *hand, int len)
{
    int  type = kHandTypeBombFake510K;

    if (len != 3)
        return false;

    if (GetCardValue(hand[0]) != getCardValueFromWord(5))
        return false;
    if (GetCardValue(hand[1]) != getCardValueFromWord(10))
        return false;
    if (GetCardValue(hand[2]) != getCardValueFromWord(13))
        return false;

    if(GetCardColor(hand[0]) == GetCardColor(hand[1]) && GetCardColor(hand[0]) == GetCardColor(hand[2]))
    {
        type = kHandTypeBombTrue510K;
    }

    FillInDescriptor(type, 0, 0, 0);
    return true;
}

bool StaticAnalyserC::Is1HeartA(int *hand, int len)
{

    if (len != 1)
        return false;

    if (hand[0] != kHeartA)
        return false;

    FillInDescriptor(kHandTypeBomb1HeartA, 0, 0, 0);
    return true;
}

bool StaticAnalyserC::IsSameBomb(int *hand, int len)
{
    if (len < 4)
        return false;

    for (int i = 0; i < len - 1; i++)
    {
        if (GetCardValue(hand[i]) != GetCardValue(hand[i+1]))
            return false;
    }
    FillInDescriptor((len == 4 ? kHandTypeBomb4:kHandTypeBomb5to8), hand[0], len, 1);
    return true;
}

bool StaticAnalyserC::IsBomb2W(int *hand, int len)
{
    if (len != 2)
        return false;

    if((hand[0] == kTrumpHead && hand[1] == kTrumpHead) || (hand[0] == kTrumpVice && hand[1] == kTrumpVice))
    {
        FillInDescriptor(kHandTypeBomb2W, hand[0], len, 1);
        return true;
    }

    return false;
}

bool StaticAnalyserC::Is2HeartA(int *hand, int len)
{
    if (len != 2)
        return false;

    if(hand[0] == kHeartA && hand[1] == kHeartA)
    {

        FillInDescriptor(kHandTypeBomb2HeartA, hand[0], len, 1);
        return true;
    }

    return false;
}

bool judgeLink(int *value , int len)
{
    if (len >1)
    {
        for(int i = 0 ; i<len; i++)
        {
            //2不参与
            if(value[i] == getCardValueFromWord(2))
                return false;
        }
    }

    for(int i = 0 ; i<len-1; i++)
    {


        if(value[i+1]-value[i]!=1)
            return false;
    }
    return true;
}

bool StaticAnalyserC::IsAirPlane(int *hand, int len)
{
    if (len %5 != 0)
        return false;
    
    int linkNeeded = len/5;

    iVector distribution[2];
    this->DistributionByValue(hand, len, distribution);

    iVector values;
    int size = distribution[1].size();
    for (int i=0; i<size; i++)
    {
        int num = distribution[1][i];
        if( num >= 3)
        {
            values.push_back(distribution[0][i]);
        }
    }
    if (values.size()<linkNeeded)
    {
        return false;
    }
    for(int i = 0 ;i<values.size()-linkNeeded+1;i++)
    {
        if(judgeLink(&values[i] , linkNeeded))
        {
            FillInDescriptor(kHandTypeAirPlane, values[i]*4+1, 3, linkNeeded);
            return true;
        }
    }
    return false;
}
bool StaticAnalyserC::IsBomb(int *hand, int len, bool is_sorted)
{

    if (len < 4)
        return this->IsBomb3W(hand, len);

    if (len == 4)
        if(this->IsBombTW(hand, len))
            return true;



    for (int i = 0; i < len - 1; i++)
    {
        if (GetCardValue(hand[i]) != GetCardValue(hand[i+1]))
            return this->IsBombLink(hand, len);
    }
    FillInDescriptor(kHandTypeBomb, hand[0], len, 1);
    return true;
}

bool StaticAnalyserC::IsBomb3W(int *hand, int len)
{
    if (len != 3)
        return false;
    for (int i = 0; i < 3; i++)
    {
        if (GetCardColor(hand[i]) != kTrumpColor)
            return false;
    }
    FillInDescriptor(kHandTypeBomb3W, hand[0], 3, 1);
    return true;
}

bool StaticAnalyserC::IsBombTW(int *hand, int len)
{
    if (len != 4)
        return false;
    for (int i = 0; i<4; i++)
    {
        if (GetCardColor(hand[i]) != kTrumpColor)
            return false;
    }
    FillInDescriptor(kHandTypeBombTW, hand[0], 4, 1);
    return true;
}

//TODO:完善排炸的描述符
bool StaticAnalyserC::IsBombLink(int *hand, int len)
{
    if (len < 12)
        return false;

    SortByLogicValue(hand, len, true);

    const int trump_num = NumTrump(hand, len);

    iVector distribution[2];
    this->DistributionByLogicValue(hand, len-trump_num, distribution);

    int num_of_gap = NumberOfGap(distribution);
    int num_trump_needed = TrumpNeededForBomb(distribution);

    if(num_of_gap*4 + num_trump_needed <= trump_num)
    {
        FillInDescriptor(kHandTypeBombLink, hand[0], 4, len);
        return true;
    }
    return false;
}

bool StaticAnalyserC::GenHandDescriptor (int *hand, int len)
{
    // for(int i = 0; i<len; i++)
    // {
    //     printf("%d ",hand[i]);
    // }
    // printf("\n");
    if(len<=0)
        return false;
    if(len == 1)
    {
        if(Is1HeartA(hand, len))
        {
            return true;
        }
        FillInDescriptor(kHandTypeSingle, hand[0], 1, 1);
        return true;
    }
    SortByValue(hand, len);

    if(Is2HeartA(hand, len))
    {
        return true;
    }
    if(IsSameBomb(hand, len))
    {
        return true;
    }
    if(IsBomb2W(hand, len))
    {
        return true;
    }
    if(Is1HeartA(hand, len))
    {
        return true;
    }
    if(Is510K(hand, len))
    {
        return true;
    }
    ///
    if(IsAirPlane(hand, len))
    {
        return true;
    }
    if(IsShunZi(hand, len))
    {
        return true;
    }

    if(IsDoubleLink(hand, len))
    {
        return true;
    }
    if(IsDouble(hand, len))
    {
        return true;
    }

    return false;
}

bool StaticAnalyserC::OptionsSingleCard (int *hand, int len, int input_card, iVector *output_options)
{
    int logic_value = GetCardLogicValue(input_card);
    for (int i = 0; i<len; i++)
    {
        if (GetCardLogicValue(hand[i]) > logic_value)
            output_options->push_back(hand[i]);
    }
    return true;
}

void StaticAnalyserC::OptionsXples (int *hand, int len, int combo_card, int combo_len, iVector *output_options)
{
    if (len < 2)
        return;
    int combo_logic_value = GetCardLogicValue(combo_card);
    TrumpDescriptor trump_desc;
    NumTrump(hand, len, &trump_desc);

    for (int i = 0; i< len;)
    {
        int current_logic_value = GetCardLogicValue(hand[i]);
        int current_card_num    = NumCardByLogic(hand, len, current_logic_value);

        if (current_logic_value <= combo_logic_value)
        {
            i+=current_card_num;
            continue;
        }

        if (current_logic_value >= GetCardLogicValue(0x4E))
        {
            if (trump_desc.total_num >= combo_len)
            {
                this->MakeXplesWithTrump(0, 0, combo_len, trump_desc, output_options);
            }
            return;
        }

        if (current_card_num + trump_desc.total_num < combo_len)
        {
            i+=current_card_num;
            continue;
        }

        for (int base_len=1; base_len<=current_card_num;base_len++)
        {
            if (base_len > combo_len)
                break;
            int trump_needed = combo_len -base_len ;
            if (trump_desc.total_num < trump_needed)
                continue;
            this->MakeXplesWithTrump(hand[i], base_len, trump_needed, trump_desc, output_options);
        }
        i+=current_card_num;
    }
}

void StaticAnalyserC::OptionsBombs (int *hand, int len, int start_value, int block_len, int num_of_blocks, iVector *output_options)
{
    SortByLogicValue(hand, len, true);
    TrumpDescriptor trump_desc;
    NumTrump(hand, len, &trump_desc);

    int min_logic_value = GetCardLogicValue(start_value);
    for (int i = 0; i<len;)
    {
        int current_logic_value = GetCardLogicValue(hand[i]);
        int num = NumCardByLogic(hand, len, current_logic_value);
        if (current_logic_value <= min_logic_value)
        {
            int possible_max_block_len = trump_desc.total_num + num;
            if (possible_max_block_len <= block_len)
                i+=num;continue;
            this->MakeBombsWithTrump(hand, len, i, num, block_len+1, possible_max_block_len, trump_desc, output_options);
        }
        if (num >= block_len)
        {
            output_options->push_back(num);
            for (int j=0; j<num; j++)
                output_options->push_back(hand[i+j]);
        }
        i+=num;
    }
}

void StaticAnalyserC::FindBomb3TW (int *hand, int len, iVector *output_options)
{
    TrumpDescriptor trump_desc;
    NumTrump(hand, len, &trump_desc);
    if (trump_desc.total_num >= 3)
    {
        int vice = trump_desc.vice_trump_num;
        int head = trump_desc.trump_num;

        if (vice == 2)
        {
            output_options->push_back(3);
            output_options->push_back(kTrumpVice);
            output_options->push_back(kTrumpVice);
            output_options->push_back(kTrumpHead);
        }

        if (head == 2)
        {
            output_options->push_back(3);
            output_options->push_back(kTrumpHead);
            output_options->push_back(kTrumpHead);
            output_options->push_back(kTrumpVice);
        }

        if (head == 2 && vice == 2)
        {
            output_options->push_back(4);
            for (int i=0;i<2;i++)
            {
                output_options->push_back(kTrumpHead);
                output_options->push_back(kTrumpVice);
            }
        }
    }
}

void StaticAnalyserC::FindNormalBomb (int *hand, int len, iVector *output_options)
{
    for (int i=4; i<14; i++)
    {
        this->OptionsXples(hand, len, 0, i, output_options);
    }
}

//TODO: 2017-02-15
//完成寻找排炸的逻辑
void StaticAnalyserC:: FindBombLink (int *hand, int len, iVector *output_options)
{
    SortByLogicValue(hand, len, true);
    TrumpDescriptor trump_desc;
    NumTrump(hand, len, &trump_desc);

    int trump_total = trump_desc.total_num;
    int endOfNormalCards = len - trump_total;

    for (int i=0;i<endOfNormalCards;i++)
    {
        int current_logic_value = GetCardLogicValue(hand[i]);
        int current_card_num = NumCardByLogic(hand, len, current_logic_value);

        if (current_card_num + trump_total < 4)
        {
            i+=current_card_num;
            return;
        }
    }
}


//TODO:2017-02-14
//完成逻辑
bool StaticAnalyserC::OptionsXLinks (int *hand, int len, int link_start_card, int link_block_len, int link_total_len, iVector * output_options)
{
    int target_start_logic_value = GetCardLogicValue(link_start_card);
    SortByLogicValue(hand, len, false);
    for (int i=0; i<len;)
    {
        int current_logic_value = GetCardLogicValue(hand[i]);
        int current_value_num   = NumCardByLogic(hand, len, current_logic_value);
        if (current_logic_value <= target_start_logic_value)
        {
            i += current_value_num;
            continue;
        }

        // TODO:2017-02-10
        // 完成寻找连牌的逻辑
        bool is_good = true;
        int diff_card_needed = link_total_len/link_block_len;
        int previous_logic_value = current_logic_value;
        for (int j=current_value_num; j<diff_card_needed;)
        {
            int next_logic_value = GetCardLogicValue(hand[j]);
            int next_value_num   = NumCardByValue(hand, len, GetCardValue(hand[j]));
            if (next_logic_value - current_logic_value != 1)
            {
                is_good = false;
                break;
            }
            previous_logic_value = next_logic_value;
            j+=next_value_num;
        }
    }
}

//TODO:2017-02-14
//完成逻辑
bool StaticAnalyserC::React (int *hand, int len, int *out_hand)
{
    if (!this->GenHandDescriptor(hand, len))
        return false;
    this->GenOptions(hand, len);
}

//TODO:2017-02-15
//完成逻辑
bool StaticAnalyserC::GenOptions (int *hand, int len)
{
}

bool StaticAnalyserC::MakeXplesWithTrump (int base, int base_len, int trump_needed, TrumpDescriptor trump_desc, iVector *output_options)
{
    switch (trump_needed) {
        case 0:
            output_options->push_back(base_len);
            for (int j=0;j<base_len;j++)
                output_options->push_back(base);
            break;
        case 1:
            if (trump_desc.vice_trump_num > 0)
            {
                output_options->push_back(base_len);
                for (int j=0;j<base_len;j++)
                    output_options->push_back(base);
                output_options->push_back(0x4E);
            }
            if (trump_desc.trump_num > 0)
            {
                output_options->push_back(base_len);
                for (int j=0;j<base_len;j++)
                    output_options->push_back(base);
                output_options->push_back(0x4F);
            }
            break;
        case 2:
            if (trump_desc.vice_trump_num > 1)
            {
                output_options->push_back(base_len);
                for (int j=0;j<base_len;j++)
                    output_options->push_back(base);
                output_options->push_back(0x4E);
                output_options->push_back(0x4E);
            }
            if (trump_desc.trump_num > 1)
            {
                output_options->push_back(base_len);
                for (int j=0;j<base_len;j++)
                    output_options->push_back(base);
                output_options->push_back(0x4F);
                output_options->push_back(0x4F);
            }
            if (trump_desc.vice_trump_num > 0 && trump_desc.trump_num > 0)
            {
                output_options->push_back(base_len);
                for (int j=0;j<base_len;j++)
                    output_options->push_back(base);
                output_options->push_back(0x4F);
                output_options->push_back(0x4E);
            }
            break;
        case 3:
            if (trump_desc.vice_trump_num > 2)
            {
                output_options->push_back(base_len);
                for (int j=0;j<base_len;j++)
                    output_options->push_back(base);
                output_options->push_back(0x4E);
                output_options->push_back(0x4E);
                output_options->push_back(0x4E);
            }
            if (trump_desc.trump_num > 2)
            {
                output_options->push_back(base_len);
                for (int j=0;j<base_len;j++)
                    output_options->push_back(base);
                output_options->push_back(0x4F);
                output_options->push_back(0x4F);
                output_options->push_back(0x4F);
            }
            if (trump_desc.vice_trump_num > 0 && trump_desc.trump_num > 0 && trump_desc.total_num >= 3)
            {
                if (trump_desc.trump_num > 1)
                {
                    output_options->push_back(base_len);
                    for (int j=0;j<base_len;j++)
                        output_options->push_back(base);
                    output_options->push_back(0x4F);
                    output_options->push_back(0x4F);
                    output_options->push_back(0x4E);
                }
                if (trump_desc.vice_trump_num > 1)
                {
                    output_options->push_back(base_len);
                    for (int j=0;j<base_len;j++)
                        output_options->push_back(base);
                    output_options->push_back(0x4F);
                    output_options->push_back(0x4E);
                    output_options->push_back(0x4E);
                }
            }
            break;
        case 4:
            output_options->push_back(base_len);
            for (int j=0;j<base_len;j++)
                output_options->push_back(base);
            for (int j=0;j<2;j++)
            {
                output_options->push_back(0x4F);
                output_options->push_back(0x4E);
            }
            break;
        default:
            break;
    }
    return true;
}

bool StaticAnalyserC::FindCardsByValue(int *hand, int len, int card_value, int num)
{
    int occurence = 0;
    for (int i=0; i<len; i++)
    {
        int current_value = hand[i];
        if (current_value == card_value)
            occurence++;
        if (occurence == num)
            return true;
    }
    return false;
}

//TODO:2017-02-14
//测试
void StaticAnalyserC::MakeBombsWithTrump(
        int *hand,
        int len,
        int base_value_index, 
        int base_num, 
        int min_block_len, 
        int max_block_len, 
        TrumpDescriptor trump_desc, 
        iVector *output_options)
{
    for (int target_len=min_block_len; target_len<=max_block_len; target_len++)
    {
        int trump_needed = target_len - base_num;
        if (trump_desc.total_num < trump_needed)
            continue;
        for (int i=0;i<3;i++)
        {
            switch (i) {
                case 0:
                    if (trump_desc.vice_trump_num < trump_needed)
                        continue;
                    output_options->push_back(target_len);
                    for (int num=0;num<trump_needed;num++)
                        output_options->push_back(0x4E);
                    break;
                case 1:
                    if (trump_desc.trump_num < trump_needed)
                        continue;
                    output_options->push_back(target_len);
                    for (int num=0;num<trump_needed;num++)
                        output_options->push_back(0x4F);
                    break;
                case 2:
                    if (trump_needed < 2)
                        continue;
                    if (trump_desc.vice_trump_num > 0 && trump_desc.trump_num > 0)
                    {
                        output_options->push_back(target_len);
                        output_options->push_back(0x4E);
                        output_options->push_back(0x4F);
                        if (trump_needed -2 == 1)
                        {
                            if (trump_desc.vice_trump_num -1 > 0)
                                output_options->push_back(0x4E);
                            else
                                output_options->push_back(0x4F);
                        }
                        else if (trump_needed - 2 == 2)
                        {
                            output_options->push_back(0x4E);
                            output_options->push_back(0x4F);
                        }
                    }
                    break;
                default:
                    continue;
            }
            for (int z=0; z<base_num; z++)
                output_options->push_back(hand[base_value_index + z]);
        }
    }
}

int StaticAnalyserC::NumberOfGap(iVector *distribution)
{
    iVector type_value = distribution[0];
    int max = type_value.size()-1;

    // 判断点数是否连续
    int gap = 0;
    for (int i=0; i<max; i++)
    {
        int diff = type_value[i+1] - type_value[i];
        if (diff == 1)
            continue;
        gap += diff-1;
    }
    return gap;
}

int StaticAnalyserC::TrumpNeededForBomb(iVector *distribution)
{
    iVector type_num = distribution[1];

    // 判断需要使用多少张司令
    int size = type_num.size();
    int max = type_num[size-1];
    if (max < 4) 
        return 9999;

    int num_trump_needed = 0;

    for (int i=0; i<size; i++)
    {
        int current_num = type_num[i];
        int trump_needed = 4 - current_num;
        num_trump_needed += trump_needed > 0 ? trump_needed : 0;
    }
    return num_trump_needed;
}

int StaticAnalyserC::TrumpNeededForXLink(int target, iVector *distribution)
{
    iVector type_num = distribution[1];
    int size = type_num.size();
    int trump_needed = 0;
    for (int i=0; i<size; i++)
    {
        int num = type_num[i];
        if (num > target)
            return 9999;
        trump_needed += target-num;
    }
    return trump_needed;
}

void StaticAnalyserC::DistributionByValue(int *hand, int len, iVector *distribution)
{
    SortByValue(hand, len);

    iVector value_num;
    iVector value_type;

    for (int i = 0; i<len;)
    {
        int current_value = GetCardValue(hand[i]);
        int current_num = NumCardByValue(hand, len, current_value);
        value_num.push_back(current_num);
        value_type.push_back(current_value);
        i += current_num;
    }
    distribution[0] = value_type;
    distribution[1] = value_num;
}

void StaticAnalyserC::DistributionByLogicValue (int *hand, int len, iVector *distribution)
{
    SortByLogicValue(hand, len, true);

    iVector value_num;
    iVector value_type;

    for (int i = 0; i<len;)
    {
        int current_value = GetCardLogicValue(hand[i]);
        int current_num = NumCardByLogic(hand, len, current_value);
        value_num.push_back(current_num);
        value_type.push_back(current_value);
        i += current_num;
    }
    distribution[0] = value_type;
    distribution[1] = value_num;
}

void StaticAnalyserC::FillInDescriptor (int hand_type, int start_value, int block_len, int num_of_blocks)
{
    descriptor_.hand_type = hand_type;
    descriptor_.start_value = GetCardLogicValue(start_value);
    descriptor_.block_len = block_len;
    descriptor_.num_of_blocks = num_of_blocks;
}

void StaticAnalyserC::printDes()
{
    HandDescriptor des1 = this->GetDescriptor();
    printf("hand_type[%d]num_of_blocks[%d]start_value[%d]block_len[%d]\n",
    des1.hand_type , des1.num_of_blocks,  des1.start_value,  des1.block_len);
}

//判断1是否比2大
bool StaticAnalyserC::IsBiggerThan(StaticAnalyserC *target)
{
    HandDescriptor des1 = this->GetDescriptor();
    HandDescriptor des2 = target->GetDescriptor();


    if(des1.hand_type<kHandTypeBombFake510K)
    {
        if(des1.hand_type != des2.hand_type)
            return false;

        if(des1.num_of_blocks == des2.num_of_blocks && des1.start_value>des2.start_value && des1.block_len == des2.block_len)
            return true;
        else
            return false;

    }
    else /*炸弹*/
    {
        if(des1.hand_type > des2.hand_type)
            return true;
        if(des1.hand_type == des2.hand_type)
        {
            if(des1.hand_type == kHandTypeBomb5to8) /*5,6,7,8炸*/
            {
                if(des1.block_len>des2.block_len)
                    return true;
            }
            if(des1.num_of_blocks == des2.num_of_blocks && des1.start_value>des2.start_value && des1.block_len == des2.block_len)
                return true;
            else
                return false; 

        }
        else
        {
            return false;
        }
 
    }

}

}


