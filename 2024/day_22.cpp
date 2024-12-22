// #include <debug_print.h>
#include <main.h>

#include <sstream>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    std::vector<std::vector<int>> secret_number(2001, std::vector<int>(2375, 0));
    int number_of_secrets = 0, number_of_generations = 0;
    int secret, generate_new = 2000;

    while (file >> secret)
        secret_number[number_of_generations][number_of_secrets++] = secret;

    auto print_secrets = [&]() -> std::string
    {
        std::stringstream ss;
        for (int n = 0; n < number_of_secrets; n++)
        {
            ss << secret_number[0][n] << ": "
               << secret_number[number_of_generations][n] << " ("
               << number_of_generations << "th)";
            ss << std::endl;
        }
        return ss.str();
    };

    std::vector<int> expected;
#if 0
    number_of_secrets = 1;
    secret_number[0] = {123};
    generate_new = 10;
    expected = {
        123,
        15887950,
        16495136,
        527345,
        704524,
        1553684,
        12683156,
        11100544,
        12249484,
        7753432,
        5908254};
#endif

    for (int g = 0; g < generate_new; g++)
    {
        for (int n = 0; n < number_of_secrets; n++)
        {
            int64_t secret = secret_number[number_of_generations][n];
            auto result = secret * 64;
            secret ^= result;
            secret %= 16777216;

            result = secret / 32;
            secret ^= result;
            secret %= 16777216;

            result = secret * 2048;
            secret ^= result;
            secret %= 16777216;

            secret_number[number_of_generations + 1][n] = secret;
        }

        number_of_generations++;

        if (number_of_secrets == 1)
            DPRINTX(print_secrets());
    }

    if (number_of_secrets > 1)
        DPRINTX(print_secrets());

    if (expected.size())
        for (int g = 0; g <= number_of_generations; g++)
            CHECK(secret_number[g][0] == expected[g]);

    uint64_t sum_of_secret_number_generated = 0;

    for (int n = 0; n < number_of_secrets; n++)
        sum_of_secret_number_generated +=
            secret_number[number_of_generations][n];

    return sum_of_secret_number_generated;
}
