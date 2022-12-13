// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <queue>
#include <sstream>
#include <vector>

struct List
{
    std::optional<int> integer;
    std::vector<List> list;

    int compare(List right) const;
    List listify() const;
    bool operator>(const List &right);
    bool operator==(const List &right) const;

#ifdef DPRINT_ON
    friend std::ostream &operator<<(std::ostream &out, const List &list);
#endif
};

class Compare
{
public:
    bool operator()(List left, List right)
    {
        return left > right;
    }
};

List make_list(const std::string &line);
List make_list(std::istream &stream);

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::priority_queue<List, std::vector<List>, Compare> packets;

    const List divider1 = make_list("[[2]]");
    const List divider2 = make_list("[[6]]");
    packets.push(divider1);
    packets.push(divider2);

    std::string line;
    while (file >> line)
    {
        packets.push(make_list(line));
    }

    int index = 0;
    int decoder_key = 1;
    while (!packets.empty())
    {
        index++;
        const auto &packet = packets.top();
        if (packet == divider1 || packet == divider2)
            decoder_key *= index;

#ifdef DPRINT_ON
        std::cout << packet << std::endl;
#endif
        packets.pop();
    }

    return decoder_key;
}

int List::compare(List right) const
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

List List::listify() const
{
    List copy = *this;
    if (copy.integer.has_value())
    {
        copy.list.push_back(copy);
        copy.integer.reset();
        return copy;
    }
    return {};
}

bool List::operator>(const List &right)
{
    return compare(right) > 0;
}

bool List::operator==(const List &right) const
{
    return compare(right) == 0;
}

List make_list(const std::string &line)
{
    std::stringstream ss(line);
    return make_list(ss);
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
