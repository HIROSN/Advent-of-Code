// #include <debug_print.h>
#include <main.h>
#include <skip.h>

#include <cmath>
#include <functional>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    const std::string IP("IP");
    std::map<std::string, int64_t> registers{{IP, 0}};
    std::vector<int> program;
    std::vector<int> output;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
            break;

        std::stringstream ss(line);
        skip<std::string>(ss);
        char ch;
        ss >> ch;
        std::string str{ch};
        skip<char>(ss);
        int64_t value;
        ss >> value;
        registers[str] = value;
    }

    skip<std::string>(file);
    int code;

    while (file >> code)
    {
        program.push_back(code);
        skip<char>(file);
    }

    auto print_values = [](const std::vector<int> &values) -> std::string
    {
        std::stringstream ss;
        for (auto it = values.begin(); it != values.end();)
        {
            ss << *it;
            if (++it != values.end())
                ss << ',';
        }
        return ss.str();
    };

    auto print_registers = [&]() -> std::string
    {
        std::stringstream ss;
        for (auto it : registers)
            ss << it.first << ":" << it.second << " ";
        return ss.str();
    };

    DPRINTX_ENDL("Program: " << print_values(program));
    DPRINTX_ENDL(print_registers());

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

    auto get_opcode = [&]() -> Instruction
    {
        auto opcode = static_cast<Instruction>(program[registers[IP]++]);
        CHECK(opcode < opcode_size);
        return opcode;
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
            return registers["A"];
        case 5:
            return registers["B"];
        case 6:
            return registers["C"];
        default:
            CHECK(operand < 7);
        }

        return operand;
    };

    InstructionFunction adv_function = [&](int64_t operand) -> void
    {
        auto numerator = registers["A"];
        auto denominator = std::pow(2, operand);
        CHECK(denominator != 0);
        registers["A"] = numerator / denominator;
        DPRINTX(numerator << "/" << denominator << "=" << registers["A"]);
    };

    InstructionFunction bxl_function = [&](int64_t operand) -> void
    {
        auto left = registers["B"];
        auto right = operand;
        registers["B"] = left ^ right;
        DPRINTX(left << "^" << right << "=" << registers["B"]);
    };

    InstructionFunction bst_function = [&](int64_t operand) -> void
    {
        registers["B"] = operand % 8;
        DPRINTX(operand << "%8=" << registers["B"]);
    };

    InstructionFunction jnz_function = [&](int64_t operand) -> void
    {
        if (registers["A"] != 0)
            registers[IP] = operand;
        DPRINTX("IP=" << registers[IP]);
    };

    InstructionFunction bxc_function = [&](int64_t operand) -> void
    {
        auto left = registers["B"];
        auto right = registers["C"];
        registers["B"] = left ^ right;
        DPRINTX(left << "^" << right << "=" << registers["B"]);
    };

    InstructionFunction out_function = [&](int64_t operand) -> void
    {
        auto value = operand % 8;
        output.push_back(value);
        DPRINTX(operand << "%8=" << value);
    };

    InstructionFunction bdv_function = [&](int64_t operand) -> void
    {
        auto numerator = registers["A"];
        auto denominator = std::pow(2, operand);
        CHECK(denominator != 0);
        registers["B"] = numerator / denominator;
        DPRINTX(numerator << "/" << denominator << "=" << registers["B"]);
    };

    InstructionFunction cdv_function = [&](int64_t operand) -> void
    {
        auto numerator = registers["A"];
        auto denominator = std::pow(2, operand);
        CHECK(denominator != 0);
        registers["C"] = numerator / denominator;
        DPRINTX(numerator << "/" << denominator << "=" << registers["C"]);
    };

    std::map<Instruction, std::pair<InstructionFunction, OperandFunction>>
        instructions{
            {adv, {adv_function, get_combo_operand}},
            {bxl, {bxl_function, get_operand}},
            {bst, {bst_function, get_combo_operand}},
            {jnz, {jnz_function, get_operand}},
            {bxc, {bxc_function, get_operand}},
            {out, {out_function, get_combo_operand}},
            {bdv, {bdv_function, get_combo_operand}},
            {cdv, {cdv_function, get_combo_operand}},
        };

    std::map<Instruction, std::string> instruction_name{
        {adv, "adv"},
        {bxl, "bxl"},
        {bst, "bst"},
        {jnz, "jnz"},
        {bxc, "bxc"},
        {out, "out"},
        {bdv, "bdv"},
        {cdv, "cdv"},
    };

    while (registers[IP] < program.size())
    {
        auto opcode = get_opcode();
        auto &functions = instructions[opcode];

        if (registers[IP] < program.size())
        {
            DPRINTX(instruction_name[opcode] << ": ");
            auto operand = functions.second();
            functions.first(operand);
            DPRINT_ENDL();
            DPRINTX_ENDL(print_registers());
            DPRINTX_ENDL("Output: " << print_values(output));
        }

        for (auto it : registers)
            CHECK(it.second >= 0);
        for (auto it : output)
            CHECK(it >= 0);
    }

    std::cout << print_values(output) << std::endl;
    return {};
}
