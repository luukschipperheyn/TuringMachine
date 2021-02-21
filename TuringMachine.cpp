#include "daisysp.h"
#include "daisy_patch.h"
#include <string>

using namespace daisy;
using namespace daisysp;

DaisyPatch patch;

uint16_t  values[] = {0b1, 0b1};
uint8_t amplitudes[] = {255, 255};
uint8_t lengths[] = {8, 8};
DaisyPatch::Ctrl turingControls[] = {DaisyPatch::CTRL_1, DaisyPatch::CTRL_3};
DaisyPatch::Ctrl lengthControls[] = {DaisyPatch::CTRL_2, DaisyPatch::CTRL_4};
DacHandle::Channel cvChannels[] = {DacHandle::Channel::ONE, DacHandle::Channel::TWO};

int  menuPos = 0;
bool inSubMenu = false;

void UpdateControls();
void UpdateOled();
void UpdateOutputs();


uint16_t mapValue(int index) {
    uint16_t value = values[index];
    uint8_t length = lengths[index];
    uint8_t amplitude = amplitudes[index];

    uint16_t maxValue = 0;
    for (int i = 0; i < length; i++) {
        maxValue = (maxValue << 1) | 1;
    }

    return round(((value & maxValue) / (maxValue / 4095.f)) * (amplitude / 255.f));
}


int main(void)
{
    patch.Init(); // Initialize hardware (daisy seed, and patch)
    patch.StartAdc();
    while(1)
    {
        UpdateControls();
        UpdateOled();
        UpdateOutputs();
    }
}

void UpdateControls()
{
    patch.ProcessAnalogControls();
    patch.ProcessDigitalControls();

    inSubMenu = patch.encoder.RisingEdge() ? !inSubMenu : inSubMenu;
    if (!inSubMenu) {
        menuPos += patch.encoder.Increment();
        menuPos = (menuPos % 2 + 2) % 2;
    } else {
        amplitudes[menuPos] += patch.encoder.Increment();
        amplitudes[menuPos] = amplitudes[menuPos] < 0 ? 0 : amplitudes[menuPos];
        amplitudes[menuPos] = amplitudes[menuPos] > 255 ? 255 : amplitudes[menuPos];
    }

    for (int i = 0; i < 2; i++) {
        uint8_t length = 1 + patch.GetKnobValue(lengthControls[i]) * 16;
        lengths[i] = length;

        if(patch.gate_input[i].Trig())
        {
            uint16_t value = values[i];
            uint8_t turing = patch.GetKnobValue(turingControls[i]) * 100;
            uint16_t lastBit = value & (0b1 << (length - 1));
            uint16_t newBit = lastBit >> (length - 1);
            int randomValue = rand() % 50;
            if (turing < 98) {
                if (turing < 2) {
                    newBit = newBit ^ 1;
                } else if (randomValue > abs(turing - 50)) {
                    newBit = rand() % 100 >= 50 ? 1 : 0;
                }
            }
            values[i] = value << 1 | newBit;
        }
    }
    
}

void UpdateOled()
{
    patch.display.Fill(false);

    char str [16];

    for (int i = 0; i < 2; i++) {
        for(int j = 0; j < lengths[i]; j++)
        {
            sprintf(str, "%u", values[i] >> j & 1);
            patch.display.SetCursor(120 - j * 8, i * 32);
            patch.display.WriteString(str, Font_6x8, true);
        }

        sprintf(str, "amp: %d", amplitudes[i]);
        patch.display.SetCursor(80, i * 32 + 16);
        patch.display.WriteString(str, Font_6x8, true);
    }
    
    //cursor
    sprintf(str, "%s", inSubMenu ? "@" : "o");
    patch.display.SetCursor(70, menuPos == 0 ? 16 : 48);
    patch.display.WriteString(str, Font_6x8, true);

    patch.display.Update();
}

void UpdateOutputs()
{
    for (int i = 0; i < 2; i++) {
        patch.seed.dac.WriteValue(cvChannels[i], mapValue(i));
    }
}
