#include <debug_print.h>
// #define DTIMER_ON
#include <main.h>
#include <skip.h>

#include <cmath>
#include <functional>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    const char IP = 'I';
    constexpr int MAX_REGISTERS = 'Z' + 1;
    int64_t initial_registers[MAX_REGISTERS] = {};
    std::vector<int> program;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
            break;

        std::stringstream ss(line);
        skip<std::string>(ss);
        char ch;
        ss >> ch;
        skip<char>(ss);
        int64_t value;
        ss >> value;
        initial_registers[ch] = value;
    }

    std::getline(file, line);
    std::stringstream ss(line);
    skip<std::string>(ss);
    int code;

    while (ss >> code)
    {
        program.push_back(code);
        skip<char>(ss);
    }

    const int program_size = program.size();
    DPRINTX_ENDL(line << " size " << program_size);
    int64_t registers[MAX_REGISTERS] = {};
    std::vector<int> output;
    bool verbose_debug_print = false;

    enum Instruction
    {
        adv = 0,
        bxl,
        bst,
        jnz,
        bxc,
        out,
        bdv,
        cdv,
        opcode_size,
    };

    using OperandFunction = std::function<int64_t(void)>;
    using InstructionFunction = std::function<void(int64_t)>;

    OperandFunction get_operand = [&]() -> int64_t
    {
        return program[registers[IP]++];
    };

    OperandFunction get_combo_operand = [&]() -> int64_t
    {
        int operand = get_operand();

        switch (operand)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        case 4:
            return registers['A'];
        case 5:
            return registers['B'];
        case 6:
            return registers['C'];
        default:
            DCHECK(operand < 7);
        }

        return operand;
    };

    InstructionFunction adv_function = [&](int64_t operand) -> void
    {
        auto numerator = registers['A'];
        auto denominator = std::pow(2, operand);
        DCHECK(denominator != 0);
        registers['A'] = numerator / denominator;

        if (verbose_debug_print)
            DPRINTX(numerator << "/" << denominator << "=" << registers['A']);
    };

    InstructionFunction bxl_function = [&](int64_t operand) -> void
    {
        auto left = registers['B'];
        auto right = operand;
        registers['B'] = left ^ right;

        if (verbose_debug_print)
            DPRINTX(left << "^" << right << "=" << registers['B']);
    };

    InstructionFunction bst_function = [&](int64_t operand) -> void
    {
        registers['B'] = operand % 8;

        if (verbose_debug_print)
            DPRINTX(operand << "%8=" << registers['B']);
    };

    InstructionFunction jnz_function = [&](int64_t operand) -> void
    {
        if (registers['A'] != 0)
            registers[IP] = operand;

        if (verbose_debug_print)
            DPRINTX("IP=" << registers[IP]);
    };

    InstructionFunction bxc_function = [&](int64_t operand) -> void
    {
        auto left = registers['B'];
        auto right = registers['C'];
        registers['B'] = left ^ right;

        if (verbose_debug_print)
            DPRINTX(left << "^" << right << "=" << registers['B']);
    };

    InstructionFunction out_function = [&](int64_t operand) -> void
    {
        auto value = operand % 8;
        output.push_back(value);

        if (verbose_debug_print)
            DPRINTX(operand << "%8=" << value);
    };

    InstructionFunction bdv_function = [&](int64_t operand) -> void
    {
        auto numerator = registers['A'];
        auto denominator = std::pow(2, operand);
        DCHECK(denominator != 0);
        registers['B'] = numerator / denominator;

        if (verbose_debug_print)
            DPRINTX(numerator << "/" << denominator << "=" << registers['B']);
    };

    InstructionFunction cdv_function = [&](int64_t operand) -> void
    {
        auto numerator = registers['A'];
        auto denominator = std::pow(2, operand);
        DCHECK(denominator != 0);
        registers['C'] = numerator / denominator;

        if (verbose_debug_print)
            DPRINTX(numerator << "/" << denominator << "=" << registers['C']);
    };

    std::pair<InstructionFunction, OperandFunction>
        instructions[opcode_size] = {
            {adv_function, get_combo_operand},
            {bxl_function, get_operand},
            {bst_function, get_combo_operand},
            {jnz_function, get_operand},
            {bxc_function, get_operand},
            {out_function, get_combo_operand},
            {bdv_function, get_combo_operand},
            {cdv_function, get_combo_operand},
        };

    std::string instruction_name[opcode_size] =
        {"adv", "bxl", "bst", "jnz", "bxc", "out", "bdv", "cdv"};

    std::function<int(void)> get_match = [&]() -> int
    {
        int match = 0;
        for (int i = 0; i < output.size(); i++)
            if (output[i] == program[i])
                match++;
        return match;
    };

    std::function<int(int64_t)> get_output = [&](int64_t register_a) -> int
    {
        registers[IP] = 0;
        registers['A'] = register_a;
        registers['B'] = initial_registers['B'];
        registers['C'] = initial_registers['C'];
        output.clear();

        auto get_opcode = [&]() -> Instruction
        {
            auto opcode = static_cast<Instruction>(program[registers[IP]++]);
            DCHECK(opcode < opcode_size);
            return opcode;
        };

        auto print_registers = [&]() -> std::string
        {
            std::stringstream ss;
            ss << "IP: " << registers['I'] << ", "
               << "A: " << registers['A'] << ", "
               << "B: " << registers['B'] << ", "
               << "C: " << registers['C'];
            return ss.str();
        };

        while (registers[IP] < program.size())
        {
            auto opcode = get_opcode();
            auto &functions = instructions[opcode];

            if (registers[IP] < program.size())
            {
                if (verbose_debug_print)
                    DPRINTX(instruction_name[opcode] << ": ");

                auto operand = functions.second();
                functions.first(operand);

                if (verbose_debug_print)
                {
                    DPRINT_ENDL();
                    DPRINTX_ENDL(print_registers());
                }
            }

            for (auto it : registers)
                DCHECK(it >= 0);
            for (auto it : output)
                DCHECK(it >= 0);
        }

        return get_match();
    };

    auto print_output = [&](int64_t register_a) -> std::string
    {
        int match = get_output(register_a);
        std::stringstream ss;
        ss << std::dec << register_a << ": ";
        ss << std::oct << register_a << ": ";

        for (auto it = output.begin(); it != output.end();)
        {
            ss << *it;
            if (++it != output.end())
                ss << ',';
        }

        ss << std::dec << " size " << output.size() << ", match " << match;
        return ss.str();
    };

    auto find_start = [&](int64_t register_a_right, int size) -> int64_t
    {
        int64_t register_a_left = 0;

        while (output.size() < size)
        {
            register_a_left = register_a_right;
            register_a_right *= 8;
            get_output(register_a_right);
        }

        while (register_a_left < register_a_right)
        {
            auto register_a_half = (register_a_right + register_a_left) / 2;
            if (register_a_half == register_a_left)
                break;

            get_output(register_a_half);
            if (output.size() < size)
                register_a_left = register_a_half;
            else
                register_a_right = register_a_half;
        }

        return register_a_right;
    };

    int64_t register_a_start = find_start(initial_registers['A'], program_size);
    int64_t register_a_end = find_start(register_a_start, program_size + 1) - 1;

    DPRINTX_ENDL(print_output(register_a_start));
    DPRINTX_ENDL(print_output(register_a_end));

    auto find_match = [&](int64_t register_a_left,
                          int64_t register_a_right) -> int64_t
    {
        while (register_a_left < register_a_right)
        {
            auto register_a_half = (register_a_right + register_a_left) / 2;
            if (register_a_half == register_a_left)
                break;

            int match_left = get_output(register_a_left);
            int match_right = get_output(register_a_right);

            if (match_left >= match_right)
                register_a_right = register_a_half;
            else
                register_a_left = register_a_half;
        }

        return register_a_left;
    };

    std::priority_queue<std::pair<int, int64_t>> match_queue;

    while (register_a_start < register_a_end)
    {
        auto register_a_half = (register_a_end + register_a_start) / 2;
        if (register_a_half == register_a_start)
            break;

        auto register_a_match = find_match(register_a_start, register_a_end);
        match_queue.push({get_output(register_a_match), register_a_match});

        if (register_a_match <= register_a_half)
        {
            if (register_a_start == register_a_match)
                break;
            register_a_start = register_a_match;
        }
        else
        {
            if (register_a_end == register_a_match)
                break;
            register_a_end = register_a_match;
        }
    }

    while (!match_queue.empty())
    {
        auto top_match = match_queue.top();
        match_queue.pop();
        DPRINTX_ENDL(print_output(top_match.second));
    }

    // NOTE:                     Program: 2,4,1,5,7,5,1,6,4,1,5,5,0,3,3,0 size 16
    // 281001341439745: 7771067110647401: 2,4,1,5,5,5,1,6,1,1,5,5,5,3,3,5 size 16, match 12
    // 271032038257417: 7550022027171411: 2,4,1,5,7,1,1,0,7,1,5,1,0,3,3,5 size 16, match 11
    // 278777847019265: 7730600642573401: 2,4,1,5,7,3,2,5,5,7,2,5,5,7,3,5 size 16, match 7
    //   7745808761848 = 278777847019265 - 271032038257417

    return {};
}
