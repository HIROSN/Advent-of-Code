#include "main.h"

#include <vector>

bool Parse(std::ifstream &file, int count, std::vector<std::string> &entry);

uint64_t Answer(std::ifstream &file)
{
    struct NoteType
    {
        std::vector<std::string> patterns;
        std::vector<std::string> output;
    };

    std::vector<NoteType> notes;

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

    int digits_1_4_7_or_8 = 0;

    for (auto &note : notes)
    {
        for (auto &digit : note.output)
        {
            int segments = digit.size();

            switch (segments)
            {
            case 2:
            case 4:
            case 3:
            case 7:
                digits_1_4_7_or_8++;
                break;
            }
        }
    }

    return digits_1_4_7_or_8;
}

bool Parse(std::ifstream &file, int count, std::vector<std::string> &entry)
{
    for (int i = 0; i < count && file.good(); i++)
    {
        std::string str;
        file >> str;
        entry.push_back(std::move(str));
    }
    return entry.size() == count;
}
