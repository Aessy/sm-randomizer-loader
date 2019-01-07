#include "sd2snes.h"

#include <algorithm>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/algorithm/hex.hpp>

#include <boost/endian/conversion.hpp>

#include <libserialport.h>

template<typename Iter>
void print_hex(Iter begin, Iter end)
{
    std::string s;
    boost::algorithm::hex(begin, end, std::back_inserter(s));
}

std::unique_ptr<sp_port, decltype(&sp_close)> open_port(std::string const& name)
{
    sp_port * port = nullptr;
    auto err = sp_get_port_by_name(name.c_str(), &port);
    if (err != SP_OK)
    {
        throw std::runtime_error("Could not find port name");
    }

    err = sp_open(port, SP_MODE_READ_WRITE);
    if (err != SP_OK)
    {
        std::ostringstream os;
        os << "Failed opening port: " << err << '\n';
        throw std::runtime_error(os.str());
    }
    sp_set_baudrate(port, 9600);
    sp_set_parity(port, SP_PARITY_NONE);
    sp_set_bits(port, 8);
    sp_set_stopbits(port, 1);
    sp_set_dtr(port, SP_DTR_ON);
    // sp_set_dsr(port, SP_DSR_FLOW_CONTROL);

    return std::unique_ptr<sp_port, decltype(&sp_close)>(port, sp_close);
}

template<typename OIter>
static void insert_number(uint32_t number, OIter iter)
{
    auto out = boost::endian::native_to_big(number);
    std::copy(reinterpret_cast<const char*>(&out),
              reinterpret_cast<const char*>(&out)+sizeof(number),
              iter);
}


void write_file(sp_port * port, std::string const& filename, std::vector<char> const& content)
{
    constexpr char write_header[] = {'U', 'S', 'B', 'A', 1, 0, 0}; // PUT FILE NONE
    char send_buffer[512] {};

    std::copy(std::begin(write_header), std::end(write_header), send_buffer);

    uint32_t const length = content.size();
    insert_number(length, &send_buffer[252]);
    std::copy(filename.begin(), filename.end(), &send_buffer[256]);

    print_hex(std::begin(send_buffer), std::end(send_buffer));
    auto const result = sp_blocking_write(port, send_buffer, sizeof(send_buffer), 4000);
    if (result != sizeof(send_buffer))
    {
        std::cerr << "Failed writing: " << result << '\n';
        throw std::runtime_error("Failed writing\n");
    }

    for (size_t s = 0; s < content.size(); s += 512)
    {
        auto const result = sp_blocking_write(port, &content[0]+s, 512, 4000);
        if (result != 512)
        {
            std::cerr << "Failed writing chunk: " << result << '\n';
            throw std::runtime_error("Failed writing\n");
        }
    }
}
