#pragma once

#include <memory>
#include <string>
#include <vector>

#include <libserialport.h>

using SerialPort = std::unique_ptr<sp_port, decltype(&sp_close)>;

void write_file(sp_port * port, std::string const& filename, std::vector<char> const& content);

SerialPort open_port(std::string const& name);
