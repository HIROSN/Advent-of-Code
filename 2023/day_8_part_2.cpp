#include <main.h>

#include <numeric>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Node
    {
        char ch[3] = {};

        Node() = default;

        Node(const std::string &str)
        {
            for (int i = 0; i < 3; i++)
                ch[i] = str[i];
        }

        int idx(int i)
        {
            return ch[i] - 'A';
        }

        bool end() const
        {
            return ch[2] == 'Z';
        }

        bool valid() const
        {
            for (int i = 0; i < 3; i++)
            {
                if ((ch[i] < 'A' || ch[i] > 'Z') &&
                    (ch[i] < '0' || ch[i] > '9'))
                {
                    return false;
                }
            }
            return true;
        }
    };

    struct NodePair
    {
        Node node[2];

        NodePair() = default;

        NodePair(const std::string &left, const std::string &right)
        {
            node[0] = Node(left);
            node[1] = Node(right);
        }
    };

    struct Navigator
    {
        NodePair network[26][26][26];
        std::vector<int> instructions;
        std::vector<Node> nodes;
        std::vector<int> steps;
        int index = 0;

        Navigator(std::ifstream &file)
        {
            std::string line;
            std::getline(file, line);
            for (const auto &ch : line)
                instructions.push_back(ch == 'L' ? 0 : 1);
            std::getline(file, line);

            while (std::getline(file, line))
            {
                Node node(line.substr(0, 3));
                network[node.idx(0)][node.idx(1)][node.idx(2)] = NodePair(
                    line.substr(7, 3), line.substr(12, 3));

                if (node.ch[2] == 'A')
                {
                    nodes.push_back(node);
                    steps.push_back(0);
                }
            }
        }

        bool navigate(Node &node)
        {
            const auto &pair = network[node.idx(0)][node.idx(1)][node.idx(2)];
            node = pair.node[instructions[index]];
            return node.valid();
        }

        bool navigate_all()
        {
            for (int i = 0; i < nodes.size(); i++)
            {
                if (!nodes[i].end())
                {
                    if (!navigate(nodes[i]))
                        return false;
                    steps[i]++;
                }
            }
            index = ++index % instructions.size();
            return true;
        }

        bool end() const
        {
            for (int i = 0; i < nodes.size(); i++)
            {
                if (!nodes[i].end())
                    return false;
            }
            return true;
        }
    };

    Navigator navigator(file);
    uint64_t steps_lcm = 0;

    while (!navigator.end())
        CHECK(navigator.navigate_all());

    for (const auto &steps : navigator.steps)
    {
        if (!steps_lcm)
            steps_lcm = steps;
        else
            steps_lcm = std::lcm(steps_lcm, steps);
    }

    return steps_lcm;
}
