#include "main.h"

#include <bitset>
#include <iterator>
#include <vector>

constexpr int number_of_bits = 12;
using DiagnosticType = std::bitset<number_of_bits>;
using ReportType = std::vector<DiagnosticType>;

bool MostCommon(ReportType &report, int bit);
void Filter(ReportType &report, int bit, bool common);

uint64_t Answer(std::ifstream &file)
{
    ReportType report(std::istream_iterator<DiagnosticType>(file), {});
    int bit = number_of_bits - 1;
    bool most_common = MostCommon(report, bit);
    ReportType report_oxygen_generator;
    ReportType report_co2_scrubber;

    for (auto &diagnostic : report)
    {
        if (diagnostic[bit] == most_common)
        {
            report_oxygen_generator.push_back(std::move(diagnostic));
        }
        else
        {
            report_co2_scrubber.push_back(std::move(diagnostic));
        }
    }

    while (--bit >= 0)
    {
        if (report_oxygen_generator.size() > 1)
        {
            Filter(report_oxygen_generator, bit,
                   MostCommon(report_oxygen_generator, bit));
        }
        if (report_co2_scrubber.size() > 1)
        {
            Filter(report_co2_scrubber, bit,
                   !MostCommon(report_co2_scrubber, bit));
        }
    }

    DiagnosticType oxygen_generator_rating;
    DiagnosticType co2_scrubber_rating;

    if (report_oxygen_generator.size() == 1)
    {
        oxygen_generator_rating = report_oxygen_generator[0];
    }

    if (report_co2_scrubber.size() == 1)
    {
        co2_scrubber_rating = report_co2_scrubber[0];
    }

    return oxygen_generator_rating.to_ulong() * co2_scrubber_rating.to_ulong();
}

bool MostCommon(ReportType &report, int bit)
{
    int count[2] = {};

    for (auto &diagnostic : report)
    {
        count[diagnostic[bit]]++;
    }

    return count[1] >= count[0];
}

void Filter(ReportType &report, int bit, bool common)
{
    for (auto it = report.begin(); it != report.end();)
    {
        if (it->test(bit) == common)
        {
            it++;
        }
        else
        {
            report.erase(it);
        }
    }
}
