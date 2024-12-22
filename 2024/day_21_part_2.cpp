#include <debug_print.h>
#include <main.h>
#include <image_processing.h>
#include <traveler.h>

#include <climits>
#include <functional>
#include <map>
#include <queue>
#include <sstream>
#include <vector>
#include <utility>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> numeric_keys_to_type;
    std::string line;

    while (std::getline(file, line))
        numeric_keys_to_type.push_back(line);

    using Where = std::pair<int, int>;
    using Sequence = std::vector<aoc::Offset>;
    using SequenceList = std::vector<Sequence>;
    using SequenceQueue = std::priority_queue<std::string,
                                              std::vector<std::string>,
                                              std::greater<std::string>>;
    struct Keypad
    {
        using Keys = std::vector<std::vector<char>>;

        Keys keys;
        Where gap;
        std::map<char, Where> where_is;

        void Init(int size_x, int size_y)
        {
            for (int x = 0; x < size_x; x++)
                for (int y = 0; y < size_y; y++)
                    where_is[keys[y][x]] = {x, y};
            gap = where_is[' '];
        }

        std::vector<std::string> type(std::string sequence)
        {
            std::vector<SequenceList> list_per_key;
            Where start = where_is['A'];

            for (char key : sequence)
            {
                if (!key)
                    continue;
                SequenceList list;
                Where end = where_is[key];
                walk(start, end, {}, list);
                start = end;
                list_per_key.push_back(list);
            }

            SequenceQueue sequences_queue;
            to_sequence(list_per_key, 0, {}, sequences_queue);
            std::vector<std::string> sequences;

            while (!sequences_queue.empty())
            {
                sequences.push_back(sequences_queue.top());
                sequences_queue.pop();
            }

            DCHECK(!sequences.empty());
            return sequences;
        }

        void to_sequence(
            const std::vector<SequenceList> &list_per_key, uint64_t key_index,
            std::string sequence, SequenceQueue &sequences)
        {
            if (key_index < list_per_key.size())
            {
                for (const auto &offsets : list_per_key[key_index])
                    to_sequence(list_per_key, key_index + 1,
                                sequence + to_string(offsets), sequences);
                return;
            }

            if (sequences.empty() || sequence.size() <= sequences.top().size())
                sequences.push(sequence);
        }

        std::string to_string(const Sequence &offsets)
        {
            std::string sequence_per_key;
            for (const auto &offset : offsets)
                sequence_per_key += offset.ch;
            return sequence_per_key;
        }

        void walk(Where start, const Where end, Sequence offsets,
                  SequenceList &list)
        {
            int distance = aoc::distance(start, end);

            if (!distance)
            {
                offsets.push_back({end.first, end.second, 'A'});
                list.push_back(offsets);
            }

            for (auto o : aoc::FourNeighbors().offsets)
            {
                Where next{start.first + o.x, start.second + o.y};
                if (next == gap || aoc::distance(next, end) >= distance)
                    continue;

                auto offsets_copy = offsets;
                offsets_copy.push_back(o);
                walk(next, end, offsets_copy, list);
            }
        }
    };

    struct NumericKeypad : public Keypad
    {
        NumericKeypad()
        {
            keys = Keys(4, std::vector<char>(3, 0));

            keys[0] = {'7', '8', '9'};
            keys[1] = {'4', '5', '6'};
            keys[2] = {'1', '2', '3'};
            keys[3] = {' ', '0', 'A'};

            Init(3, 4);
        }
    };

    struct DirectionalKeypad : public Keypad
    {
        DirectionalKeypad()
        {
            keys = Keys(2, std::vector<char>(3, 0));

            keys[0] = {' ', '^', 'A'};
            keys[1] = {'<', 'v', '>'};

            Init(3, 2);
        }
    };

    uint64_t complexities_of_codes = 0;
    constexpr int robots = 2;

    for (const auto &numeric_keys : numeric_keys_to_type)
    {
        DPRINTX(numeric_keys << ": ");
        std::vector<std::vector<std::string>>
            sequence_list(robots, std::vector<std::string>());
        std::vector<int64_t> sequence_size(robots, LLONG_MAX);

        for (const auto &numeric_sequence : NumericKeypad().type(numeric_keys))
        {
            for (const auto &sequence0 :
                 DirectionalKeypad().type(numeric_sequence))
            {
                sequence_list[0].push_back(sequence0);
                if (sequence0.size() < sequence_size[0])
                    sequence_size[0] = sequence0.size();
            }
        }

        for (int index = 0; index < robots; index++)
        {
            DPRINTX("{" << index + 1 << "," << sequence_size[index] << "}");

            for (const auto &sequence0 : sequence_list[index])
            {
                if (sequence0.size() != sequence_size[index])
                    continue;

                for (auto sequence1 : DirectionalKeypad().type(sequence0))
                {
                    if (sequence1.size() < sequence_size[index + 1])
                    {
                        sequence_size[index + 1] = sequence1.size();
                        // sequence_list[index + 1].push_back(sequence1);
                    }
                }
            }

            sequence_list[index].clear();
        }

        int numeric_part;
        std::stringstream(numeric_keys) >> numeric_part;
        complexities_of_codes += sequence_size[robots - 1] * numeric_part;
        DPRINT_ENDL();
    }

    return complexities_of_codes;
}
