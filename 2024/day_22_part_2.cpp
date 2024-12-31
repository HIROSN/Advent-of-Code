// #include <debug_print.h>
#include <main.h>

#include <map>
#include <queue>
#include <sstream>
#include <utility>
#include <vector>

std::optional<uint64_t> Answer(std::ifstream &file)
{
    constexpr int max_generations = 2000;
    constexpr int max_buyers = 2375;

    char prices[max_buyers][max_generations + 1] = {};
    std::vector<int64_t> initial_secrets, secrets;
    int number_of_generations = max_generations;
    int number_of_buyers = 0;
    int64_t secret;

    while (file >> secret)
    {
        initial_secrets.push_back(secret);
        number_of_buyers++;
    }

    auto get_price = [&](int buyer, int g) -> int
    {
        DCHECK(buyer >= 0);
        return prices[buyer][g];
    };

#ifdef DPRINT_ON
    std::vector<int> expected;
#if 0
    number_of_buyers = 1;
    initial_secrets[0] = {123};
    number_of_generations = 10;
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
#endif

    for (int buyer = 0; buyer < number_of_buyers; buyer++)
    {
        for (int g = 0; g <= number_of_generations; g++)
        {
            if (!g)
            {
                secret = initial_secrets[buyer];
                secrets.push_back(secret);
            }
            else
            {
                secret = secrets[buyer];
                auto result = secret * 64;
                secret ^= result;
                secret %= 16777216;

                result = secret / 32;
                secret ^= result;
                secret %= 16777216;

                result = secret * 2048;
                secret ^= result;
                secret %= 16777216;
                secrets[buyer] = secret;
            }

            prices[buyer][g] = secret % 10;
        }
    }

    using Changes = std::vector<short>;

    auto print_changes = [](const Changes &changes) -> std::string
    {
        if (changes.empty())
            return {};
        std::stringstream ss;
        ss << "{";
        for (auto it = changes.begin(); it != changes.end(); it++)
        {
            if (it != changes.begin())
                ss << ',';
            ss << *it;
        }
        ss << "}";
        return ss.str();
    };

    auto get_changes = [&](int buyer, int g) -> std::pair<int, Changes>
    {
        DCHECK(buyer >= 0);
        DCHECK(g >= 4);
        Changes changes;
        int highest_price = 0;
        int price = get_price(buyer, g);

        for (int h = g - 3; h < g; h++)
        {
            if (h < 0)
                continue;
            int ph = get_price(buyer, h);
            if (ph > highest_price)
                highest_price = ph;
        }

        if (price <= highest_price)
            return {};

        for (int i = g - 4; i < g; i++)
            changes.push_back(get_price(buyer, i + 1) - get_price(buyer, i));

        return {price, changes};
    };

#ifdef DPRINT_ON
    if (expected.size())
    {
        for (int g = 0; g <= number_of_generations; g++)
        {
            CHECK(get_price(0, g) == (expected[g] % 10));
            DPRINTX(get_price(0, g));
            if (g)
            {
                DPRINTX(": (" << get_price(0, g) - get_price(0, g - 1)
                              << ")");
            }
            if (g >= 4)
            {
                auto ch = get_changes(0, g);
                if (!ch.second.empty())
                {
                    DPRINTX("\t" << ch.first << ":"
                                 << print_changes(ch.second));
                }
            }
            DPRINT_ENDL();
        }
    }
#endif

    int most_bananas = 0;

    std::map<Changes, std::vector<std::priority_queue<int>>>
        buyer_top_price_sequences;

    for (int buyer = 0; buyer < number_of_buyers; buyer++)
    {
        std::map<Changes, bool> sequences_occured;

        for (int g = 4; g <= number_of_generations; g++)
        {
            auto changes = get_changes(buyer, g);
            if (changes.first == 0 || changes.second.empty())
                continue;

            if (sequences_occured.find(
                    changes.second) != sequences_occured.end())
                continue;

            if (!buyer_top_price_sequences[changes.second].size())
                buyer_top_price_sequences[changes.second] =
                    std::vector<std::priority_queue<int>>(
                        number_of_buyers, std::priority_queue<int>());

            buyer_top_price_sequences[changes.second][buyer].push(
                changes.first);

            sequences_occured[changes.second] = true;
        }
    }

    for (auto it : buyer_top_price_sequences)
    {
        int bananas = 0;
        for (const auto &buyer : it.second)
            if (!buyer.empty())
                bananas += buyer.top();

        if (bananas > most_bananas)
        {
            most_bananas = bananas;
            DPRINTX_ENDL(most_bananas << " : " << print_changes(it.first));
        }
    }

    return most_bananas;
}
