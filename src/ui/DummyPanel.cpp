#include "DummyPanel.hpp"

#include <GL/gl.h>

namespace blastmap {
namespace ui {

DummyPanel::DummyPanel(std::string title)
    : UIPanel(std::move(title))
{
}

void DummyPanel::render(int originX, int originY, int panelWidth, int panelHeight)
{
    if(!isOpen()) { return; }

    glColor4f(0.18f, 0.18f, 0.28f, 0.92f);
    glBegin(GL_QUADS);
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY + panelHeight));
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY + panelHeight));
    glEnd();

    glColor4f(0.4f, 0.4f, 0.6f, 0.92f);
    const int headerHeight = 24;
    glBegin(GL_QUADS);
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY + panelHeight - headerHeight));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY + panelHeight - headerHeight));
        glVertex2f(static_cast<float>(originX + panelWidth), static_cast<float>(originY + panelHeight));
        glVertex2f(static_cast<float>(originX), static_cast<float>(originY + panelHeight));
    glEnd();
}

} // namespace ui
} // namespace blastmap
