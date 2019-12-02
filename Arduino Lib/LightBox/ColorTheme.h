#ifndef LIGHTBOX_COLOR_THEME_H
#define LIGHTBOX_COLOR_THEME_H

enum ColorIndex
{
    Primary = 0,
    Secondary = 1
};

const uint16_t PRIMARY[16] = 
{
    CL(13, 170, 255),
    CL(13, 207, 255),
    CL(13, 219, 255),
    CL(13, 239, 255),

    CL(0, 224, 240),
    CL(0, 240, 240),
    CL(28, 255, 255),
    CL(28, 255, 244),

    CL(38, 255, 230),
    CL(38, 255, 212),
    CL(30, 247, 197),
    CL(23, 252, 176),

    CL(15, 242, 151),
    CL(15, 242, 102),
    CL(0, 232, 85),
    CL(0, 222, 63)
};

const uint16_t SECONDARY[16] = 
{
    CL(255, 145, 209),
    CL(250, 162, 199),
    CL(255, 186, 210),
    CL(255, 209, 218),

    CL(252, 167, 175),
    CL(247, 156, 156),
    CL(247, 153, 121),
    CL(255, 133, 92),

    CL(255, 163, 92),
    CL(247, 145, 67),
    CL(252, 169, 86),
    CL(255, 195, 135),

    CL(255, 211, 135),
    CL(250, 200, 112),
    CL(255, 207, 94),
    CL(242, 219, 68)
};

class ColorTheme
{
public:

    static uint16_t get(uint8_t index, uint8_t colorType) 
    {
        if(colorType == ColorIndex::Primary)
            return PRIMARY[index];
        else
            return SECONDARY[index];
    }

};


#endif