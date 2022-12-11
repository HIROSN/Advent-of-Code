// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <list>
#include <queue>
#include <sstream>
#include <vector>

struct Monkey
{
    std::list<uint64_t> items;
    char operation;
    int value;
    int divisible_by;
    int if_true;
    int if_false;
    int inspected;

    bool read(std::ifstream &file);
};

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<Monkey> monkeys;
    uint64_t common_divisible_by = 1;

    while (true)
    {
        Monkey monkey = {};
        if (monkey.read(file))
        {
            common_divisible_by *= monkey.divisible_by;
            monkeys.push_back(std::move(monkey));
            continue;
        }
        break;
    }

    for (int round = 1; round <= 10000; round++)
    {
        for (auto &monkey : monkeys)
        {
            while (!monkey.items.empty())
            {
                uint64_t worry_level = monkey.items.front();
                monkey.items.pop_front();
                monkey.inspected++;

                switch (monkey.operation)
                {
                case '+':
                    worry_level += monkey.value;
                    break;
                case '*':
                    worry_level *= monkey.value ? monkey.value : worry_level;
                    break;
                }

                if (worry_level > common_divisible_by)
                {
                    worry_level %= common_divisible_by;
                }

                if (worry_level % monkey.divisible_by == 0)
                {
                    monkeys[monkey.if_true].items.push_back(worry_level);
                }
                else
                {
                    monkeys[monkey.if_false].items.push_back(worry_level);
                }
            }
        }
#ifdef DPRINT_ON
        if (round == 1 || round == 20 || round % 1000 == 0)
        {
            std::cout << "After round " << round << ":" << std::endl;
            for (int i = 0; i < monkeys.size(); i++)
            {
                std::cout << i << " (" << monkeys[i].inspected << "): ";
                for (auto &item : monkeys[i].items)
                    std::cout << item << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
#endif
    }

    std::priority_queue<int> most_active;
    for (auto &monkey : monkeys)
        most_active.push(monkey.inspected);
    uint64_t monkey_business = most_active.top();
    most_active.pop();
    monkey_business *= most_active.top();

    return monkey_business;
}

bool Monkey::read(std::ifstream &file)
{
    if (!skip<std::string>(file, 4))
        return false;

    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    int item;
    while (ss >> item)
    {
        items.push_back(item);
        skip<char>(ss);
    }

    skip<std::string>(file, 4);
    std::string str;
    file >> operation >> str;
    std::stringstream(str) >> value;

    skip<std::string>(file, 3);
    file >> divisible_by;

    skip<std::string>(file, 5);
    file >> if_true;

    skip<std::string>(file, 5);
    file >> if_false;

    return true;
}
