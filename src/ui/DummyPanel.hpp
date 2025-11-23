#ifndef BLASTMAP_UI_DUMMYPANEL_HPP
#define BLASTMAP_UI_DUMMYPANEL_HPP

#include "UIPanel.hpp"

namespace blastmap {
namespace ui {

class DummyPanel : public UIPanel
{
public:
    explicit DummyPanel(std::string title);
    void render(int originX, int originY, int panelWidth, int panelHeight) override;
};

} // namespace ui
} // namespace blastmap

#endif // BLASTMAP_UI_DUMMYPANEL_HPP
