// #include <debug_print.h>
#define DTIMER_ON
// #define PART_1
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
#if 0
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
        ss << std::dec << register_a << ": 0";
        ss << std::oct << register_a << ": ";

        for (auto it = output.begin(); it != output.end();)
        {
            ss << *it;
            if (*it != program[it - output.begin()])
                ss << '*';

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

    auto begin_with = [&](const std::vector<int> list) -> bool
    {
        for (int i = 0; i < list.size(); i++)
            if (list[i] != output[i])
                return false;
        return true;
    };

    std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>>
        sorted_queue;

    while (!match_queue.empty())
    {
        auto top_match = match_queue.top();
        match_queue.pop();
        get_output(top_match.second);
        if (begin_with({2, 4, 1, 5}))
        {
            DPRINTX_ENDL(print_output(top_match.second));
            sorted_queue.push(top_match.second);
        }
    }

    auto print_sorted_queue = [&]() -> void
    {
        if (!sorted_queue.empty())
            DPRINTX_ENDL(print_output(sorted_queue.top()));

        while (!sorted_queue.empty())
        {
            auto top = sorted_queue.top();
            sorted_queue.pop();

            if (sorted_queue.empty())
                break;

            auto next = sorted_queue.top();
            auto diff = next - top;
            DPRINTX_ENDL(print_output(next) << ", diff " << std::dec << diff
                                            << ": 0" << std::oct << diff);
        }
    };

    print_sorted_queue();

    const int64_t size_16_start = 01000000000000000;
    const int64_t size_16_end = 07777777777777777;
    const int64_t oct_13 = 010000000000000;

    auto find = [&](int64_t register_a, int min_match) -> int
    {
        int match = get_output(register_a);

        if (match >= min_match)
        {
            DPRINTX_ENDL(print_output(register_a));
            sorted_queue.push(register_a);
        }

        return match;
    };

    auto get_end = [](int64_t end) -> int64_t
    {
        end += 1;
        end *= oct_13;
        end -= 1;
        return end;
    };

    /* NOTE: Manual adhoc search from here. This is not a general solution.

    The lowest and the highest of register A values that produce size 16 of output.
    The answer is somewhere in between thesse two values.

    35184372088832: 01000000000000000: 3,3,3,3,3,3,3,3,3,3,3,3,3,1,3,2 size 16, match 1
    281474976710655: 07777777777777777: 3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,5 size 16, match 2

    So far the highest number of match is 12 and found in very high side of the range
    (0777ooooooooooooo).

    Three interesting things,
    (1) 281001341439745 (07771067110647401) the register A value with 12 match.
    (2) 27640 (065770) the lowest difference repeating the first 4 output (2,4,1,5).
    (3) 127336448 (0745600000) somewhat interesting difference with 5 zeros in Octal.

    281001341439745: 07771067110647401: 2,4,1,5,5,5,1,6,1,1,5,5,5,3,3,5 size 16, match 12
    281001341412105: 07771067110561411: 2,4,1,5,0,3,1,6,1,1,5,5,5,3,3,5 size 16, match 11
    280963349892865: 07770434052447401: 2,4,1,5,5,5,1,3,4,1,0,5,4,3,3,5 size 16, match 11
    271032165593865: 07550022774771411: 2,4,1,5,3,5,1,3,1,1,5,1,0,3,3,5 size 16, match 11
    271032038257417: 07550022027171411: 2,4,1,5,7,1,1,0,7,1,5,1,0,3,3,5 size 16, match 11
    280933340047770: 07770074374064632: 2,4,1,5,5,4,1,3,7,1,5,5,4,3,3,5 size 16, match 10
    280918266515201: 07767714137107401: 2,4,1,5,5,5,3,4,4,1,5,3,1,2,3,5 size 16, match 9
    278777847019265: 07730600642573401: 2,4,1,5,7,3,2,5,5,7,2,5,5,7,3,5 size 16, match 7
    278911729651457: 07732546244347401: 2,4,1,5,1,4,5,5,7,1,1,3,6,7,3,5 size 16, match 6
    271032038257417: 07550022027171411: 2,4,1,5,7,1,1,0,7,1,5,1,0,3,3,5 size 16, match 11

    271032165593865: 07550022774771411: 2,4,1,5,3,5,1,3,1,1,5,1,0,3,3,5 size 16, match 11, diff 127336448: 0745600000
    278777847019265: 07730600642573401: 2,4,1,5,7,3,2,5,5,7,2,5,5,7,3,5 size 16, match 7, diff 7745681425400: 0160555645601770
    278911729651457: 07732546244347401: 2,4,1,5,1,4,5,5,7,1,1,3,6,7,3,5 size 16, match 6, diff 133882632192: 01745401554000
    280918266515201: 07767714137107401: 2,4,1,5,5,5,3,4,4,1,5,3,1,2,3,5 size 16, match 9, diff 2006536863744: 035145672540000
    280933340047770: 07770074374064632: 2,4,1,5,5,4,1,3,7,1,5,5,4,3,3,5 size 16, match 10, diff 15073532569: 0160234755231
    280963349892865: 07770434052447401: 2,4,1,5,5,5,1,3,4,1,0,5,4,3,3,5 size 16, match 11, diff 30009845095: 0337456362547
    281001341412105: 07771067110561411: 2,4,1,5,0,3,1,6,1,1,5,5,5,3,3,5 size 16, match 11, diff 37991519240: 0433036112010
    281001341439745: 07771067110647401: 2,4,1,5,5,5,1,6,1,1,5,5,5,3,3,5 size 16, match 12, diff 27640: 065770

    Using (1) as the starting value and difference (2), found other 12 matches in 0777ooooooooooooo range.
    (4) 280929868379905 (07770042500573401) the first register A value with 12 match in 0777ooooooooooooo range.

    280929868379905: 7770042500573401: 2,4,1,5,7,3,1,6,3,1,5,5,4,3,3,5 size 16, match 12
    280930385966545: 7770046367132721: 7,4,1,5,7,1,1,6,4,1,5,5,4,3,3,5 size 16, match 12, diff 517586640
    280932015869705: 7770062500607411: 2,4,1,5,2,5,1,6,3,1,5,5,4,3,3,5 size 16, match 12, diff 1629903160
    280967660441345: 7770474146007401: 2,4,1,5,7,2,1,1,4,1,5,5,4,3,3,5 size 16, match 12, diff 35644571640
    280981925334785: 7770646366207401: 2,4,1,5,7,6,3,6,4,1,5,5,4,3,3,5 size 16, match 12, diff 14264893440
    281001341439745: 7771067110647401: 2,4,1,5,5,5,1,6,1,1,5,5,5,3,3,5 size 16, match 12, diff 19416104960
    281067281165065: 7772042334725411: 2,4,1,3,7,5,1,2,4,1,5,5,6,3,3,5 size 16, match 12, diff 65939725320
    281069347337985: 7772061646573401: 2,4,1,5,7,3,1,5,4,1,5,5,6,3,3,5 size 16, match 12, diff 2066172920
    281070061997825: 7772067114747401: 2,4,1,5,1,5,1,6,1,1,5,5,6,3,3,5 size 16, match 12, diff 714659840
    281136486418185: 7773046032003411: 2,4,1,0,7,5,1,6,2,1,5,5,7,3,3,5 size 16, match 12, diff 66424420360
    281139444119305: 7773074054573411: 2,4,1,5,7,3,1,3,4,1,5,5,7,3,3,5 size 16, match 12, diff 2957701120
    281139447657225: 7773074072171411: 2,4,1,5,7,7,1,5,4,1,5,5,7,3,3,5 size 16, match 12, diff 3537920
    281380356598145: 7776477014724601: 2,2,1,5,7,5,1,6,4,3,5,5,2,3,3,5 size 16, match 12, diff 240908940920

    Using (4) as the starting value and difference (3), found 13 matches in the entire size 16 range,
    35184372088832 (01000000000000000) ~ 281474976710655 (07777777777777777).
    (5) 107347878541057 (03032074012573401) the first register A value with 13 match.

    107347878541057: 03032074012573401: 2,4,1,5,7,3,1,2,4,1,5,5,5,3,3,0 size 16, match 13
    108375738349313: 03051046130573401: 2,4,1,5,7,3,1,6,1,1,5,1,0,3,3,0 size 16, match 13, diff 1027859808256: 016752116000000
    108378285078273: 03051071113573401: 2,4,1,5,7,3,1,6,6,1,5,1,0,3,3,0 size 16, match 13, diff 2546728960: 022763000000
    108398149564161: 03051315114573401: 2,4,1,5,7,3,1,6,4,3,7,5,0,3,3,0 size 16, match 13, diff 19864485888: 0224001000000
    108619205637889: 03054474112573401: 2,4,1,5,7,3,1,2,4,1,5,5,1,3,3,0 size 16, match 13, diff 221056073728: 03156776000000
    108753672926977: 03056446002573401: 2,4,1,5,7,3,1,7,2,1,5,5,0,3,3,0 size 16, match 13, diff 134467289088: 01751670000000
    108756729001729: 03056474614573401: 2,4,1,5,7,3,1,1,1,1,5,5,0,3,3,0 size 16, match 13, diff 3056074752: 026612000000
    112738794403585: 03150441362573401: 2,4,1,5,7,3,4,6,4,0,5,5,0,3,3,0 size 16, match 13, diff 3982065401856: 071744546000000
    112742359824129: 03150474023573401: 2,4,1,5,7,3,0,7,4,1,5,5,0,3,3,0 size 16, match 13, diff 3565420544: 032441000000
    112756621506305: 03150646227573401: 2,4,1,5,7,3,1,2,3,1,5,5,0,3,3,0 size 16, match 13, diff 14261682176: 0152204000000
    113154675242753: 03156474016173401: 2,4,1,1,7,5,4,2,4,1,5,5,0,3,3,0 size 16, match 13, diff 398053736448: 05625566400000
    129269102737153: 03531071674573401: 2,4,1,5,7,3,1,1,4,1,5,5,7,3,3,0 size 16, match 13, diff 16114427494400: 0352375656400000
    130331598059265: 03550446046573401: 2,4,1,5,7,3,1,5,2,1,5,5,0,3,3,0 size 16, match 13, diff 1062495322112: 017354152000000
    130717682169601: 03556142500573401: 2,4,1,5,7,3,1,6,3,1,1,5,0,3,3,0 size 16, match 13, diff 386084110336: 05474432000000
    130718191515393: 03556146327573401: 2,4,1,5,7,3,1,2,4,1,1,5,0,3,3,0 size 16, match 13, diff 509345792: 03627000000
    130746205533953: 03556467110573401: 2,4,1,5,7,3,1,6,1,1,1,5,0,3,3,0 size 16, match 13, diff 28014018560: 0320561000000
    135144915793665: 03656474054573401: 2,4,1,5,7,3,1,3,4,1,5,5,0,3,6,0 size 16, match 13, diff 4398710259712: 0100004744000000
    138134266246913: 03732074367573401: 2,4,1,5,7,3,1,6,7,1,5,5,6,3,3,0 size 16, match 13, diff 2989350453248: 053400313000000
    139540060632833: 03756446357573401: 2,4,1,5,7,3,7,3,4,1,5,5,0,3,3,0 size 16, match 13, diff 1405794385920: 024351770000000

    Using (5) as the starting value and difference (2), 13 matches in 0303ooooooooooooo range.

    107347878541057: 03032074012573401: 2,4,1,5,7,3,1,2,4,1,5,5,5,3,3,0 size 16, match 13
    107382247139737: 03032474054424631: 3,4,1,5,7,5,1,3,4,1,5,5,5,3,3,0 size 16, match 13, diff 34368598680: 0400041631230
    107410181312257: 03033014154747401: 2,4,1,5,1,5,1,3,4,1,6,5,0,3,3,0 size 16, match 13, diff 27934172520: 0320100322550
    107412332367617: 03033034172447401: 2,4,1,5,5,5,1,7,4,1,3,5,0,3,3,0 size 16, match 13, diff 2151055360: 020015500000
    107413690928897: 03033046271047401: 2,4,1,5,5,5,1,1,3,1,5,5,0,3,3,0 size 16, match 13, diff 1358561280: 012076400000
    107413700215937: 03033046334402201: 2,7,1,3,2,5,1,6,4,1,5,5,0,3,3,0 size 16, match 13, diff 9287040: 043332600
    107413707264137: 03033046367310211: 2,3,5,5,7,6,1,6,4,1,5,5,0,3,3,0 size 16, match 13, diff 7048200: 032706010
    107413708590857: 03033046374347411: 2,4,1,5,1,7,1,3,4,1,5,5,0,3,3,0 size 16, match 13, diff 1326720: 05037200
    107415137910537: 03033061110677411: 2,4,1,3,1,5,1,6,2,1,5,5,0,3,3,0 size 16, match 13, diff 1429319680: 012514330000
    107416609685257: 03033074067177411: 2,4,1,3,7,1,1,5,4,1,5,5,0,3,3,0 size 16, match 13, diff 1471774720: 012756300000
    107416611039617: 03033074074324601: 2,2,1,5,7,7,1,5,4,1,5,5,0,3,3,0 size 16, match 13, diff 1354360: 05125170
    107416616761097: 03033074122173411: 2,4,1,1,7,3,1,7,4,1,5,5,0,3,3,0 size 16, match 13, diff 5721480: 025646610
    107416620299017: 03033074137571411: 2,4,1,5,7,3,3,4,4,1,5,5,0,3,3,0 size 16, match 13, diff 3537920: 015376000
    107417009497857: 03033077054323401: 2,4,1,3,7,5,1,3,4,3,5,5,0,3,3,0 size 16, match 13, diff 389198840: 02714531770
    107448093635337: 03033446534771411: 2,4,1,5,3,5,1,6,3,1,5,1,0,3,3,0 size 16, match 13, diff 31084137480: 0347460446010
    107461715954057: 03033614132004611: 2,1,1,5,7,5,1,4,4,1,5,6,0,3,3,0 size 16, match 13, diff 13622318720: 0145375013200
    107519681235337: 03034474032004611: 2,1,1,5,7,5,1,4,4,1,5,5,1,3,3,0 size 16, match 13, diff 57965281280: 0657700000000

    Using difference 3537920 (015376000) and the following {starting value, ending value 0OOO}, 0OOO for 0OOO7777777777777,
    found 14 matches.

    108378943131393: 03051076032007401: 2,4,1,5,7,5,1,6,4,x,5,x,0,3,3,0 size 16, match 14
    108399491370753: 03051327113247401: 2,4,1,5,x,5,1,6,x,1,5,5,0,3,3,0 size 16, match 14, diff 20548239360: 0231061240000
    108753736797953: 03056446366307401: 2,4,1,5,7,x,x,6,4,1,5,5,0,3,3,0 size 16, match 14, diff 354245427200: 05117253040000
    108756625526529: 03056474002007401: 2,4,1,5,7,x,1,x,4,1,5,5,0,3,3,0 size 16, match 14, diff 2888728576: 025413500000
    112742056365825: 03150471616107401: 2,4,1,5,7,5,x,x,4,1,5,5,0,3,3,0 size 16, match 14, diff 3985430839296: 071775614100000
    112768148049665: 03150774132007401: 2,4,1,5,7,5,1,x,4,1,x,5,0,3,3,0 size 16, match 14, diff 26091683840: 0302313700000
    130747011526401: 03556475113267401: 2,4,1,x,7,5,1,6,4,x,5,5,0,3,3,0 size 16, match 14, diff 17978863476736: 0405500761260000
    138440543981313: 03736446334447401: 2,4,1,5,x,5,1,6,4,1,5,5,x,3,3,0 size 16, match 14, diff 7693532454912: 0157751221160000
    */

    std::vector<std::pair<int64_t, int64_t>> values{
        {107347878541057, 0303},
        {108375738349313, 0305},
        {108378285078273, 0305},
        {108398149564161, 0305},
        {108619205637889, 0305},
        {108753672926977, 0305},
        {108756729001729, 0305},
        {112738794403585, 0315},
        {112742359824129, 0315},
        {112756621506305, 0315},
        {113154675242753, 0315},
        {129269102737153, 0353},
        {130331598059265, 0355},
        {130717682169601, 0355},
        {130718191515393, 0355},
        {130746205533953, 0355},
        {135144915793665, 0365},
        {138134266246913, 0373},
        {139540060632833, 0375},
    };

    for (auto value : values)
    {
        for (auto register_a = value.first;
             register_a <= get_end(value.second);
             register_a += 015376000)
        {
            if (find(register_a, 14) == program_size)
                return register_a;
        }
    }

    print_sorted_queue();

    /*
    15 matches:
    108753730031361: 03056446334447401: 2,4,1,5,x,5,1,6,4,1,5,5,0,3,3,0 size 16, match 15
    113151776542465: 03156446334447401: 2,4,1,5,x,5,1,6,4,1,5,5,0,3,3,0 size 16, match 15, diff 4398046511104: 0100000000000000
    130743962586881: 03556446334447401: 2,4,1,5,x,5,1,6,4,1,5,5,0,3,3,0 size 16, match 15, diff 17592186044416: 0400000000000000
    139540055609089: 03756446334447401: 2,4,1,5,x,5,1,6,4,1,5,5,0,3,3,0 size 16, match 15, diff 8796093022208: 0200000000000000
    */

    int64_t match_14[] = {
        108378943131393, 108399491370753, 108753736797953, 108756625526529,
        112742056365825, 112768148049665, 130747011526401, 138440543981313};

    for (auto match : match_14)
    {
        int64_t register_a = match;

        while (register_a >= size_16_start)
        {
            if (find(register_a, 15) == program_size)
                return register_a;

            register_a -= oct_13;
        }

        register_a = match + oct_13;

        while (register_a <= size_16_end)
        {
            if (find(register_a, 15) == program_size)
                return register_a;

            register_a += oct_13;
        }
    }

    std::vector<std::pair<int64_t, int64_t>> values_15{
        {108753730031361, 0305},
        // {113151776542465, 0315},
        // {130743962586881, 0355},
        // {139540055609089, 0375},
    };

    for (auto value : values_15)
    {
        const int step = 065770;

        for (auto register_a = value.first - step;
             register_a > (get_end(value.second) - oct_13 + 1);
             register_a -= step)
        {
            if (find(register_a, 15) == program_size)
                return register_a;
        }

        for (auto register_a = value.first + step;
             register_a <= get_end(value.second);
             register_a += step)
        {
            if (find(register_a, 15) == program_size)
                return register_a;
        }
    }

    print_sorted_queue();

    int64_t match_15[] =
        {108753730031361, 113151776542465, 130743962586881, 139540055609089};

    for (auto match : match_15)
    {
        int64_t register_a = match;

        while (register_a >= size_16_start)
        {
            if (find(register_a, 15) == program_size)
                return register_a;

            register_a -= oct_13;
        }

        register_a = match + oct_13;

        while (register_a <= size_16_end)
        {
            if (find(register_a, 15) == program_size)
                return register_a;

            register_a += oct_13;
        }
    }
#endif
    constexpr unsigned int program_size = 16;

    auto run = [&](int64_t register_a) -> int
    {
        const unsigned int program[] = {2, 4, 1, 5, 7, 5, 1, 6, 4, 1, 5, 5, 0, 3, 3, 0};
        unsigned int ip = 0;
        int64_t a = register_a, b = 0, c = 0;
        unsigned int output[program_size] = {};
        unsigned int output_size = 0;

        auto get_operand = [&]() -> int64_t
        {
            if (ip < program_size)
                return program[ip++];
            return 0;
        };

        auto get_combo_operand = [&]() -> int64_t
        {
            auto operand = get_operand();

            switch (operand)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                break;
            case 4:
                return a;
            case 5:
                return b;
            case 6:
                return c;
            }

            return operand;
        };

        while (ip < program_size)
        {
            switch (program[ip++])
            {
            case 0:
                a = a / std::pow(2, get_combo_operand());
                break;
            case 1:
                b = b ^ get_operand();
                break;
            case 2:
                b = get_combo_operand() % 8;
                break;
            case 3:
                if (a != 0)
                    ip = get_operand();
                else
                    get_operand();
                break;
            case 4:
                b = b ^ c;
                get_operand();
                break;
            case 5:
                if (output_size < program_size)
                    output[output_size++] = get_combo_operand() % 8;
                else
                    get_combo_operand();
                break;
            case 6:
                b = a / std::pow(2, get_combo_operand());
                break;
            case 7:
                c = a / std::pow(2, get_combo_operand());
                break;
            }
        }

#ifdef DPRINT_ON
        std::cout << register_a;

        for (int i = 0; i < output_size; i++)
        {
            std::cout << (i ? ',' : ' ');
            std::cout << output[i];
        }
#endif
        int match = 0;

        for (int i = 0; i < output_size; i++)
            if (output[i] == program[i])
                match++;

#ifdef DPRINT_ON
        std::cout << " match " << match << std::endl;
#endif
        return match;
    };

#ifdef PART_1
    run(60589763);
    return {};
#endif

    const int64_t start = 108753730031361;
    const int64_t end = 03057777777777777;
    const int64_t oct_13 = 010000000000000;
    const int step = 8;

    for (auto register_a = start - step;
         register_a > end - oct_13 + 1;
         register_a -= step)
    {
        if (run(register_a) == program_size)
            return register_a;
    }

    for (auto register_a = start + step;
         register_a <= end;
         register_a += step)
    {
        if (run(register_a) == program_size)
            return register_a;
    }

    return {};
}
