#include "main.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <vector>

using PatternsType = std::vector<std::string>;
using DigitsType = std::map<std::string, int>;

bool Parse(std::ifstream &file, int count, std::vector<std::string> &entry);
void FindDigits(PatternsType &patterns, DigitsType &digits);
bool Includes(const std::string &pattern, const std::string &sub_pattern);
std::string Remove(std::string pattern, const std::string &remove);

uint64_t Answer(std::ifstream &file)
{
    struct NoteType
    {
        PatternsType patterns;
        PatternsType output;
    };

    std::vector<NoteType> notes;
    int output_value = 0;

    while (file.good())
    {
        NoteType note;
        std::string delimiter;

        if (Parse(file, 10, note.patterns) &&
            file >> delimiter && delimiter == "|" &&
            Parse(file, 4, note.output))
        {
            notes.push_back(std::move(note));
        }
    }

    for (auto &note : notes)
    {
        DigitsType digits;
        FindDigits(note.patterns, digits);
        std::stringstream ss;

        for (auto output : note.output)
        {
            ss << digits[output];
        }

        int value;
        ss >> value;
        output_value += value;
    }

    return output_value;
}

bool Parse(std::ifstream &file, int count, PatternsType &entry)
{
    for (int i = 0; i < count && file.good(); i++)
    {
        std::string str;
        file >> str;
        std::sort(str.begin(), str.end());
        entry.push_back(std::move(str));
    }
    return entry.size() == count;
}

void FindDigits(PatternsType &patterns, DigitsType &digits)
{
    std::string p1, p4, p7, p8;

    for (auto it = patterns.begin(); it != patterns.end();)
    {
        switch (it->size())
        {
        case 2:
            p1 = *it;
            digits[p1] = 1;
            break;
        case 4:
            p4 = *it;
            digits[p4] = 4;
            break;
        case 3:
            p7 = *it;
            digits[p7] = 7;
            break;
        case 7:
            p8 = *it;
            digits[p8] = 8;
            break;
        default:
            it++;
            continue;
        }

        patterns.erase(it);
    }

    for (auto it = patterns.begin(); it != patterns.end();)
    {
        switch (it->size())
        {
        case 5:
            if (Includes(*it, p1))
            {
                digits[*it] = 3;
            }
            else if (Includes(*it, Remove(p8, p4)))
            {
                digits[*it] = 2;
            }
            else
            {
                digits[*it] = 5;
            }
            break;
        case 6:
            if (Includes(*it, p4))
            {
                digits[*it] = 9;
            }
            else if (Includes(*it, Remove(p8, p7)))
            {
                digits[*it] = 6;
            }
            else
            {
                digits[*it] = 0;
            }
            break;
        default:
            it++;
            continue;
        }

        patterns.erase(it);
    }
}

bool Includes(const std::string &pattern, const std::string &sub_pattern)
{
    for (auto ch : sub_pattern)
    {
        if (pattern.find(ch) == std::string::npos)
        {
            return false;
        }
    }
    return true;
}

std::string Remove(std::string pattern, const std::string &remove)
{
    for (auto ch : remove)
    {
        auto it = pattern.find(ch);

        if (it != std::string::npos)
        {
            pattern.erase(it, 1);
        }
    }
    return pattern;
}
