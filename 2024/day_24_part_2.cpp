// #include <debug_print.h>
// #define DTIMER_ON
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <bitset>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

// #define PART_1

#define ManualSwap(ms, n1, n2)                \
    Swapped ms(connection_by_output(n1),      \
               connection_by_output(n2),      \
               &all_connections);             \
    names_queue.push(ms.gates.first->output); \
    names_queue.push(ms.gates.second->output)

using Value = std::optional<short>;

struct Wire
{
    Value value;
    int tick = 0;

    void reset()
    {
        value.reset();
        tick = 0;
    }
};

struct Gate
{
    std::string name;
    Value output;

    Gate() = default;
    Gate(std::string n) : name(n) {}
    virtual ~Gate() = default;

    virtual void set_outpput(const Wire &left, const Wire &right) = 0;

    void reset()
    {
        output.reset();
    }
};

struct Connection
{
    std::string left;
    std::string right;
    std::unique_ptr<Gate> gate;
    std::string output;
};

const char name_chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
constexpr int max_chars = sizeof(name_chars) / sizeof(char);
Wire wire_map[max_chars][max_chars][max_chars] = {};
const std::string name_chars_str(name_chars);
std::vector<std::string> all_wire_names;

int idx(char ch)
{
    auto it = name_chars_str.find(ch);
    DCHECK(it != std::string::npos);
    return it;
}

Wire &use_wire(const std::string &name)
{
    DCHECK(name.size() == 3);

    if (std::find(all_wire_names.begin(), all_wire_names.end(),
                  name) == all_wire_names.end())
        all_wire_names.push_back(name);

    return wire_map[idx(name[0])][idx(name[1])][idx(name[2])];
}

std::ostream &operator<<(std::ostream &out, const Connection &c)
{
    auto c_left = c.left;
    auto c_right = c.right;
    if (c_left > c_right)
        std::swap(c_left, c_right);
    const auto &l = use_wire(c_left).value;
    const auto &r = use_wire(c_right).value;
    const auto &g = c.gate->output;
    const auto &o = use_wire(c.output);

    out << "{" << c_left;
    if (l.has_value())
        out << ":" << l.value();
    out << " " << c.gate->name << " " << c_right;
    if (r.has_value())
        out << ":" << r.value();
    out << " -> " << c.output;
    if (o.value.has_value())
        out << ":" << o.value.value();
    out << " @" << o.tick << "}";

    return out;
}

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;

    struct WireName
    {
        char ch[3] = {};

        WireName() = default;
        WireName(char c1, char c2, char c3) : ch{c1, c2, c3} {}

        std::string str()
        {
            return {ch[0], ch[1], ch[2]};
        }
    };

    struct AND : public Gate
    {
        AND() : Gate("AND") {}

        void set_outpput(const Wire &left, const Wire &right) override
        {
            output = left.value.value() && right.value.value();
        }
    };

    struct OR : public Gate
    {
        OR() : Gate("OR") {}

        void set_outpput(const Wire &left, const Wire &right) override
        {
            output = left.value.value() || right.value.value();
        }
    };

    struct XOR : public Gate
    {
        XOR() : Gate("XOR") {}

        void set_outpput(const Wire &left, const Wire &right) override
        {
            output = left.value.value() != right.value.value();
        }
    };

    std::vector<Connection> all_connections;
    std::vector<Gate *> all_gates;
    std::map<std::string, Value> wire_start_value;
    int clock = 0;

    while (std::getline(file, line))
    {
        if (line.empty())
            break;
        std::stringstream ss(line);
        char c1, c2, c3;
        ss >> c1 >> c2 >> c3;
        skip<char>(ss);
        int v;
        ss >> v;
        auto name = WireName(c1, c2, c3).str();
        use_wire(name).value = v;
        wire_start_value[name] = v;
    }

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        Connection c;
        std::string g, x;
        ss >> c.left >> g >> c.right >> x >> c.output;
        switch (g[0])
        {
        case 'A':
            c.gate = std::make_unique<AND>();
            break;
        case 'O':
            c.gate = std::make_unique<OR>();
            break;
        case 'X':
            c.gate = std::make_unique<XOR>();
            break;
        }
        all_gates.push_back(c.gate.get());
        all_connections.push_back(std::move(c));
    }

    auto number = [&](char xyz) -> int64_t
    {
        std::bitset<64> number;

        for (int bit = 0; bit < 64; bit++)
        {
            const auto &value = wire_map[idx(xyz)][bit / 10][bit % 10].value;
            number.set(bit, value.has_value() && value.value());
        }

        return number.to_ullong();
    };

#ifdef DPRINT_ON
    auto debug_print = [&](bool verbose = false) -> std::string
    {
        auto a = number('x') + number('y');
        auto z = number('z');
        if (verbose)
        {
            DPRINTX_ENDL("x: " << std::bitset<64>(number('x')));
            DPRINTX_ENDL("y: " << std::bitset<64>(number('y')));
            DPRINTX_ENDL("+: " << std::bitset<64>(a));
        }
        DPRINTX_ENDL("z: " << std::bitset<64>(z));
        DPRINTX_ENDL("e: " << std::bitset<64>(a ^ z));
        return {};
    };
#endif

    auto gates_finished = [&]() -> int
    {
        int finished = 0;
        for (const auto *gate : all_gates)
            if (gate->output.has_value())
                finished++;
        return finished;
    };

    auto reset = [&]() -> void
    {
        for (const auto &wire : all_wire_names)
            use_wire(wire).reset();

        for (auto it : wire_start_value)
            use_wire(it.first).value = it.second;

        for (auto *gate : all_gates)
            gate->reset();

        clock = 0;
    };

    auto addition_system = [&](bool return_error = false) -> std::bitset<64>
    {
        reset();

        while (gates_finished() < all_gates.size())
        {
            clock++;

            for (const auto &connection : all_connections)
            {
                const Wire &left = use_wire(connection.left);
                const Wire &right = use_wire(connection.right);
                Wire &output = use_wire(connection.output);

                if (!output.value.has_value() &&
                    left.value.has_value() && right.value.has_value())
                {
                    connection.gate->set_outpput(left, right);
                    output.tick = clock;
                }

                output.value = connection.gate->output;
            }
        }

        if (return_error)
        {
            auto a = number('x') + number('y');
            auto z = number('z');
            return a ^ z;
        }

        return number('x') + number('y');
    };

#ifdef PART_1
    addition_system();
    return number('z');
#else

    using SwapPair = std::pair<Connection *, Connection *>;

    struct Swapped
    {
        SwapPair gates = {};
        std::vector<Connection> *all_connections;

        Swapped() = default;

        Swapped(Connection *c1, Connection *c2,
                std::vector<Connection> *ac)
            : gates({c1, c2}), all_connections(ac)
        {
            if (gates.first->output < gates.second->output)
                std::swap(gates.first, gates.second);
            Swap();
        }

        Swapped(const SwapPair &swap_pair,
                std::vector<Connection> *ac)
            : Swapped(swap_pair.first, swap_pair.second, ac) {}

        ~Swapped()
        {
            Swap();
        }

        void Swap()
        {
            if (gates.first && gates.second)
            {
                auto first = gates.first->output;
                auto second = gates.second->output;
                // rename_output(first, "???");
                // rename_output(second, first);
                // rename_output("???", second);
                std::swap(gates.first->output, gates.second->output);
            }
        }

        void rename_output(const std::string &from, const std::string &to)
        {
            for (auto &connection : *all_connections)
            {
                if (connection.left == from)
                    connection.left = to;
                if (connection.right == from)
                    connection.right = to;
                if (connection.output == from)
                    connection.output = to;
            }
        }
    };

    auto connection_by_output = [&](const std::string &output,
                                    Connection *c = nullptr)
        -> Connection *
    {
        auto it = all_connections.begin();

        if (c)
        {
            while (it != all_connections.end())
            {
                const auto &connection = *it;

                if (connection.output == output)
                {
                    if (&connection == c)
                    {
                        it++;
                        break;
                    }
                }
                it++;
            }
        }

        while (it != all_connections.end())
        {
            auto &connection = *it;
            if (connection.output == output)
                return &connection;
            it++;
        }

        return nullptr;
    };

    auto connection_by_input = [&](const std::string &input,
                                   Connection *c = nullptr)
        -> Connection *
    {
        auto it = all_connections.begin();

        if (c)
        {
            while (it != all_connections.end())
            {
                const auto &connection = *it;
                if (connection.left == input || connection.right == input)
                {
                    if (&connection == c)
                    {
                        it++;
                        break;
                    }
                }
                it++;
            }
        }

        while (it != all_connections.end())
        {
            auto &connection = *it;
            if (connection.left == input || connection.right == input)
                return &connection;
            it++;
        }

        return nullptr;
    };

    auto msb = [](const std::bitset<64> &bits) -> int
    {
        for (int i = bits.size() - 1; i >= 0; i--)
            if (bits.test(i))
                return i;
        return 0;
    };

    auto combination = [](int64_t s, int n) -> int64_t
    {
        int c = s;
        for (int i = 1; i < n; i++)
            c *= --s;
        return c;
    };

    auto result_bits = addition_system();
    DPRINTX_ENDL("result " << msb(result_bits) << " bits");
    DPRINTX(debug_print(true));
    std::map<int, std::map<std::string, int>> success_outputs_per_bit;
    std::map<int, std::map<std::string, int>> problem_outputs_per_bit;

    std::function<bool(int, const std::string &, const std::string &,
                       std::vector<Connection *> &,
                       std::vector<Connection *> &)>
        walk_from_xy = [&](int start_bit,
                           const std::string &name_xy,
                           const std::string &name_z,
                           std::vector<Connection *> &path,
                           std::vector<Connection *> &list) -> bool
    {
        bool found_z = name_xy == name_z;
        auto *xy = connection_by_input(name_xy);

        while (xy && !found_z)
        {
            if (xy->gate->name == "XOR")
            {
                found_z = walk_from_xy(start_bit, xy->output, name_z,
                                       path, list);
                break;
            }

            xy = connection_by_input(name_xy, xy);
        }

        if (xy)
        {
            if (found_z)
            {
                if (std::find(path.begin(), path.end(), xy) == path.end())
                    path.insert(path.begin(), xy);

                success_outputs_per_bit[start_bit][xy->output]++;
            }
            else
            {
                if (std::find(list.begin(), list.end(), xy) == list.end())
                    list.insert(list.begin(), xy);

                problem_outputs_per_bit[start_bit][xy->output]++;
            }
        }

        return found_z;
    };

    std::function<bool(int, int, const std::string &, const std::string &)>
        walk_for_carry_over = [&](int start_bit, int depth,
                                  const std::string &name_xy,
                                  const std::string &name_z) -> bool
    {
        const std::string gate_name[3]{"AND", "OR", "XOR"};
        bool found_z = name_xy == name_z;
        auto *xy = connection_by_input(name_xy);

        while (xy && !found_z && depth < 3)
        {
            if (xy->gate->name == gate_name[depth])
            {
                found_z = walk_for_carry_over(start_bit, depth + 1,
                                              xy->output, name_z);
                break;
            }

            xy = connection_by_input(name_xy, xy);
        }

        if (xy && start_bit > 0 && depth < 3)
        {
            if (found_z)
                success_outputs_per_bit[start_bit][xy->output]++;
            else
                problem_outputs_per_bit[start_bit][xy->output]++;
        }

        return found_z;
    };

    std::function<bool(Connection *, const std::string &, int, int,
                       const std::vector<std::string> &,
                       std::vector<Connection *> &,
                       std::vector<Connection *> &)>
        walk_back_from_z = [&](Connection *z, const std::string &name_xy,
                               int depth, int limit,
                               const std::vector<std::string> &gate_name,
                               std::vector<Connection *> &path,
                               std::vector<Connection *> &list) -> bool
    {
        if (depth >= limit || depth > 0 && z->gate->name != gate_name[depth])
            return false;

        bool found_xy = z->left == name_xy || z->right == name_xy;
        if (depth == 0 && z->gate->name != gate_name[depth])
        {
            found_xy = false;
        }
        else
        {
            auto *left = connection_by_output(z->left);
            auto *right = connection_by_output(z->right);

            while (!found_xy && (left || right))
            {
                if (left)
                {
                    found_xy |= walk_back_from_z(
                        left, name_xy, depth + 1, limit,
                        gate_name, path, list);

                    left = connection_by_output(z->left, left);
                }

                if (right)
                {
                    found_xy |= walk_back_from_z(
                        right, name_xy, depth + 1, limit,
                        gate_name, path, list);
                    right = connection_by_output(z->right, right);
                }
            }
        }

        if (z)
        {
            if (found_xy)
            {
                if (std::find(path.begin(), path.end(), z) == path.end())
                    path.push_back(z);
            }
            else
            {
                if (std::find(list.begin(), list.end(), z) == list.end())
                    list.push_back(z);
            }
        }

        return found_xy;
    };

    auto print_problems = [](const std::vector<Connection *> &list)
        -> std::string
    {
        std::stringstream ss;
        for (auto *c : list)
            ss << *c;
        return ss.str();
    };

    auto print_names = [](const std::list<std::string> &names)
        -> std::string
    {
        std::stringstream ss;
        std::priority_queue<std::string,
                            std::vector<std::string>,
                            std::greater<std::string>>
            queue;

        for (const auto &n : names)
            queue.push(n);

        while (!queue.empty())
        {
            ss << queue.top();
            queue.pop();
            if (!queue.empty())
                ss << ",";
        }

        return ss.str();
    };

    std::bitset<64> problem_bits;
    std::list<std::string> problem_xor_names;
    std::list<std::string> problem_and_names;
    std::list<std::string> problem_or_names;
    std::map<std::string, bool> problem_output_names;

    auto find_problems = [&](int bit_start, int bit_end,
                             bool verbose = false,
                             bool walk_back = false,
                             bool problem_only = false) -> int
    {
        int total_success_outputs = 0;

        std::map<int, std::vector<Connection *>> success_path;
        std::map<int, std::vector<Connection *>> problems;
        std::map<int, std::vector<Connection *>> success_path_back;
        std::map<int, std::vector<Connection *>> problems_back;

        success_outputs_per_bit.clear();
        problem_outputs_per_bit.clear();

        problem_bits.reset();
        problem_xor_names.clear();
        problem_and_names.clear();
        problem_or_names.clear();
        problem_output_names.clear();

        for (int bit = bit_start; bit < bit_end; bit++)
        {
            char high = '0' + bit / 10;
            char low = '0' + bit % 10;
            std::string x{'x', high, low};
            std::string z{'z', high, low};

            high = '0' + (bit - 1) / 10;
            low = '0' + (bit - 1) % 10;
            std::string px{'x', high, low};

            high = '0' + (bit + 1) / 10;
            low = '0' + (bit + 1) % 10;
            std::string nz{'z', high, low};

            if (!walk_from_xy(bit, x, z, success_path[bit], problems[bit]))
                problem_bits.set(bit);

            if (bit == 0)
                continue;

            if (bit + 1 < msb(result_bits))
                if (!walk_for_carry_over(bit, 0, x, nz))
                    problem_bits.set(bit);

            if (bit == 1)
                continue;

            if (!walk_back_from_z(
                    connection_by_output(z), px, 0, 3, {"XOR", "OR", "AND"},
                    success_path_back[bit], problems_back[bit]) /*||
                !walk_back_from_z(
                    connection_by_output(z), x, 0, 2, {"XOR", "XOR"},
                    success_path_back[bit], problems_back[bit])*/
            )
            {
                problem_bits.set(bit);

                for (auto *pc : problems_back[bit])
                    problem_outputs_per_bit[bit][pc->output]++;
            }

            total_success_outputs += success_outputs_per_bit[bit].size();
        }

        if (verbose)
        {
            DPRINTX_ENDL("p: " << problem_bits);

            for (int bit = 0; bit < bit_end; bit++)
            {
                if (problem_bits.test(bit))
                {
                    DPRINTX(bit << " ");
                }
            }
            DPRINTX_ENDL("(" << problem_bits.count() << " bits)");

            for (int bit = bit_start; bit < bit_end; bit++)
            {
                if (!problem_only)
                {
                    DPRINTX(bit << ": valid: ");
                    for (auto it : success_outputs_per_bit[bit])
                    {
                        DPRINTX(it.first << " ");
                    }
                    DPRINT_ENDL();
                }

                if (problem_outputs_per_bit[bit].size())
                {
                    DPRINTX(bit << ": PROBLEM: ");
                    for (auto it : problem_outputs_per_bit[bit])
                    {
                        DPRINTX(it.first << " ");
                    }
                    DPRINT_ENDL();
                }
            }
        }

        if (walk_back)
        {
            for (int bit = bit_start; bit < bit_end; bit++)
            {
                if (problem_only && !problem_bits.test(bit))
                    continue;

                if (success_path_back[bit].size())
                {
                    DPRINTX_ENDL(bit << "< " << print_problems(success_path_back[bit]));
                }
                else if (problems_back[bit].size())
                {
                    DPRINTX_ENDL(bit << "< " << print_problems(problems_back[bit]));
                }

                if (problems[bit].size())
                {
                    DPRINTX_ENDL(bit << "> " << print_problems(problems[bit]));
                }
                else if (success_path[bit].size())
                {
                    DPRINTX_ENDL(bit << "> " << print_problems(success_path[bit]));
                }
            }
        }

        for (int bit = bit_start; bit < bit_end; bit++)
            if (problem_outputs_per_bit[bit].size())
                for (auto it : problem_outputs_per_bit[bit])
                    problem_output_names[it.first] = true;

        for (auto it : problem_output_names)
        {
            const auto &name = it.first;
            auto *output = connection_by_output(name);
            if (output->gate->name == "XOR")
                problem_xor_names.push_back(name);
            else if (output->gate->name == "AND")
                problem_and_names.push_back(name);
            else if (output->gate->name == "OR")
                problem_or_names.push_back(name);
            else
                DCHECK(false);
        }

        return total_success_outputs;
    };

    find_problems(0, msb(result_bits));

    // DPRINTX_ENDL(problem_output_names.size() << " problem output names");

    // DPRINTX_ENDL(problem_xor_names.size()
    //              << " XOR: " << print_names(problem_xor_names));
    // DPRINTX_ENDL(problem_and_names.size()
    //              << " AND: " << print_names(problem_and_names));
    // DPRINTX_ENDL(problem_or_names.size()
    //              << " OR: " << print_names(problem_or_names));

    std::priority_queue<std::string,
                        std::vector<std::string>,
                        std::greater<std::string>>
        names_queue;

/*
    NOTE: Manual adhoc search from here. This is not a general solution.
*/
#ifdef ManualSwap
    ManualSwap(qdg_z12, "qdg", "z12");
    ManualSwap(vvf_z19, "vvf", "z19");
    ManualSwap(dck_fgn, "dck", "fgn"); // bit 23
    ManualSwap(nvh_z37, "nvh", "z37");

    auto error_bits = addition_system(true);
    DPRINTX(debug_print());

    for (int bit = 0; bit < msb(result_bits); bit++)
    {
        if (error_bits.test(bit))
        {
            DPRINTX(bit << " ");
        }
    }

    DPRINTX_ENDL("(" << error_bits.count() << " bits)");
    find_problems(0, msb(result_bits), false, true, false);
#endif

    while (!names_queue.empty())
    {
        std::cout << names_queue.top();
        names_queue.pop();
        if (!names_queue.empty())
            std::cout << ",";
    }

    std::cout << std::endl;
    return {};

#endif
}
