#include "main.h"

#include <algorithm>
#include <bitset>
#include <climits>
#include <ios>
#include <sstream>
#include <vector>

struct Packet
{
    int version = 0;
    int type_id = 0;
    int length_type_id = 0;
    std::vector<std::bitset<4>> literal_values;
    std::vector<Packet> sub_packets;

    uint64_t literal_value() const
    {
        uint64_t value = 0;

        for (auto &literal : literal_values)
        {
            value <<= 4;
            value += literal.to_ullong();
        }

        return value;
    }
};

class BuoyancyInterchangeTransmissionSystem
{
public:
    BuoyancyInterchangeTransmissionSystem(
        const std::string &input, unsigned int &decoded,
        bool outermost = true);

    bool decode(Packet &packet, std::string &outstanding_input);

private:
    bool get_version(Packet &packet);
    bool get_type_id(Packet &packet);
    bool get_length_type_id(Packet &packet);
    bool get_literal_values(Packet &packet);
    bool get_sub_packets_by_length(Packet &packet);
    bool get_sub_packets_by_number(Packet &packet);
    bool get_sub_packet(Packet &packet);
    bool get_literal_value(std::bitset<4> &literal_value, bool &keep_reading);
    bool get_length_sub_packets(std::bitset<15> &length_sub_packets);
    bool get_number_sub_packets(std::bitset<11> &number_sub_packets);
    bool get(int bits);
    void ignore_extra_zero_bits_at_end();
    void ignore_extra_zero_nibbles_at_end();

    std::string binary_input;
    std::stringstream front;
    unsigned int &bits_decoded;
    bool outermost_layer = false;
};

std::string hex_to_binary(const std::string &line);
bool empty_ignoring_extra_zeros_at_end(const std::string &input);
uint64_t evaluate_expression(const Packet &packet);

uint64_t Answer(std::ifstream &file)
{
    std::string line;
    file >> line;
    const std::string binary_input = hex_to_binary(line);
    unsigned int total_bits_decoded = 0;
    std::string outstanding_input;
    Packet packet = {};

    BuoyancyInterchangeTransmissionSystem bits(
        binary_input, total_bits_decoded);

    if (bits.decode(packet, outstanding_input) &&
        empty_ignoring_extra_zeros_at_end(outstanding_input))
    {
        return evaluate_expression(packet);
    }

    return 0;
}

std::string hex_to_binary(const std::string &line)
{
    std::string binary_input;

    for (int i = 0; i < line.size(); i++)
    {
        std::string hex = line.substr(i, 1);
        unsigned short us;
        std::stringstream(hex) >> std::hex >> us;
        std::bitset<4> bit4(us);
        std::stringstream ss;
        ss << bit4;
        binary_input += ss.str();
    }

    return binary_input;
}

bool empty_ignoring_extra_zeros_at_end(const std::string &input)
{
    if (input.empty())
    {
        return true;
    }
    else
    {
        return input == std::string(input.size(), '0');
    }
}

uint64_t evaluate_expression(const Packet &packet)
{
    uint64_t evaluated_value = 0;

    switch (packet.type_id)
    {
    case 0:
        for (auto &sub_packet : packet.sub_packets)
        {
            evaluated_value += evaluate_expression(sub_packet);
        }
        break;
    case 1:
        evaluated_value = 1;

        for (auto &sub_packet : packet.sub_packets)
        {
            evaluated_value *= evaluate_expression(sub_packet);
        }
        break;
    case 2:
        evaluated_value = ULONG_LONG_MAX;

        for (auto &sub_packet : packet.sub_packets)
        {
            evaluated_value = std::min(evaluated_value,
                                       evaluate_expression(sub_packet));
        }
        break;
    case 3:
        for (auto &sub_packet : packet.sub_packets)
        {
            evaluated_value = std::max(evaluated_value,
                                       evaluate_expression(sub_packet));
        }
        break;
    case 4:
        return packet.literal_value();
    case 5:
        if (packet.sub_packets.size() == 2)
        {
            return evaluate_expression(packet.sub_packets[0]) >
                   evaluate_expression(packet.sub_packets[1]);
        }
        break;
    case 6:
        if (packet.sub_packets.size() == 2)
        {
            return evaluate_expression(packet.sub_packets[0]) <
                   evaluate_expression(packet.sub_packets[1]);
        }
        break;
    case 7:
        if (packet.sub_packets.size() == 2)
        {
            return evaluate_expression(packet.sub_packets[0]) ==
                   evaluate_expression(packet.sub_packets[1]);
        }
        break;
    }

    return evaluated_value;
}

BuoyancyInterchangeTransmissionSystem::BuoyancyInterchangeTransmissionSystem(
    const std::string &input, unsigned int &decoded, bool outermost)
    : binary_input(input), bits_decoded(decoded), outermost_layer(outermost) {}

bool BuoyancyInterchangeTransmissionSystem::decode(
    Packet &packet, std::string &outstanding_input)
{
    outstanding_input = binary_input;

    if (!(get_version(packet) && get_type_id(packet)))
    {
        return false;
    }

    bool ok = false;

    if (packet.type_id == 4)
    {
        ok = get_literal_values(packet);
    }
    else if (get_length_type_id(packet))
    {
        if (packet.length_type_id == 0)
        {
            ok = get_sub_packets_by_length(packet);
        }
        else
        {
            ok = get_sub_packets_by_number(packet);
        }
    }

    if (ok)
    {
        if (outermost_layer)
        {
            ignore_extra_zero_bits_at_end();
            ignore_extra_zero_nibbles_at_end();
        }
        outstanding_input = binary_input;
    }

    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_version(
    Packet &packet)
{
    std::bitset<3> version;
    bool ok = get(version.size());
    front >> version;
    packet.version = version.to_ulong();
    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_type_id(
    Packet &packet)
{
    std::bitset<3> type_id;
    bool ok = get(type_id.size());
    front >> type_id;
    packet.type_id = type_id.to_ulong();
    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_literal_value(
    std::bitset<4> &literal_value, bool &keep_reading)
{
    std::bitset<1> prefix;
    bool ok = get(prefix.size());
    front >> prefix;
    keep_reading = prefix[0];
    ok = ok && get(literal_value.size());
    front >> literal_value;
    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_length_type_id(
    Packet &packet)
{
    std::bitset<1> length_type;
    bool ok = get(length_type.size());
    front >> length_type;
    packet.length_type_id = length_type.to_ulong();
    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_literal_values(
    Packet &packet)
{
    bool keep_reading = true;

    while (keep_reading)
    {
        std::bitset<4> literal_value;

        if (get_literal_value(literal_value, keep_reading))
        {
            packet.literal_values.push_back(literal_value);
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool BuoyancyInterchangeTransmissionSystem::get_sub_packets_by_length(
    Packet &packet)
{
    std::bitset<15> length_sub_packets;
    bool ok = get_length_sub_packets(length_sub_packets);
    auto bits_decoded_limit = bits_decoded + length_sub_packets.to_ulong();

    while (ok && bits_decoded < bits_decoded_limit)
    {
        ok = get_sub_packet(packet);
    }

    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_sub_packets_by_number(
    Packet &packet)
{
    std::bitset<11> number_sub_packets;
    bool ok = get_number_sub_packets(number_sub_packets);

    for (int i = 0; ok && i < number_sub_packets.to_ulong(); i++)
    {
        ok = get_sub_packet(packet);
    }

    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_sub_packet(
    Packet &packet)
{
    Packet sub_packet = {};
    std::string outstanding_input;
    BuoyancyInterchangeTransmissionSystem bits(
        binary_input, bits_decoded, false);

    if (bits.decode(sub_packet, outstanding_input))
    {
        packet.sub_packets.push_back(sub_packet);
        binary_input = outstanding_input;
        return true;
    }
    else
    {
        return false;
    }
}

bool BuoyancyInterchangeTransmissionSystem::get_length_sub_packets(
    std::bitset<15> &length_sub_packets)
{
    bool ok = get(length_sub_packets.size());
    front >> length_sub_packets;
    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get_number_sub_packets(
    std::bitset<11> &number_sub_packets)
{
    bool ok = get(number_sub_packets.size());
    front >> number_sub_packets;
    return ok;
}

bool BuoyancyInterchangeTransmissionSystem::get(
    int bits)
{
    front.clear();

    if (bits <= binary_input.size())
    {
        auto binary = binary_input.substr(0, bits);
        binary_input.erase(0, bits);
        front << binary;
        bits_decoded += bits;
        return true;
    }
    else
    {
        return false;
    }
}

void BuoyancyInterchangeTransmissionSystem::ignore_extra_zero_bits_at_end()
{
    int extra = 4 - (bits_decoded % 4);

    if (extra > 0 && extra < 4 &&
        binary_input.substr(0, extra) == std::string(extra, '0'))
    {
        binary_input.erase(0, extra);
        bits_decoded += extra;
    }
}

void BuoyancyInterchangeTransmissionSystem::ignore_extra_zero_nibbles_at_end()
{
    while (binary_input.substr(0, 4) == "0000")
    {
        binary_input.erase(0, 4);
        bits_decoded += 4;
    }
}
