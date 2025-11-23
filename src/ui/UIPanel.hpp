#ifndef BLASTMAP_UI_UIPANEL_HPP
#define BLASTMAP_UI_UIPANEL_HPP

#include <string>
#include <utility>

namespace blastmap {
namespace ui {

class UIPanel
{
public:
    explicit UIPanel(std::string title)
        : title_(std::move(title))
    {
    }
    virtual ~UIPanel() = default;

    const std::string &title() const { return title_; }
    bool isOpen() const { return open_; }
    void open() { open_ = true; }
    void close() { open_ = false; }
    void toggle() { open_ = !open_; }

    virtual void render(int originX, int originY, int panelWidth, int panelHeight) = 0;
    virtual void onScroll(int delta) {}

protected:
    std::string title_;
    bool open_ = false;
};

} // namespace ui
} // namespace blastmap

#endif // BLASTMAP_UI_UIPANEL_HPP
