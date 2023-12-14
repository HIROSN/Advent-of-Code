// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    int number_of_columns = 0, number_of_rows = 0;

    while (!file.eof())
    {
        std::vector<std::string> pattern;
        std::string line;

        while (std::getline(file, line) && !line.empty())
            pattern.push_back(line);

        DPRINT(pattern);
        const int size_x = pattern[0].size();
        const int size_y = pattern.size();

        enum class Type
        {
            Column,
            Row,
        };

        struct Reflection
        {
            Type type;
            int at = 0;
            int match = 0;
        };

        auto find_reflection = [&](Type type, int at) -> int
        {
            int i = 0, smudge = 0;

            switch (type)
            {
            case Type::Column:
                for (i = 0;
                     at + i < size_x && at - 1 - i >= 0;
                     i++)
                {
                    for (int y = 0; y < size_y; y++)
                    {
                        if (pattern[y][at + i] !=
                            pattern[y][at - 1 - i])
                        {
                            if (smudge++ == 1)
                                return i;
                        }
                    }
                }
                break;

            case Type::Row:
                for (i = 0;
                     at + i < size_y && at - 1 - i >= 0;
                     i++)
                {
                    for (int x = 0; x < size_x; x++)
                    {
                        if (pattern[at + i][x] !=
                            pattern[at - 1 - i][x])
                        {
                            if (smudge++ == 1)
                                return i;
                        }
                    }
                }
                break;
            }

            return smudge == 1 ? i : 0;
        };

        auto find_perfect_reflection = [&](Type type, int size,
                                           Reflection &reflection) -> bool
        {
            for (int at = 1; at < size; at++)
            {
                int match = find_reflection(type, at);
                if (match == std::min(at, size - at))
                {
                    reflection = {type, at, match};
                    return true;
                }
            }
            return false;
        };

        Reflection reflection;

        if (find_perfect_reflection(Type::Column, size_x, reflection) ||
            find_perfect_reflection(Type::Row, size_y, reflection))
        {
            if (reflection.type == Type::Column)
                number_of_columns += reflection.at;
            else
                number_of_rows += reflection.at;
        }
    }

    return number_of_columns + 100 * number_of_rows;
}
