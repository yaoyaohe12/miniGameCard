#include "catch.hpp"
#include "static_analyser.h"
#include "common.h"
#include <vector>

namespace skrobot {

    
TEST_CASE("检查一手牌是不是对子", "[StaticAnalyserC, isDouble]") {
    StaticAnalyserC target;

    SECTION("数组长度不为２要报错") {
        int wrong_input[3] = {0x21,0x22,0x23};
        REQUIRE(target.IsDouble(wrong_input, 3) == false);
    }

    SECTION("数组长度为２，但点数不同") {
        int input[2] = {1, 5};
        REQUIRE(target.IsDouble(input, 2) == false);
    }

    SECTION("数组长度为２，且点数相同") {
        int input[2] = {1, 4};
        REQUIRE(target.IsDouble(input, 2) == true);
    }

    SECTION("红桃A不是对子，是炸") {
        int input[2] = {46, 46};
        REQUIRE(target.IsDouble(input, 2) == false);
    }
}



TEST_CASE("检查一手牌是不是顺子", "[StaticAnalyserC, IsShunZi]") {
    StaticAnalyserC target;

    SECTION("手牌长度小于５，返回false") {
        int input[4] = {1,2,3,4};
        REQUIRE(target.IsShunZi(input, 4) == false);
    }

    SECTION("手牌长度大于等于５，但牌的点数间有间隔，返回false") {
        int input[5] = {2,6,13,27,29};
        REQUIRE(target.IsShunZi(input, 5) == false);
    }

    SECTION("手牌长度大于等于5，且牌都是连贯的, 返回true") {
        int input[5] = {2,6,9,13,17};
        REQUIRE(target.IsShunZi(input, 5) == true);
    }

    SECTION("手牌长度大于等于5，不连贯，返回false") {
        int input[5] = {2,6,9,13,13};
        REQUIRE(target.IsShunZi(input, 5) == false);
    }

    SECTION("手牌长度大于等于5，且牌都是连贯的, 返回true") {
        int input[12] = {2,6,9,13,17,22,27,29,33,38,41,45};
        REQUIRE(target.IsShunZi(input, 12) == true);
    }

}


TEST_CASE("检查一手牌是否是对连", "[StaticAnalyserC, IsDoubleLink]") {
    StaticAnalyserC target;

    SECTION("数组的长度不被２整除，返回false") {
        int input[7] = {1,2,3,4,5,6,7};
        REQUIRE(target.IsDoubleLink(input, 7) == false);
    }

    SECTION("数组的长度小于5，返回false") {
        int input[4] = {1,2,3,4};
        REQUIRE(target.IsDoubleLink(input, 4) == false);
    }

    SECTION("2不能出现在对连中") {
        int input[6] = {2, 4, 6, 8, 10, kHeartTwo};
        REQUIRE(target.IsDoubleLink(input, 6) == false);
    }

    SECTION("数组长度符合条件，但存在落单的牌，返回false") {
        int input[6] = {2, 4, 6, 8, 10, 16};
        REQUIRE(target.IsDoubleLink(input, 6) == false);
    }

    SECTION("数组的长度符合条件，全是对子，但点数不连贯, 返回false") {
        int input[6] = {2, 4, 6, 8, 14, 16};
        REQUIRE(target.IsDoubleLink(input, 6) == false);
    }

    SECTION("数组的长度符合条件，全是对子，且点数连贯, 返回true") {
        int input[6] = {2, 4, 6, 8, 10, 12};
        REQUIRE(target.IsDoubleLink(input, 6) == true);
    }
}


TEST_CASE("检查510K", "[StaticAnalyserC, Is510K]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[3] = {1,2,3};
        REQUIRE(target.Is510K(input, 3) == false);
    }

    SECTION("11111111") {
        int input[3] = {1,2};
        REQUIRE(target.Is510K(input, 2) == false);
    }

    SECTION("11111111") {
        int input[3] = {9,29,29};
        REQUIRE(target.Is510K(input, 3) == false);
    }

    SECTION("11111111") {
        int input[3] = {9,29,41};
        REQUIRE(target.Is510K(input, 3) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBombTrue510K);
    }

    SECTION("11111111") {
        int input[3] = {9,29,42};
        REQUIRE(target.Is510K(input, 3) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBombFake510K);
    }
}

TEST_CASE("红桃A", "[StaticAnalyserC, HeartA]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[1] = {kHeartA};
        REQUIRE(target.Is1HeartA(input, 1) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb1HeartA);
    }

    SECTION("11111111") {
        int input[2] = {kHeartA,kHeartA};
        REQUIRE(target.Is2HeartA(input, 2) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb2HeartA);
    }

    SECTION("11111111") {
        int input[1] = {1};
        REQUIRE(target.Is1HeartA(input,1) == false);
    }  

    SECTION("11111111") {
        int input[2] = {1,kHeartA};
        REQUIRE(target.Is2HeartA(input, 2) == false);
    }    

    SECTION("11111111") {
        int input[3] = {kHeartA, kHeartA, kTrumpHead};
        REQUIRE(target.Is2HeartA(input, 3) == false);
    }    
}

TEST_CASE("重复牌的炸弹", "[StaticAnalyserC, IsSameBomb]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[4] = {25,26,27,28};
        REQUIRE(target.IsSameBomb(input, 4) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb4);
    }

    SECTION("11111111") {
        int input[6] = {25,25,26,26,27,28};
        REQUIRE(target.IsSameBomb(input, 6) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb5to8);
        REQUIRE(target.GetDescriptor().block_len == 6);
        REQUIRE(target.GetDescriptor().start_value == 6);
    }
}

TEST_CASE("俩王炸", "[StaticAnalyserC, IsBomb2W]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[2] = {kTrumpHead,kTrumpHead};
        REQUIRE(target.IsBomb2W(input, 2) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb2W);
    }

    SECTION("11111111") {
        int input[2] = {kTrumpVice,kTrumpVice};
        REQUIRE(target.IsBomb2W(input, 2) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb2W);
    }

    SECTION("11111111") {
        int input[2] = {kTrumpVice,kTrumpHead};
        REQUIRE(target.IsBomb2W(input, 2) == false);
    }

    SECTION("11111111") {
        int input[3] = {kTrumpVice,kTrumpVice,kTrumpHead};
        REQUIRE(target.IsBomb2W(input, 3) == false);
    }
}

TEST_CASE("飞机，包括3带2", "[StaticAnalyserC, IsAirPlane]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[5] = {2,3,4,5,6};
        REQUIRE(target.IsAirPlane(input, 5) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 0);
        REQUIRE(target.GetDescriptor().num_of_blocks == 1);
    }

    SECTION("11111111") {
        int input[10] = {2,3,4,5,42,43,44,45,46,47};
        REQUIRE(target.IsAirPlane(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 10);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[10] = {42,43,44,45,46,47,48,49,50,51}; /*KKKAAAA222*/
        REQUIRE(target.IsAirPlane(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 10);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[10] = {12,13,14,15,16,17,18,19,20,21}; /*5666677778*/
        REQUIRE(target.IsAirPlane(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 3);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[15] = {1,2,5,6,7,10,11,12,13,14,15,18,22,25,32}; /*444555666 */
        REQUIRE(target.IsAirPlane(input, 15) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 1);
        REQUIRE(target.GetDescriptor().num_of_blocks == 3);
    }

    SECTION("11111111") {
        int input[10] = {1,2,3,5,9,10,11,14,15,16}; /*3334555666 */
        REQUIRE(target.IsAirPlane(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 2);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[10] = {1,4,8,12,45,46,47,49,50,51}; /*3456AAA222 */
        REQUIRE(target.IsAirPlane(input, 10) == false);
    }
}




/***************************************************/

TEST_CASE("检查一手牌是不是对子 GenHandDescriptor", "[StaticAnalyserC, GenHandDescriptor]") {
    StaticAnalyserC target;

    SECTION("数组长度为２，但点数不同") {
        int input[2] = {1, 5};
        REQUIRE(target.GenHandDescriptor(input, 2) == false);
    }

    SECTION("数组长度为２，且点数相同") {
        int input[2] = {1, 4};
        REQUIRE(target.GenHandDescriptor(input, 2) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeDouble);
    }
}


TEST_CASE("检查一手牌是不是三条 GenHandDescriptor", "[StaticAnalyserC, IsTriple]") {
    StaticAnalyserC target;

    SECTION("数组长度为３，但点数不同，返回false") {
        int input[3] = {51, 52, 53};
        REQUIRE(target.GenHandDescriptor(input, 3) == false);
    }

    SECTION("数组长度为３，且点数相同，返回true") {
        int input[3] = {1, 2, 3};
        REQUIRE(target.GenHandDescriptor(input, 3) == false);
    }

    SECTION("数组长度为３，且点数相同，返回false") {
        int input[3] = {50, 51, 52};
        REQUIRE(target.GenHandDescriptor(input, 3) == false);
    }

}


TEST_CASE("检查一手牌是不是顺子 GenHandDescriptor", "[StaticAnalyserC, IsShunZi]") {
    StaticAnalyserC target;

    SECTION("手牌长度大于等于５，但牌的点数间有间隔，返回false") {
        int input[5] = {2,6,13,27,29};
        REQUIRE(target.GenHandDescriptor(input, 5) == false);
    }

    SECTION("手牌长度大于等于5，且牌都是连贯的, 返回true") {
        int input[5] = {2,6,9,13,17};
        REQUIRE(target.GenHandDescriptor(input, 5) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeShunzi);
    }

    SECTION("手牌长度大于等于5，不连贯，返回false") {
        int input[5] = {2,6,9,13,13};
        REQUIRE(target.GenHandDescriptor(input, 5) == false);
    }

    SECTION("手牌长度大于等于5，且牌都是连贯的, 返回true") {
        int input[12] = {2,6,9,13,17,22,27,29,33,38,41,45};
        REQUIRE(target.GenHandDescriptor(input, 12) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeShunzi);
    }

}


TEST_CASE("检查一手牌是否是对连 GenHandDescriptor", "[StaticAnalyserC, IsDoubleLink]") {
    StaticAnalyserC target;

    SECTION("数组的长度不被２整除，返回false") {
        int input[7] = {1,2,3,4,5,6,7};
        REQUIRE(target.GenHandDescriptor(input, 7) == false);
    }

    SECTION("2不能出现在对连中") {
        int input[6] = {2, 4, 6, 8, 10, kHeartTwo};
        REQUIRE(target.GenHandDescriptor(input, 6) == false);
    }

    SECTION("数组长度符合条件，但存在落单的牌，返回false") {
        int input[6] = {2, 4, 6, 8, 10, 16};
        REQUIRE(target.GenHandDescriptor(input, 6) == false);
    }

    SECTION("数组的长度符合条件，全是对子，但点数不连贯, 返回false") {
        int input[6] = {2, 4, 6, 8, 14, 16};
        REQUIRE(target.GenHandDescriptor(input, 6) == false);
    }

    SECTION("数组的长度符合条件，全是对子，且点数连贯, 返回true") {
        int input[6] = {2, 4, 6, 8, 10, 12};
        REQUIRE(target.GenHandDescriptor(input, 6) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeDoubleLink);
    }
}

TEST_CASE("检查一手牌是否是三连 GenHandDescriptor", "[StaticAnalyserC, GenHandDescriptor]") {
    StaticAnalyserC target;

    SECTION("数组的长度不被3整除，返回false") {
        int input[7] = {1,2,3,4,5,6,7};
        REQUIRE(target.GenHandDescriptor(input, 7) == false);
    }

    SECTION("数组长度符合条件，但存在落单的牌,返回false") {
        int input[9] = {2, 3, 4, 6, 7, 8, 10, 11, 27};
        REQUIRE(target.GenHandDescriptor(input, 9) == false);
    }

    SECTION("数组的长度符合条件，全是三条，但点数不连贯, 返回false") {
        int input[9] = {2, 3, 4, 6, 7, 8, 25, 26, 27};
        REQUIRE(target.GenHandDescriptor(input, 9) == false);
    }

    SECTION("数组的长度符合条件，全是三条，且点数连贯, 返回true") {
        int input[9] = {2, 3, 4, 6, 7, 8, 10, 11, 12};
        REQUIRE(target.GenHandDescriptor(input, 9) == false);
    }

    SECTION("数组的长度符合条件，全是三条，且点数连贯, 返回true") {
        int input[12] = {2, 3, 4, 6, 7, 8, 10, 11, 12,14,15,16};
        REQUIRE(target.GenHandDescriptor(input, 12) == false);
    }
}


TEST_CASE("检查510K GenHandDescriptor", "[StaticAnalyserC, Is510K]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[3] = {9,29,29};
        REQUIRE(target.GenHandDescriptor(input, 3) == false);
    }

    SECTION("11111111") {
        int input[3] = {9,29,41};
        REQUIRE(target.GenHandDescriptor(input, 3) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBombTrue510K);
    }

    SECTION("11111111") {
        int input[3] = {9,29,42};
        REQUIRE(target.GenHandDescriptor(input, 3) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBombFake510K);
    }
}

TEST_CASE("红桃A GenHandDescriptor", "[StaticAnalyserC, HeartA]") {
    StaticAnalyserC target;


    SECTION("11111111") {
        int input[2] = {kHeartA,kHeartA};
        REQUIRE(target.GenHandDescriptor(input, 2) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb2HeartA);
    }

    SECTION("11111111") {
        int input[1] = {kHeartA};
        REQUIRE(target.GenHandDescriptor(input,1) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb1HeartA);
    }  

    SECTION("11111111") {
        int input[2] = {1,kHeartA};
        REQUIRE(target.GenHandDescriptor(input, 2) == false);
    }    

    SECTION("11111111") {
        int input[3] = {kHeartA, kHeartA, kTrumpHead};
        REQUIRE(target.GenHandDescriptor(input, 3) == false);
    }    
}

TEST_CASE("重复牌的炸弹 GenHandDescriptor", "[StaticAnalyserC, IsSameBomb]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[4] = {25,26,27,28};
        REQUIRE(target.GenHandDescriptor(input, 4) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb4);
    }

    SECTION("11111111") {
        int input[6] = {25,25,26,26,27,28};
        REQUIRE(target.GenHandDescriptor(input, 6) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb5to8);
        REQUIRE(target.GetDescriptor().block_len == 6);
        REQUIRE(target.GetDescriptor().start_value == 6);
    }
}

TEST_CASE("俩王炸 GenHandDescriptor", "[StaticAnalyserC, GenHandDescriptor]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[2] = {kTrumpHead,kTrumpHead};
        REQUIRE(target.GenHandDescriptor(input, 2) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb2W);
    }

    SECTION("11111111") {
        int input[2] = {kTrumpVice,kTrumpVice};
        REQUIRE(target.GenHandDescriptor(input, 2) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeBomb2W);
    }

    SECTION("11111111") {
        int input[2] = {kTrumpVice,kTrumpHead};
        REQUIRE(target.GenHandDescriptor(input, 2) == false);
    }

    SECTION("11111111") {
        int input[3] = {kTrumpVice,kTrumpVice,kTrumpHead};
        REQUIRE(target.GenHandDescriptor(input, 3) == false);
    }
}

TEST_CASE("飞机，包括3带2 GenHandDescriptor", "[StaticAnalyserC, GenHandDescriptor]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[5] = {2,3,4,5,6};
        REQUIRE(target.GenHandDescriptor(input, 5) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 0);
        REQUIRE(target.GetDescriptor().num_of_blocks == 1);
    }

    SECTION("11111111") {
        int input[10] = {2,3,4,5,42,43,44,45,46,47};
        REQUIRE(target.GenHandDescriptor(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 10);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[10] = {42,43,44,45,46,47,48,49,50,51}; /*KKKAAAA222*/
        REQUIRE(target.GenHandDescriptor(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 10);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[10] = {12,13,14,15,16,17,18,19,20,21}; /*5666677778*/
        REQUIRE(target.GenHandDescriptor(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 3);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[15] = {1,2,5,6,7,10,11,12,13,14,15,18,22,25,32}; /*444555666 */
        REQUIRE(target.GenHandDescriptor(input, 15) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 1);
        REQUIRE(target.GetDescriptor().num_of_blocks == 3);
    }

    SECTION("11111111") {
        int input[10] = {1,2,3,5,9,10,11,14,15,16}; /*3334555666 */
        REQUIRE(target.GenHandDescriptor(input, 10) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeAirPlane);
        REQUIRE(target.GetDescriptor().start_value == 2);
        REQUIRE(target.GetDescriptor().num_of_blocks == 2);
    }

    SECTION("11111111") {
        int input[10] = {1,4,8,12,45,46,47,49,50,51}; /*3456AAA222 */
        REQUIRE(target.GenHandDescriptor(input, 10) == false);
    }
}

TEST_CASE("单张牌", "[StaticAnalyserC, GenHandDescriptor]") {
    StaticAnalyserC target;

    SECTION("11111111") {
        int input[1] = {1};
        REQUIRE(target.GenHandDescriptor(input,1) == true);
        REQUIRE(target.GetDescriptor().hand_type == kHandTypeSingle);
    }  
}


TEST_CASE("大小比较", "[StaticAnalyserC, IsBiggerThan]") {
    StaticAnalyserC target1;
    StaticAnalyserC target2;

    SECTION("11111111") {
        int input1[1] = {1};
        int input2[1] = {5};
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }  

    SECTION("11111111") {
        int input1[1] = {1};
        int input2[1] = {kTrumpHead};
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }  

    SECTION("11111111") {
        int input1[1] = {1};
        int input2[3] = {9, 29, 44}; /*假5 10 K*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }  

    SECTION("11111111") {
        int input1[1] = {9};
        int input2[1] = {10};
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }  

    SECTION("11111111") {
        int input1[2] = {9,10};
        int input2[2] = {13,14};
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }  

    SECTION("11111111") {
        int input1[2] = {9,10};
        int input2[2] = {1,2};
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }

    SECTION("11111111") {
        int input1[2] = {9,10};
        int input2[5] = {1,2,3,5,6};
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    } 

    SECTION("11111111") {
        int input1[2] = {9,10};
        int input2[3] = {9, 29, 44}; /*假5 10 K*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    } 

    SECTION("11111111") {
        int input1[2] = {9,10};
        int input2[4] = {33,34,35,36}; /*4炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    } 

    SECTION("11111111") {
        int input1[2] = {9,10};
        int input2[1] = {kHeartA}; /*红桃A*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    } 

    SECTION("11111111") {
        int input1[5] = {6,7,8,9,10}; /*3带2*/
        int input2[5] = {10,11,12,13,14};  /*3带2*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }    
    SECTION("11111111") {
        int input1[5] = {6,7,8,9,10}; /*3带2*/
        int input2[5] = {4,8,12,16,20}; /*34567*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }    


    SECTION("11111111") {
        int input1[5] = {6,7,8,9,10}; /*3带2*/
        int input2[3] = {9, 29, 44}; /*假5 10 K*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }   

    SECTION("11111111") {
        int input1[3] = {9, 29, 44}; /*假5 10 K*/
        int input2[2] = {9, 9}; /*假5 10 K*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }   

    SECTION("11111111") {
        int input1[3] = {9, 29, 44}; /*假5 10 K*/
        int input2[3] = {9, 29, 44}; /*假5 10 K*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }   

    SECTION("11111111") {
        int input1[3] = {9, 29, 44}; /*假5 10 K*/
        int input2[1] = {kHeartA}; /*红桃A*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }   

    SECTION("11111111") {
        int input1[3] = {9, 29, 44}; /*假5 10 K*/
        int input2[3] = {9, 29, 41}; /*真5 10 K*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    } 

    SECTION("11111111") {
        int input1[3] = {9, 29, 44}; /*假5 10 K*/
        int input2[4] = {33,34,35,36}; /*4炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    } 

    SECTION("11111111") {
        int input1[4] = {1,2,3,4}; /*4炸*/
        int input2[4] = {33,34,35,36}; /*4炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    } 

    SECTION("11111111") {
        int input1[4] = {1,2,3,4}; /*4炸*/
        int input2[1] = {kHeartA}; /*红桃A*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    } 

    SECTION("11111111") {
        int input1[4] = {1,2,3,4}; /*4炸*/
        int input2[5] = {1,1,2,3,4}; /*5炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }

    SECTION("11111111") {
        int input1[2] = {kTrumpVice, kTrumpVice}; /*两小王*/
        int input2[4] = {1,2,3,4}; /*4炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }

    SECTION("11111111") {
        int input1[2] = {kTrumpVice, kTrumpVice}; /*两小王*/
        int input2[1] = {1}; 
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }

    SECTION("11111111") {
        int input1[2] = {kTrumpVice, kTrumpVice}; /*两小王*/
        int input2[2] = {kTrumpHead, kTrumpHead}; /*两大王*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        target1.printDes();
        target2.printDes();
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }

    SECTION("11111111") {
        int input1[2] = {kTrumpVice, kTrumpVice}; /*两小王*/
        int input2[5] = {1,1,2,3,4}; /*5炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }

    SECTION("11111111") {
        int input1[2] = {kTrumpVice, kTrumpVice}; /*两小王*/
        int input2[2] = {kHeartA, kHeartA}; 
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }

    SECTION("11111111") {
        int input1[2] = {kTrumpHead, kTrumpHead}; /*两大王*/
        int input2[2] = {kTrumpVice, kTrumpVice}; /*两小王*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }

    SECTION("11111111") {
        int input1[2] = {kTrumpHead, kTrumpHead}; /*两大王*/
        int input2[5] = {1,1,2,3,4}; /*5炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }

    SECTION("11111111") {
        int input1[5] = {1,1,2,3,4}; /*5炸*/
        int input2[5] = {1,1,2,3,4}; /*5炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == false);
    }

    SECTION("11111111") {
        int input1[5] = {1,1,2,3,4}; /*5炸*/
        int input2[5] = {5,6,7,8,8}; /*5炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }

    SECTION("11111111") {
        int input1[5] = {1,1,2,3,4}; /*5炸*/
        int input2[8] = {1,1,2,2,3,3,4,4}; /*8炸*/
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }

    SECTION("11111111") {
        int input1[8] = {1,1,2,2,3,3,4,4}; /*8炸*/
        int input2[2] = {kHeartA, kHeartA}; 
        REQUIRE(target1.GenHandDescriptor(input1,sizeof(input1)/sizeof(input1[0])) == true);
        REQUIRE(target2.GenHandDescriptor(input2,sizeof(input2)/sizeof(input2[0])) == true);
        REQUIRE(target2.IsBiggerThan(&target1) == true);
    }
}

}
