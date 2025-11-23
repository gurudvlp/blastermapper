#ifndef HEADER_VIEW_MAP
#define HEADER_VIEW_MAP 1

#include <cstdint>

namespace blastmap {
namespace view {

class MapRenderer
{
public:
	void SetLevelMode(int lvl, int lvlmode);
	void SetSelection(int x, int y);
	void Render(unsigned char darken = 0x01);

private:
	int level_ = 0;
	int mode_ = 0;
	int selectedX_ = 0;
	int selectedY_ = 0;
};

} // namespace view
} // namespace blastmap

#endif
