#include <main.h>

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
            return ch[0] == 'Z' && ch[1] == 'Z' && ch[2] == 'Z';
        }

        bool valid() const
        {
            for (int i = 0; i < 3; i++)
            {
                if (ch[i] < 'A' || ch[i] > 'Z')
                    return false;
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
        int index = 0;
        int steps = 0;

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
            }
        }

        bool navigate(Node &node)
        {
            const auto &pair = network[node.idx(0)][node.idx(1)][node.idx(2)];
            node = pair.node[instructions[index]];
            index = ++index % instructions.size();
            steps++;
            return node.valid();
        }
    };

    Navigator navigator(file);
    Node node("AAA");

    while (!node.end())
        CHECK(navigator.navigate(node));

    return navigator.steps;
}
