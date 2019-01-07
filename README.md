# sm-randomizer-loader
Just a small CLI program that automatically creates super metroid randomizers from http://randommetroidsolver.pythonanywhere.com and loads them on your sd2snes over usb.

Only tested on linux (arch and debian). To load over usb the sd2snes has to run usb2snes firmware.

# Dependencies
* libserialport
* libdocopt
* libfmt
* librestclient-cpp
* boost
* curl
* cmake
* stdc++fs

# Build and install
Build with cmake and make

# Usage
Randomizer Loader.

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
      
# Example

```
# Create a randomizer from "super_hard_preset" preset with hard complexity and fast progression and 
# load it to the sd2snes over usb
./rl ./super_metroid.smc super_hard_preset /dev/ttyACM0 --complexity=hard --progression=fast
```

```
# Create a randomizer from "super_hard_preset" preset, load it to the sd2snes over usb and
# put a copy in /home/samus
./rl ./super_metroid.smc super_hard_preset /dev/ttyACM0 -o /home/samus/
```

```
# Just load super_metroid.smc to sd2snes over usb
./rl ./super_metroid.smc /dev/ttyACM0 --load
```
