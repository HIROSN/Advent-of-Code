// #include <debug_print.h>
#include <main.h>

#include <algorithm>
#include <iterator>
#include <vector>

std::vector<int> read_input(std::ifstream &file, std::vector<int> &mixing);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<int> mixing;
    const std::vector<int> sequence = read_input(file, mixing);

    for (int index = 0; index < sequence.size(); index++)
    {
        const int number = sequence[index];
        if (number == 0)
            continue;

        auto it = std::find(mixing.begin(), mixing.end(), index);
        int position = it - mixing.begin();
        it = mixing.erase(it);

        int new_position = position + number;
        const int size = mixing.size();
        new_position = new_position % size;
        new_position += new_position < 0 ? size : 0;

        std::advance(it, new_position - position);
        if (it == mixing.begin())
            it = mixing.end();
        mixing.insert(it, index);

#ifdef DPRINT_ON
        if (mixing.size() < 80)
        {
            std::cout << number << ": ";
            for (auto i : mixing)
                std::cout << sequence[i] << " ";
            std::cout << std::endl;
        }
#endif
    }

    const int size = mixing.size();
    for (int index_zero = 0; index_zero < size; index_zero++)
    {
        if (sequence[mixing[index_zero]] == 0)
        {
#ifdef DPRINT_ON
            std::cout << sequence[mixing[(index_zero + 1000) % size]] << " "
                      << sequence[mixing[(index_zero + 2000) % size]] << " "
                      << sequence[mixing[(index_zero + 3000) % size]]
                      << std::endl;
#endif
            std::cout << (sequence[mixing[(index_zero + 1000) % size]] +
                          sequence[mixing[(index_zero + 2000) % size]] +
                          sequence[mixing[(index_zero + 3000) % size]])
                      << std::endl;
            break;
        }
    }
    return {};
}

std::vector<int> read_input(std::ifstream &file, std::vector<int> &mixing)
{
    std::vector<int> sequence;
    int number, index = 0;

    while (file >> number)
    {
        sequence.push_back(number);
        mixing.push_back(index++);
    }

#ifdef DPRINT_ON
    if (mixing.size() < 80)
    {
        for (auto i : mixing)
            std::cout << sequence[i] << " ";
        std::cout << std::endl;
    }
#endif

    return sequence;
}
