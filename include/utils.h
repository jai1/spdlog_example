#pragma once

#include <iomanip>
#include <locale>
#include <sstream>

namespace utils {

    template<typename T>
    inline std::string format(const T &value)
    {
        static std::locale loc("");
        std::stringstream ss;
        ss.imbue(loc);
        ss << value;
        return ss.str();
    }

    template<>
    inline std::string format(const double &value)
    {
        static std::locale loc("");
        std::stringstream ss;
        ss.imbue(loc);
        ss << std::fixed << std::setprecision(1) << value;
        return ss.str();
    }

} // namespace utils