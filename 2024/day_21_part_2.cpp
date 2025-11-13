// #include <debug_print.h>
#include <main.h>
#include <image_processing.h>
#include <traveler.h>

#include <functional>
#include <map>
#include <queue>
#include <sstream>
#include <vector>
#include <utility>

std::map<std::pair<char, char>, std::vector<std::string>> shortest_paths;
std::map<std::tuple<char, char, int>, int64_t> results_cache;

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
        init_shortest_paths();
    }

    void init_shortest_paths()
    {
        for (auto start : where_is)
        {
            if (start.first == ' ')
                continue;

            for (auto end : where_is)
            {
                if (end.first == ' ')
                    continue;
                if (start.first == end.first)
                    continue;

                std::pair<char, char> key{start.first, end.first};
                if (shortest_paths.find(key) != shortest_paths.end())
                    return;

                shortest_paths[key] = path(start.first, end.first);
            }
        }
#ifdef DPRINT_ON
        for (auto it : shortest_paths)
        {
            std::cout << it.first.first << ":" << it.first.second;
            for (auto sq : it.second)
                std::cout << " " << sq;
            std::cout << std::endl;
        }
#endif
    }

    std::vector<std::string> path(char from, char to)
    {
        std::vector<SequenceList> list_per_key;
        Where start = where_is[from];
        Where end = where_is[to];
        SequenceList list;
        walk(start, end, {}, list);
        list_per_key.push_back(list);

        SequenceQueue sequences_queue;
        to_sequence(list_per_key, 0, {}, sequences_queue);
        std::vector<std::string> sequences;

        while (!sequences_queue.empty())
        {
            sequences.push_back(sequences_queue.top());
            sequences_queue.pop();
        }
        return sequences;
    }

    void to_sequence(
        const std::vector<SequenceList> &list_per_key, int key_index,
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

int64_t minimum_path_size(char start, char end, int robots);

int64_t minimum_sequence_size(std::string sequence, int robots)
{
    int64_t sequence_size = 0;
    char start = 'A';

    for (auto end : sequence)
    {
        if (start == end)
            sequence_size++;
        else
            sequence_size += minimum_path_size(start, end, robots);
        start = end;
    }

    return sequence_size;
}

int64_t minimum_path_size(char start, char end, int robots)
{
    std::tuple<char, char, int> key{start, end, robots};
    if (results_cache.find(key) != results_cache.end())
        return results_cache[key];

    int64_t sequence_size = 0;
    for (const auto &path : shortest_paths[{start, end}])
    {
        int64_t path_size;

        if (robots == 0)
            path_size = path.size();
        else
            path_size = minimum_sequence_size(path, robots - 1);
        CHECK(path_size > 0);

        if (sequence_size == 0 || path_size < sequence_size)
            sequence_size = path_size;
    }

    CHECK(sequence_size > 0);
    results_cache[key] = sequence_size;
#ifdef DPRINT_ON
    std::cout << robots << ": " << start << end << ": " << sequence_size << std::endl;
#endif
    return sequence_size;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> numeric_keys_to_type;
    std::string line;

    while (std::getline(file, line))
        numeric_keys_to_type.push_back(line);

    NumericKeypad n_key;
    DirectionalKeypad d_key;
    uint64_t complexities_of_codes = 0;

    for (const auto &numeric_keys : numeric_keys_to_type)
    {
        int64_t sequence_size = minimum_sequence_size(numeric_keys, 25);
        int numeric_part;
        std::stringstream(numeric_keys) >> numeric_part;
        complexities_of_codes += sequence_size * numeric_part;
        DPRINTX_ENDL(numeric_keys << ": " << sequence_size);
    }

    return complexities_of_codes;
}
