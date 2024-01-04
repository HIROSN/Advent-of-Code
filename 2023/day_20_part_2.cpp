// #define PART_1
// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <algorithm>
#include <cctype>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

using Number = int;

Number high_pulses = 0;
Number low_pulses = 0;
Number number_of_button_presses = 0;
std::vector<Number> lo_to_ff_or_hi_to_cj[26][27];

std::vector<Number> &sent_pulse(const std::string &name)
{
    return lo_to_ff_or_hi_to_cj[name[0] - 'a']
                               [name.size() > 1 ? name[1] - 'a' : 0];
}

enum Pulse
{
    NoPulse,
    LowPulse,
    HighPulse,
};

struct Module
{
    std::string name;
    char prefix = 0;
    std::vector<Module *> destinations;
    std::vector<Module *> senders;
    std::queue<std::pair<Pulse, std::string>> pulse_queue;
    Pulse single_pulse = NoPulse;
    std::pair<std::string, std::vector<Module *>> path_info;

    Module() = default;
    Module(const std::string &str, char ch = 0) : name(str), prefix(ch) {}

    void receive(Pulse pulse, Module *sender_module)
    {
        pulse_queue.push({pulse, sender_module->name});
        single_pulse = pulse;
        path_info = sender_module->path_info;
        if (prefix)
        {
            path_info.first += prefix + name;
            path_info.second.push_back(this);
        }
    }

    std::vector<Module *> send()
    {
        std::vector<Module *> received;
        while (!pulse_queue.empty())
        {
            const auto &queued = pulse_queue.front();
            process_queue(queued.first, queued.second);
            auto r = process_send(queued.first);
            received.insert(received.end(), r.begin(), r.end());
            pulse_queue.pop();
        }
        return received;
    }

    void send_pulse(Pulse pulse, Module *sender_module)
    {
        for (auto &module : destinations)
        {
            switch (pulse)
            {
            case HighPulse:
                high_pulses++;
                // DPRINTX_ENDL(name << " -high-> " << module->name);
                if (module->prefix == '&')
                    sent_pulse(name).push_back(number_of_button_presses);
                break;
            case LowPulse:
                low_pulses++;
                // DPRINTX_ENDL(name << " -low-> " << module->name);
                if (module->prefix == '%')
                    sent_pulse(name).push_back(number_of_button_presses);
                break;
            }

            module->receive(pulse, sender_module);
        }
    }

    virtual void process_queue(Pulse pulse, const std::string &sender) {}

    virtual std::vector<Module *> process_send(Pulse pulse)
    {
        send_pulse(pulse, this);
        return destinations;
    }

    friend std::ostream &operator<<(std::ostream &out, const Module &module);
};

struct Flipflop : public Module
{
    bool is_on = false;

    Flipflop(const std::string &str) : Module(str, '%') {}

    void process_queue(Pulse pulse, const std::string &sender) override
    {
        if (pulse == LowPulse)
        {
            is_on = !is_on;
        }
    }

    std::vector<Module *> process_send(Pulse pulse) override
    {
        if (pulse == LowPulse)
        {
            send_pulse(is_on ? HighPulse : LowPulse, this);
            return destinations;
        }
        return {};
    }
};

struct Conjunction : public Module
{
    std::map<std::string, Pulse> sender_pulse;
    std::optional<int> first_low_pulse;

    Conjunction(const std::string &str) : Module(str, '&') {}

    bool high_for_all_inputs() const
    {
        for (auto it : sender_pulse)
        {
            if (it.second == LowPulse)
                return false;
        }
        return sender_pulse.size();
    }

    void process_queue(Pulse pulse, const std::string &sender) override
    {
        if (!sender_pulse.size())
        {
            for (auto *known_sender : senders)
                sender_pulse[known_sender->name] = LowPulse;
        }
        sender_pulse[sender] = pulse;
    }

    std::vector<Module *> process_send(Pulse pulse) override
    {
        if (first_low_pulse.has_value() && !first_low_pulse.value() &&
            high_for_all_inputs())
        {
            first_low_pulse = number_of_button_presses;
            DPRINTX(*this);
        }
        send_pulse(high_for_all_inputs() ? LowPulse : HighPulse, this);
        return destinations;
    }
};

struct Button : public Module
{
    Button(Module *module) : Module("button")
    {
        destinations.push_back(module);
    }

    void push()
    {
        ++number_of_button_presses;
        receive(LowPulse, this);
        std::vector<Module *> sending{this};
        std::vector<Module *> receiving;

        while (true)
        {
            for (auto *module : sending)
            {
                for (const auto &module : module->send())
                    receiving.push_back(module);
            }

            if (!receiving.empty())
            {
                sending = receiving;
                receiving.clear();
                continue;
            }
            break;
        }
    }
};

struct FinalMachine
{
    std::unique_ptr<Module> module_map[26][27];
    std::vector<Module *> module_list;
    Module broadcast{"broadcaster"};
    Button button{&broadcast};
    Module *rx;
    std::map<std::string, int> paths;

    FinalMachine(const std::vector<std::string> &input)
    {
        auto &module = get_module("rx");
        module = std::make_unique<Module>("rx");
        rx = module.get();

        for (const auto &line : input)
            make_modules(line);

        for (const auto &line : input)
            connect_modules(line);
    }

    void push_button()
    {
        rx->single_pulse = NoPulse;
        button.path_info = {};
        button.push();
        paths[rx->path_info.first]++;
    }

    std::unique_ptr<Module> &get_module(const std::string &name)
    {
        return module_map[name[0] - 'a']
                         [name.size() > 1 ? name[1] - 'a' : 0];
    }

    void trim_spaces(std::string &str)
    {
        str.erase(std::remove_if(str.begin(), str.end(),
                                 [](unsigned char x)
                                 { return std::isspace(x); }),
                  str.end());
    }

    void read_name(std::stringstream &ss,
                   std::string &name, char &prefix)
    {
        ss >> name;
        skip<std::string>(ss);

        if (name != broadcast.name)
        {
            prefix = name[0];
            name = name.substr(1);
        }
    }

    void make_modules(const std::string &line)
    {
        std::stringstream ss(line);
        std::string name;
        char prefix;

        read_name(ss, name, prefix);
        if (name == broadcast.name)
            return;

        auto &module = get_module(name);
        switch (prefix)
        {
        case '%':
            module = std::make_unique<Flipflop>(name);
            break;
        case '&':
            module = std::make_unique<Conjunction>(name);
            break;
        }
    }

    void connect_modules(const std::string &line)
    {
        std::stringstream ss(line);
        std::string name, destination;
        char prefix;
        Module *module;

        read_name(ss, name, prefix);
        if (name == broadcast.name)
            module = &broadcast;
        else
            module = get_module(name).get();

        while (std::getline(ss, destination, ','))
        {
            trim_spaces(destination);
            auto &dest_module = get_module(destination);
            if (!dest_module)
                dest_module = std::make_unique<Module>(destination);
            module->destinations.push_back(dest_module.get());
            dest_module->senders.push_back(module);
        }

        module_list.push_back(module);
        // DPRINTX_ENDL(*module);
    }

    friend std::ostream &operator<<(std::ostream &out,
                                    const FinalMachine &machine);
};

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::string> input;
    std::string line;

    while (std::getline(file, line))
        input.push_back(line);

    FinalMachine machine(input);
#ifdef PART_1
    for (int i = 0; i < 1000; i++)
        machine.push_button();

    return low_pulses * high_pulses;
#else
    std::vector<Conjunction *> conjunctions;
    CHECK(machine.rx->senders.size() == 1);

    for (auto *module : machine.rx->senders[0]->senders)
    {
        DCHECK(module->prefix == '&');
        CHECK(module->senders.size() == 1);
        auto *sender = module->senders[0];
        CHECK(sender->prefix == '&');
        auto *conjunction = reinterpret_cast<Conjunction *>(sender);
        DPRINTX(*conjunction);
        conjunction->first_low_pulse = 0;
        conjunctions.push_back(conjunction);
    }

    uint64_t single_low_pulse_to_rx = 0;

    while (!single_low_pulse_to_rx)
    {
        machine.push_button();

        single_low_pulse_to_rx = 1;
        for (auto *conjunction : conjunctions)
        {
            if (!conjunction->first_low_pulse.value())
            {
                single_low_pulse_to_rx = 0;
                break;
            }
            single_low_pulse_to_rx *= conjunction->first_low_pulse.value();
        }
    }

    return single_low_pulse_to_rx;
#endif
}

std::ostream &operator<<(std::ostream &out, const Module &module)
{
#ifdef DPRINT_ON
    int c = 0;
    out << module.prefix << module.name;
    for (const auto &destination : module.destinations)
        out << (c++ ? ", " : " {") << destination->name;
    out << (c ? "}" : "");
    if (module.senders.size())
    {
        c = 0;
        for (const auto &sender : module.senders)
            out << (c++ ? ", " : " [") << sender->prefix << sender->name;
        out << (c ? "]" : "");
    }
    out << " ";
    if (module.prefix == '%')
    {
        const Flipflop &ff = reinterpret_cast<const Flipflop &>(module);
        out << (ff.is_on ? "(on)" : "(OFF)");
    }
    if (module.prefix == '&')
    {
        const Conjunction &cj = reinterpret_cast<const Conjunction &>(module);
        out << (cj.high_for_all_inputs() ? "(HIGH)" : "(low)");
        if (cj.first_low_pulse.has_value())
            out << " " << cj.first_low_pulse.value();
    }
    out << std::endl;
#endif
    return out;
}

std::ostream &operator<<(std::ostream &out, const FinalMachine &machine)
{
#ifdef DPRINT_ON
    for (auto it : machine.paths)
        if (it.first.size())
            out << it.first << " - " << it.second << std::endl;
#endif // DPRINT_ON
    return out;
}
