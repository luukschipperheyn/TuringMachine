# Turing Machine

## Author

Luuk Schipperheyn

## Description
2 channel Turing Machine. Similar to the [Music Thing one](https://musicthing.co.uk/pages/turing.html).

## Controls

| Control | Description | Comment |
| --- | --- | --- |
| CTRL in 1 - 3 | Turing Machine Knob | Noon: produce random cv, 5 o'clock: cv values lock into repeating sequence, 7 o'clock: cv values lock into inverting repeating sequence twice as long |
| CTRL in 2 - 4 | Length | set sequence length |
| Gate Ins | Advance Sequence |  |
| CV Outs | CV out | Either random or looping CV, depending on CTRL 1 and 3 |
| Encoder | Navigate Menu | Turn to move through steps. Press to enter/leave submenu (values), can be used to adjust amp value to attenuate outgoing cv |

## Building

Clone the [DaisyExamples repo](https://github.com/electro-smith/DaisyExamples) and move this project inside the `patch` directory. Then cd into the project directory and run `make clean; make; make program-dfu`.