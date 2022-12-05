#include <main.h>

#include <sstream>
#include <stack>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::stack<std::string> starting_stacks;
    int number_of_stacks = 0;

    while (file.good())
    {
        std::string line;
        std::getline(file, line);

        if (line.size() > 1 && line[1] == '1')
        {
            std::stringstream ss(line);
            while (ss >> number_of_stacks)
            {
                continue;
            }
            break;
        }
        starting_stacks.push(std::move(line));
    }

    std::vector<std::stack<char>> crate_stack(number_of_stacks);

    while (!starting_stacks.empty())
    {
        const auto &line = starting_stacks.top();
        for (int n = 0; n < number_of_stacks; n++)
        {
            const int i = n * 4 + 1;
            if (i < line.size())
            {
                if (line[i] != ' ')
                {
                    crate_stack[n].push(line[i]);
                }
            }
            else
            {
                break;
            }
        }
        starting_stacks.pop();
    }

    std::string s;
    int move, from, to;

    while (file >> s && file >> move &&
           file >> s && file >> from &&
           file >> s && file >> to)
    {
        from--;
        to--;
        for (int i = 0; i < move; i++)
        {
            auto crate = crate_stack[from].top();
            crate_stack[from].pop();
            crate_stack[to].push(crate);
        }
    }

    for (const auto &stack : crate_stack)
    {
        std::cout << stack.top();
    }
    std::cout << std::endl;

    return {};
}
