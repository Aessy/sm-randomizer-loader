# sm-randomizer-loader
Commandline interface for creating Super Metroid Randomizer from http://randommetroidsolver.pythonanywhere.com and loading it on a usb2snes patched sd2snes.

# Dependencies
* libserialport
* libdocopt
* libfmt
* librestclient-cpp
* boost
* curl
* cmake

# Usage
Randomizer Loader.

    Usage:
      rl <rom> <preset> <port> [--complexity=<cm> | --seed=<sd> | --progression=<ps>]
      rl (-h | --help)
      rl --version

    Options:
      -h --help               Show this screen.
      -c --complexity=<cm>    Compexity [default: simple]
      -s --seed=<sd>          Seed [default: 0]
      -p --progression=<ps>   Progression Speed [default: slow]
      
# Example

```
./rl ./super_metroid.smc super_hard_preset /dev/ttyACM0 --complexity=hard --progression=fast
```
