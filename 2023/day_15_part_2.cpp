#include <main.h>

#include <algorithm>
#include <list>
#include <sstream>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);

    auto HASH = [](const std::string &str) -> int
    {
        int current_value = 0;

        for (const auto &ASCII_code : str)
        {
            current_value += ASCII_code;
            current_value *= 17;
            current_value %= 256;
        }

        return current_value;
    };

    struct LensSlot
    {
        std::string label;
        char operation = 0;
        int focal_length = 0;

        bool operator==(const std::string &right) const
        {
            return label == right;
        }
    };

    std::vector<LensSlot> boxes[256];

    while (ss.good())
    {
        std::string str;
        std::getline(ss, str, ',');
        LensSlot lens;

        for (char operation : {'-', '='})
        {
            auto it = str.find(operation);
            if (it != std::string::npos)
            {
                lens = {str.substr(0, it), operation};
                std::stringstream(str.substr(++it)) >> lens.focal_length;
                break;
            }
        }

        auto &box = boxes[HASH(lens.label)];
        auto it = std::find(box.begin(), box.end(), lens.label);

        switch (lens.operation)
        {
        case '-':
            if (it != box.end())
                box.erase(it);
            break;
        case '=':
            if (it != box.end())
                *it = lens;
            else
                box.push_back(lens);
            break;
        }
    }

    int total_focusing_power = 0;

    for (int ib = 0; ib < sizeof(boxes) / sizeof(boxes[0]); ib++)
    {
        const int one_plus = ib + 1;
        const auto &box = boxes[ib];

        for (int is = 0; is < box.size(); is++)
        {
            const int slot = is + 1;
            const auto &lens = box[is];
            total_focusing_power += one_plus * slot * lens.focal_length;
        }
    }

    return total_focusing_power;
}
