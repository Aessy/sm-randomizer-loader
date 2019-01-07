# sm-randomizer-loader
Just a small CLI program that automatically creates super metroid randomizers from http://randommetroidsolver.pythonanywhere.com. It can also load them automatically on your sd2snes over usb.

Only tested on linux (arch and debian)
To load over usb the sd2snes has to be loaded with usb2snes firmware.

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
