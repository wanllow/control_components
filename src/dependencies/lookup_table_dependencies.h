#pragma once

enum class SearchMethod
{
    seq = 0, // sequential search, from begin to end
    bin = 1, // binary search
    near = 2 // sequential search based on last value
};

enum class InterpMethod
{
    nearest = 0, // use nearest point value
    linear = 1,  // linear interpolation
    next = 2,    // use the next point value
    previous = 3 // use the previous point value
};

enum class ExtrapMethod
{
    clip = 0,   // use end value
    linear = 1, // linear extrpolation
    specify = 2 // user specify the values
};