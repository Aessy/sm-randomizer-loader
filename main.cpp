#include <iostream>
#include <stdexcept>
#include <fstream>
#include <filesystem>

#include <boost/algorithm/string.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "sd2snes.h"
#include <docopt/docopt.h>

#include "json11.hpp"

#include "restclient-cpp/restclient.h"

using namespace std::string_literals;

struct Options
{
    std::string rom;
    std::string preset;
    std::string complexity;
    std::string seed;
    std::string progression_speed;
};

static std::string createBody(Options const& options, std::string const& preset_details)
{
    static std::map<std::string, std::string> const default_values
    {
         {"itemsounds", "on"}
        ,{"spinjumprestart", "off"}
        ,{"elevators_doors_speed", "on"}
        ,{"skip_ceres", "on"}
        ,{"skip_intro", "off"}
        ,{"animals", "off"}
        ,{"layoutPatches", "on"}
        ,{"noGravHeat", "on"}
        ,{"areaLayout", "off"}
        ,{"variaTweaks", "on"}
        ,{"areaRandomization", "off"}
        ,{"No_Music", "off"}
        ,{"randoPreset", ""}
        ,{"missileQty", "3"}
        ,{"superQty", "2"}
        ,{"powerBombQty", "1"}
        ,{"minorQty", "100"}
        ,{"energyQty", "vanilla"}
        ,{"maxDifficulty", "hardcore"}
        ,{"fullRandomization", "on"}
        ,{"suitsRestriction", "on"}
        ,{"morphPlacement", "early"}
        ,{"funCombat", "off"}
        ,{"funMovement", "off"}
        ,{"funSuits", "off"}
        ,{"progressionDifficulty", "normal"}
        ,{"hideItems", "off"}
        ,{"strictMinors", "off"}
    };

    std::string s =   "paramsFileTarget="s + preset_details
                    + "&preset=" + options.preset
                    + "&complexity=" + options.complexity
                    + "&progressionSpeed=" + options.progression_speed
                    + "&seed=" + options.seed;

    for (auto const& [key, value] : default_values)
    {
        s += fmt::format("&{}={}", key, value);
    }

    return s;
}

std::string fetchPreset(Options const& options)
{

    std::cout << "Fething preset '" << options.preset<< "' from randomizer web service\n";
    std::string const body = "preset="s + options.preset;
    auto const r = RestClient::post("https://randommetroidsolver.pythonanywhere.com/presetWebService", "application/x-www-form-urlencoded", body);
    auto preset_details = r.body;

    std::string const request = createBody(options, preset_details);

    std::cout << "Fetching patch file from randomizer web service\n";
    auto const preset_response = RestClient::post("https://randommetroidsolver.pythonanywhere.com/randomizerWebService", "application/x-www-form-urlencoded", request);

    return preset_response.body;
}



std::vector<char> getFileContent(std::string const& path)
{
    std::ifstream file(path);
    return std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string patchRom(std::vector<char> & rom, std::string const& patch_data)
{
    std::string error;
    auto jsn = json11::Json::parse(patch_data, error);
    if (error.length())
    {
        std::cerr << "Json parse error: " << error << '\n';
        throw std::runtime_error(error);
    }
    else if (jsn.object_items().size() == 0)
    {
        throw std::runtime_error(patch_data);
    }

    std::string filename = "unnamed-rando.smc";
    for (auto const& [key, value] : jsn.object_items())
    {
        if (key == "errorMsg")
        {
            auto const &error = value.string_value();
            if (error.size())
            {
                throw std::runtime_error(error);
            }
        }
        else if (key == "fileName")
        {
             filename = value.string_value();
        }
        else
        {
            auto const address = stoul(key);
            if (address > rom.size())
            {
                throw std::runtime_error("Invalid address in patch file");
            }
            rom[address] = value.int_value();
        }
    }

    std::cout << "Patched ROM\n";

    return filename;
}

static const char usage[] =
R"(Randomizer Loader.

    Usage:
      rl <rom> <preset> <port> [--complexity=<cm> --seed=<sd> --progression=<ps> --output=<file>]
      rl <rom> <preset>        [--complexity=<cm> --seed=<sd> --progression=<ps> --output=<file>]
      rl <rom> <port>          [--load]
      rl (-h | --help)
      rl --version

    Options:
      -h --help               Show this screen.
      -c --complexity=<cm>    Complexity [default: simple]
      -s --seed=<sd>          Seed [default: 0]
      -p --progression=<ps>   Progression Speed [default: slow]
      -o --output=<file>      Output the patched ROM to this file.
      -l --load               Indicate to only load a room
)";

template<typename T>
Options parse_opts(T const& opts)
{
    Options options {
         opts.at("<rom>").asString()
        ,""                                     // Optional preset
        ,opts.at("--complexity").asString()
        ,opts.at("--seed").asString()
        ,opts.at("--progression").asString()
    };

    if (opts.at("<preset>").isString())
    {
        options.preset = opts.at("<preset>").asString();
    }

    std::cout << "Parsed opts\n";

    return options;
}

template<typename T>
static auto create_port(T const& opts)
{
    if (opts.at("<port>").isString())
    {
        std::cout << "Found port\n";
        return open_port(opts.at("<port>").asString());
    }

    return SerialPort(nullptr, sp_close);
}

int main(int argc, char * argv[]) try
{
    auto const args
        = docopt::docopt(usage,
                         { argv + 1, argv + argc },
                         true,                  // show help if requested
                         "Randomizer loader");  // version string

    auto const opts = parse_opts(args);

    std::cout << "Starting randomizer loader\n";
    auto port = create_port(args);

    auto rom = getFileContent(opts.rom);
    std::cout << "Loaded ROM: " << rom.size() << "\n";

    std::filesystem::path path(opts.rom);

    std::string filename = "";
    if (opts.preset != "")
    {
        auto preset = fetchPreset(opts);
        filename = patchRom(rom, preset);
    }
    else
    {
        filename = path.filename().string();
    }

    if (port)
    {
        std::cout << "Writing ROM to sd2snes: " << filename << "\n";
        write_file(port.get(), filename, rom);
    }

    if (args.at("--output").isString())
    {
        auto const output_path = std::filesystem::path(args.at("--output").asString()) / filename;
        std::ofstream file(output_path, std::ios::binary | std::ios::out);
        std::copy(rom.begin(), rom.end(), std::ostream_iterator<char>(file));
    }

    std::cout << "DONE!\n";
} catch (std::exception const& e) {
    std::cerr << "Randomizer-loader stopped with an error: " << e.what() << '\n';
}
