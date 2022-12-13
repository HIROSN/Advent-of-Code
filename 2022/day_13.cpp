// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <sstream>
#include <utility>
#include <vector>

#define L 0
#define R 1

struct List
{
    std::optional<int> integer;
    std::vector<List> list;

    int compare(List right);
    List listify();
    bool operator<(const List &right);

#ifdef DPRINT_ON
    friend std::ostream &operator<<(std::ostream &out, const List &list);
#endif
};

List make_list(std::istream &stream);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::pair<List, List>> pairs;
    std::string line_left, line_right;

    while (file >> line_left >> line_right)
    {
        std::stringstream ss_left(line_left);
        std::stringstream ss_right(line_right);
        pairs.push_back({make_list(ss_left), make_list(ss_right)});
    }

    int sum_of_indices = 0;
    int index = 0;
    for (const auto &pair : pairs)
    {
        index++;
        auto packet_left = std::get<L>(pair);
        auto packet_right = std::get<R>(pair);
        if (packet_left < packet_right)
            sum_of_indices += index;

#ifdef DPRINT_ON
        std::cout << "== Pair " << index << " ==" << std::endl
                  << packet_left << std::endl
                  << packet_right << std::endl;
        if (packet_left < packet_right)
            std::cout << "in the right order" << std::endl;
        std::cout << std::endl;
#endif
    }

    return sum_of_indices;
}

int List::compare(List right)
{
    if (integer.has_value() && right.integer.has_value())
    {
        if (integer.value() < right.integer.value())
            return -1;
        if (integer.value() > right.integer.value())
            return 1;
    }
    else if (integer.has_value())
    {
        return listify().compare(right);
    }
    else if (right.integer.has_value())
    {
        return compare(right.listify());
    }
    else
    {
        for (int i = 0; i < list.size() && i < right.list.size(); i++)
        {
            int result = list[i].compare(right.list[i]);
            if (result)
                return result;
        }
        if (list.size() < right.list.size())
            return -1;
        if (list.size() > right.list.size())
            return 1;
    }
    return 0;
}

List List::listify()
{
    if (integer.has_value())
    {
        list.push_back(*this);
        integer.reset();
        return *this;
    }
    return {};
}

bool List::operator<(const List &right)
{
    return compare(right) < 0;
}

List make_list(std::istream &stream)
{
    List list;

    if (stream.peek() == ',')
        skip<char>(stream);

    if (stream.peek() == '[')
    {
        skip<char>(stream);
        while (stream.peek() != ']')
            list.list.push_back(make_list(stream));
        skip<char>(stream);
    }
    else
    {
        int value = 0;
        stream >> value;
        list.integer = value;
    }

    return list;
}

#ifdef DPRINT_ON
std::ostream &operator<<(std::ostream &out, const List &list)
{
    if (list.integer.has_value())
    {
        out << list.integer.value();
    }
    else
    {
        out << "[";
        for (int i = 0; i < list.list.size(); i++)
        {
            if (i)
                out << ",";
            out << list.list[i];
        }
        out << "]";
    }
    return out;
}
#endif
