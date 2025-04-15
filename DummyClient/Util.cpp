#include "pch.h"
#include "Util.h"
#include <iomanip>
#include <sstream>

string Util::ToString(int32 number, int32 digit)
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(digit) << number;

    return oss.str();
}
