#ifndef HEADER_VIEW_PALETTE
#define HEADER_VIEW_PALETTE 1

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t Palette[64][3];

void InitializePalette();

#ifdef __cplusplus
}

namespace view {
struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

inline Color ColorAt(size_t index)
{
	Color color{Palette[index][0], Palette[index][1], Palette[index][2]};
	return color;
}

inline constexpr size_t PaletteSize = 64;
}
#endif

#endif
