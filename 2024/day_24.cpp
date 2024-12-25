// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <bitset>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    const char name_chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    constexpr int max_chars = sizeof(name_chars) / sizeof(char);
    const std::string name_chars_str(name_chars);
    std::string line;

    using Value = std::optional<short>;

    struct Wire
    {
        Value value;
    };

    struct Gate
    {
        std::string name;
        Value output;

        Gate() = default;
        Gate(std::string n) : name(n) {}
        virtual ~Gate() = default;

        Value simulate(const Wire &left, const Wire &right)
        {
            if (!output.has_value() &&
                left.value.has_value() && right.value.has_value())
            {
                set_outpput(left, right);
            }
            return output;
        }

        virtual void set_outpput(const Wire &left, const Wire &right) = 0;
    };

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

    struct Connection
    {
        std::string left;
        std::string right;
        std::unique_ptr<Gate> gate;
        std::string output;
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

    Wire wire_map[max_chars][max_chars][max_chars] = {};
    std::map<std::string, bool> all_z_wire_names;
    std::vector<std::string> all_wire_names;
    std::vector<Connection> all_connections;
    std::vector<Gate *> all_gates;

    auto use_wire = [&](const std::string &name) -> Wire &
    {
        DCHECK(name.size() == 3);

        auto idx = [&](char ch) -> int
        {
            auto it = name_chars_str.find(ch);
            DCHECK(it != std::string::npos);
            return it;
        };

        if (name[0] == 'z')
            all_z_wire_names[name] = false;

        if (std::find(all_wire_names.begin(), all_wire_names.end(),
                      name) == all_wire_names.end())
            all_wire_names.push_back(name);

        return wire_map[idx(name[0])][idx(name[1])][idx(name[2])];
    };

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

    auto z_wires_with_value = [&]() -> size_t
    {
        size_t has_value = 0;

        for (const auto &z_wire : all_z_wire_names)
            if (use_wire(z_wire.first).value.has_value())
                has_value++;

        return has_value;
    };

#ifdef DPRINT_ON
    auto debug_print = [&](bool verbose = false,
                           bool initial = false) -> std::string
    {
        if (initial)
        {
            for (const auto &c : all_connections)
            {
                DPRINTX_ENDL(c.left << " " << c.gate->name << " " << c.right
                                    << " -> " << c.output);
            }
        }
        for (const auto &wire : all_wire_names)
        {
            if (initial || verbose || wire[0] == 'z')
            {
                DPRINTX_ENDL(wire << ": "
                                  << use_wire(wire).value.value_or(false));
            }
        }
        for (const auto *g : all_gates)
        {
            DPRINTX(g->name << ":" << g->output.has_value() << " ");
        }
        DPRINT_ENDL();
        DPRINT2(z_wires_with_value(), all_z_wire_names.size());
        return {};
    };
    DPRINTX_ENDL(debug_print(true));
#endif

    while (!all_z_wire_names.size() ||
           z_wires_with_value() < all_z_wire_names.size())
    {
        for (const auto &connection : all_connections)
        {
            use_wire(connection.output).value = connection.gate->simulate(
                use_wire(connection.left), use_wire(connection.right));
        }

        DPRINTX_ENDL(debug_print());
    }

    std::bitset<64> number;

    for (const auto &z_wire : all_z_wire_names)
    {
        int bit;
        std::stringstream(z_wire.first.substr(1)) >> bit;
        number.set(bit, !!use_wire(z_wire.first).value.value());
    }

    return number.to_ullong();
}
