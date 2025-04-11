#include "pch.h"
#include "Random.h"

int32 Random::GetRandom(int32 min, int32 max)
{
    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<> random_num(min, max);

    return random_num(gen);
}
