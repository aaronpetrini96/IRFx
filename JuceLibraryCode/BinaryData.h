/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Bypass_png;
    const int            Bypass_pngSize = 3321;

    extern const char*   CancelX_png;
    const int            CancelX_pngSize = 113569;

    extern const char*   Knob_10brighter_png;
    const int            Knob_10brighter_pngSize = 2193906;

    extern const char*   Knob_10_png;
    const int            Knob_10_pngSize = 1753539;

    extern const char*   LatoMedium_ttf;
    const int            LatoMedium_ttfSize = 663564;

    extern const char*   LatoRegular_ttf;
    const int            LatoRegular_ttfSize = 120196;

    extern const char*   Mute_png;
    const int            Mute_pngSize = 95071;

    extern const char*   screw_png;
    const int            screw_pngSize = 12789;

    extern const char*   Ver_flat_slider_png;
    const int            Ver_flat_slider_pngSize = 294674;

    extern const char*   Ver_slider_png;
    const int            Ver_slider_pngSize = 6785719;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 10;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
