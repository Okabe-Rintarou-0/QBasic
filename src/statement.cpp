#include "statement.h"
#include "stringutils.h"
#include <exception>

namespace statement {
    RawStatement *RawStatement::fromCmdline(const std::string &cmdline) {
        if (valid(cmdline)) {
            std::vector <std::string> linenoAndSrcCode;
            StringUtils::split(cmdline, linenoAndSrcCode, 2);
            int lineno = std::atoi(linenoAndSrcCode[0].c_str());
            std::string srcCode = linenoAndSrcCode[1];
            if (lineno < 0 || lineno > 1000000)
                throw "Invalid Statement! Should contain line number! Line number should be >= 0 and <= 1000000!";
            return new RawStatement(lineno, srcCode);
        }
            throw "Invalid Statement! Should contain line number and statement!";
    }
}
