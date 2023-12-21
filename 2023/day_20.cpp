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

enum Pulse
{
    LowPulse,
    HighPulse,
};

struct Module
{
    std::string name;
    std::vector<Module *> destinations;
    std::vector<Module *> senders;
    std::queue<std::pair<Pulse, std::string>> pulse_queue;

    static Number high_pulses;
    static Number low_pulses;

    Module() = default;
    Module(const std::string &str) : name(str) {}

    void receive(Pulse pulse, const std::string &sender)
    {
        pulse_queue.push({pulse, sender});
    }

    std::vector<Module *> send()
    {
        std::vector<Module *> received;
        while (!pulse_queue.empty())
        {
            const auto &queued = pulse_queue.front();
            process_queue(queued.first, queued.second);
            auto r = process_send(queued.first, queued.second);
            received.insert(received.end(), r.begin(), r.end());
            pulse_queue.pop();
        }
        return received;
    }

    void send_pulse(Pulse pulse, const std::string &sender)
    {
        for (auto &module : destinations)
        {
            switch (pulse)
            {
            case HighPulse:
                high_pulses++;
                DPRINTX_ENDL(name << " -high-> " << module->name);
                break;
            case LowPulse:
                low_pulses++;
                DPRINTX_ENDL(name << " -low-> " << module->name);
                break;
            }

            module->receive(pulse, sender);
        }
    }

    virtual void process_queue(Pulse pulse, const std::string &sender) {}

    virtual std::vector<Module *> process_send(Pulse pulse,
                                               const std::string &sender)
    {
        send_pulse(pulse, name);
        return destinations;
    }

    friend std::ostream &operator<<(std::ostream &out, const Module &module);
};

Number Module::high_pulses = 0;
Number Module::low_pulses = 0;

std::optional<uint64_t> Answer(std::ifstream &file)
{
    struct Flipflop : public Module
    {
        bool is_on = false;

        Flipflop(const std::string &str) : Module(str) {}

        void process_queue(Pulse pulse, const std::string &sender) override
        {
            if (pulse == LowPulse)
            {
                is_on = !is_on;
            }
        }

        std::vector<Module *> process_send(Pulse pulse,
                                           const std::string &sender) override
        {
            if (pulse == LowPulse)
            {
                send_pulse(is_on ? HighPulse : LowPulse, name);
                return destinations;
            }
            return {};
        }
    };

    struct Conjunction : public Module
    {
        std::map<std::string, Pulse> sender_pulse;

        Conjunction(const std::string &str) : Module(str) {}

        void process_queue(Pulse pulse, const std::string &sender) override
        {
            if (!sender_pulse.size())
            {
                for (auto *known_sender : senders)
                    sender_pulse[known_sender->name] = LowPulse;
            }
            sender_pulse[sender] = pulse;
        }

        std::vector<Module *> process_send(Pulse pulse,
                                           const std::string &sender) override
        {
            bool high_for_all_inputs = true;
            for (auto it : sender_pulse)
            {
                if (it.second == LowPulse)
                {
                    high_for_all_inputs = false;
                    break;
                }
            }
            send_pulse(high_for_all_inputs ? LowPulse : HighPulse, name);
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
            receive(LowPulse, name);
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

            DPRINT2(low_pulses, high_pulses);
            DPRINT_ENDL();
        }
    };

    auto get_module = [](const std::string &name) -> std::unique_ptr<Module> &
    {
        static std::unique_ptr<Module> modules[26][27];
        return modules[name[0] - 'a'][name.size() > 1 ? name[1] - 'a' : 0];
    };

    auto trim_spaces = [](std::string &str) -> void
    {
        str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char x)
                                 { return std::isspace(x); }),
                  str.end());
    };

    Module broadcast("broadcaster");

    auto read_input = [&](const std::string &line,
                          bool make_destinations) -> void
    {
        std::stringstream ss(line);
        std::string name, destination;
        char prefix;
        ss >> name;
        skip<std::string>(ss);

        if (name != broadcast.name)
        {
            prefix = name[0];
            name = name.substr(1);
        }

        if (make_destinations)
        {
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
        else
        {
            Module *module;
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
            // std::cout << *module;
        }
    };

    std::vector<std::string> input;
    std::string str;

    while (std::getline(file, str))
        input.push_back(str);

    for (const auto &line : input)
        read_input(line, true);

    for (const auto &line : input)
        read_input(line, false);

    Button button(&broadcast);
    // std::cout << button;
    // std::cout << broadcast;

    for (int i = 0; i < 1000; i++)
        button.push();

    return button.low_pulses * button.high_pulses;
}

std::ostream &operator<<(std::ostream &out, const Module &module)
{
#ifdef DPRINT_ON
    int c = 0;
    out << module.name;
    for (const auto &destination : module.destinations)
        out << (c++ ? ", " : " {") << destination->name;
    out << (c ? "}" : "");
    if (module.senders.size())
    {
        c = 0;
        for (const auto &sender : module.senders)
            out << (c++ ? ", " : " [") << sender->name;
        out << (c ? "]" : "");
    }
    out << std::endl;
#endif
    return out;
}
