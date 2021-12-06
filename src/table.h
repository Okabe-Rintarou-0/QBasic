#ifndef TABLE_H
#define TABLE_H

#include <map>

namespace env {
    enum valueType {
        INT,
        STRING
    };

    class Value {
    private:
        std::string sVal;
        int iVal;
    public:
        Value() = default;

        ~Value() = default;

        Value(const std::string &sVal) : sVal(sVal) {}

        Value(int iVal) : iVal(iVal) {}

        inline int getInt() const { return iVal; }

        inline std::string getString() const { return sVal; }
    };

    template<typename K, typename V>
    class Table {
    private:
        std::map <K, V> map;
    public:
        Table() = default;

        ~Table() = default;

        inline V *look(const K &key) {
            if (map.find(key) != map.end())
                return &(map[key]);
            return nullptr;
        }

        inline void enter(const K &key, const V &value) {
            map[key] = value;
        }
    };
}


#endif // TABLE_H
