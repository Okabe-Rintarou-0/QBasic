#ifndef QBASIC_STRINGUTILS_H
#define QBASIC_STRINGUTILS_H

#include <string>
#include <vector>
#include <string_view>
#include <iostream>

class StringUtils {
public:
    static inline bool startWith(const std::string &str, const std::string &start) {
        return str.find(start) != std::string::npos;
    }

    static inline void
    split(std::string_view target, std::vector <std::string> &results, int maxNum, char separate = ' ') {
        int cursor = 0;
        int subStrNo = 1;
        int len = target.size();
        std::cout << subStrNo << std::endl;
        while (subStrNo < maxNum && cursor < len) {
            auto pos = target.find(separate, cursor);
            if (pos != std::string::npos) {
                results.emplace_back(target.substr(cursor, (pos - cursor)));
                cursor = pos + 1;
                ++subStrNo;
            } else break;
        }
        if (cursor < len) {
            results.emplace_back(target.substr(cursor, len - cursor));
        }

        for (auto result:results) {
            std::cout << result << std::endl;
        }
    }

    static inline std::string trim(const std::string &str) {
        auto pos1 = str.find_first_not_of(" ");
        pos1 = pos1 != std::string::npos ? pos1 : 0;
        auto pos2 = str.find_last_not_of(" ");
        pos2 = pos2 != std::string::npos ? pos2 : str.size() - 1;
        int len = pos2 - pos1 + 1;
        return str.substr(pos1, len);
    }

    static inline std::string getAfter(std::string_view str, std::string_view after) {
        auto pos = str.find(after);
        std::string result;
        if (pos != std::string::npos) {
            pos = pos + after.size();
            int subLen = str.size() - pos;
            result = str.substr(pos, subLen);
        }
        return trim(result);
    }
};

#endif //QBASIC_STRINGUTILS_H
