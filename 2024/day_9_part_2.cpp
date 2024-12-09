// #include <debug_print.h>
#include <main.h>

#include <list>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Blocks
    {
        int id;
        int size;
    };

    std::list<Blocks> disk_map;
    const int Space = -1;
    std::string line;
    std::getline(file, line);
    int position = 0;
    uint64_t checksum = 0;

    for (int i = 0; i < line.size(); i++)
        disk_map.push_back({i % 2 ? Space : i / 2, line[i] - '0'});

    auto debug_print = [&]() -> void
    {
#ifdef DPRINT_ON
        for (auto blocks : disk_map)
            for (int i = 0; i < blocks.size; i++)
                if (blocks.id == Space)
                    std::cout << '.';
                else
                    std::cout << blocks.id;
        std::cout << std::endl;
#endif
    };
    debug_print();

    while (true)
    {
        bool moved = false;
        auto it_right = disk_map.rbegin();

        for (int r = disk_map.size() - 1; !moved && r >= 0; r--)
        {
            if (it_right->id != Space)
            {
                auto it_left = disk_map.begin();

                for (int l = 0; !moved && l < disk_map.size(); l++)
                {
                    if (l < r && it_left->id == Space)
                    {
                        if (it_left->size >= it_right->size)
                        {
                            if (it_left->size == it_right->size)
                            {
                                it_left->id = it_right->id;
                                it_right->id = Space;
                            }
                            else
                            {
                                auto move = *it_right;
                                it_right->id = Space;
                                it_left->size -= move.size;
                                disk_map.insert(it_left, move);
                            }
                            moved = true;
                        }
                    }
                    it_left++;
                }
            }
            it_right++;
        }

        if (!moved)
            break;
        debug_print();
    }

    for (auto it = disk_map.begin(); it != disk_map.end(); it++)
    {
        if (it->id != Space)
        {
            for (int i = 0; i < it->size; i++)
                checksum += (position + i) * it->id;
        }
        position += it->size;
    }

    return checksum;
}
