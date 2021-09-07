// based on https://github.com/nem0/LumixEngine/blob/master/external/imgui/imgui_dock.inl
// modified from https://bitbucket.org/duangle/liminal/src/tip/src/liminal/imgui_dock.cpp

#include "imgui.h"
#define IMGUI_DEFINE_PLACEMENT_NEW
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "Editor/ImGui/imgui_dock.h"
#include <string>

using namespace ImGui;

struct DockContext
{
    enum class EndAction {
        None = 0,
        Panel,
        End,
        EndChild
    };

    enum class Status {
        Docked = 0,
        Float,
        Dragged
    };

    struct Dock
    {
        Dock()
            : id(0)
            , nextTab(nullptr)
            , prevTab(nullptr)
            , parent(nullptr)
            , pos(0, 0)
            , size(-1, -1)
            , active(true)
            , status(Status::Float)
            , label(nullptr)
            , opened(false)
        
        {
            location[0] = 0;
            children[0] = children[1] = nullptr;
        }

        ~Dock()
        {
            MemFree(label);
        }

        [[nodiscard]] ImVec2 getMinSize() const
        {
            if (!children[0])
                return {16, 16 + GetTextLineHeightWithSpacing()};

            ImVec2 minSize0 = children[0]->getMinSize();
            ImVec2 minSize1 = children[1]->getMinSize();
            return isHorizontal() ? ImVec2(minSize0.x + minSize1.x, ImMax(minSize0.y, minSize1.y))
                                  : ImVec2(ImMax(minSize0.x, minSize1.x), minSize0.y + minSize1.y);
        }

        [[nodiscard]] bool isHorizontal() const { return children[0]->pos.x < children[1]->pos.x; }

        void setParent(Dock* dock)
        {
            parent = dock;
            for (Dock *tmp = prevTab; tmp; tmp = tmp->prevTab)
                tmp->parent = dock;
            for (Dock *tmp = nextTab; tmp; tmp = tmp->nextTab)
                tmp->parent = dock;
        }
        
        Dock &getRoot()
        {
            Dock *dock = this;
            while (dock->parent)
                dock = dock->parent;
            return *dock;
        }

        Dock &getSibling()
        {
            IM_ASSERT(parent);
            if (parent->children[0] == &getFirstTab())
                return *parent->children[1];
            return *parent->children[0];
        }

        Dock &getFirstTab()
        {
            Dock *tmp = this;
            while (tmp->prevTab)
                tmp = tmp->prevTab;
            return *tmp;
        }

        void setActive()
        {
            active = true;
            for (Dock* tmp = prevTab; tmp; tmp = tmp->prevTab)
                tmp->active = false;
            for (Dock* tmp = nextTab; tmp; tmp = tmp->nextTab)
                tmp->active = false;
        }

        [[nodiscard]] bool isContainer() const { return children[0] != nullptr; }

        void setChildrenPosSize(const ImVec2 &_pos, const ImVec2 &_size)
        {
            ImVec2 childSize = children[0]->size;
            if (isHorizontal())
            {
                childSize.y = _size.y;
                childSize.x = (float)int(_size.x * children[0]->size.x / (children[0]->size.x + children[1]->size.x));
                if (childSize.x < children[0]->getMinSize().x)
                    childSize.x = children[0]->getMinSize().x;
                else if (_size.x - childSize.x < children[1]->getMinSize().x)
                    childSize.x = _size.x - children[1]->getMinSize().x;
                children[0]->setPosSize(_pos, childSize);

                childSize.x = _size.x - children[0]->size.x;
                ImVec2 p = _pos;
                p.x += children[0]->size.x;
                children[1]->setPosSize(p, childSize);
            }
            else
            {
                childSize.x = _size.x;
                childSize.y = (float)int(_size.y * children[0]->size.y / (children[0]->size.y + children[1]->size.y));
                if (childSize.y < children[0]->getMinSize().y)
                    childSize.y = children[0]->getMinSize().y;
                else if (_size.y - childSize.y < children[1]->getMinSize().y)
                    childSize.y = _size.y - children[1]->getMinSize().y;
                children[0]->setPosSize(_pos, childSize);

                childSize.y = _size.y - children[0]->size.y;
                ImVec2 p = _pos;
                p.y += children[0]->size.y;
                children[1]->setPosSize(p, childSize);
            }
        }

        void setPosSize(const ImVec2 &_pos, const ImVec2 &_size)
        {
            size = _size;
            pos = _pos;
            for (Dock *tmp = prevTab; tmp; tmp = tmp->prevTab)
            {
                tmp->size = _size;
                tmp->pos = _pos;
            }
            for (Dock *tmp = nextTab; tmp; tmp = tmp->nextTab)
            {
                tmp->size = _size;
                tmp->pos = _pos;
            }

            if (!isContainer())
                return;
            setChildrenPosSize(_pos, _size);
        }

        char *label;
        ImU32 id;
        Dock *nextTab;
        Dock *prevTab;
        Dock *children[2];
        Dock *parent;
        bool active;
        ImVec2 pos;
        ImVec2 size;
        Status status;
        int lastFrame;
        int invalidFrames;
        char location[16];
        bool opened;
        bool first;
    };

    ImVector<Dock*> docks;
    ImVec2 dragOffset;
    Dock *current;
    Dock *nextParent;
    int lastFrame;
    EndAction endAction;
    ImVec2 workspacePos;
    ImVec2 workspaceSize;
    ImGuiDockSlot nextDockSlot;

    DockContext()
        : current(nullptr)
        , nextParent(nullptr)
        , lastFrame(0)
        , nextDockSlot(ImGuiDockSlot_Tab)
    {}

    ~DockContext() = default;

    Dock &getDock(const char *label, bool opened)
    {
        ImU32 id = ImHash(label, 0);
        for(auto &dock : docks)
        {
            if (dock->id == id)
                return *dock;
        }

        Dock *newDock = (Dock*)MemAlloc(sizeof(Dock));
        IM_PLACEMENT_NEW(newDock) Dock();
        docks.push_back(newDock);
        newDock->label = ImStrdup(label);
        IM_ASSERT(newDock->label);
        newDock->id = id;
        newDock->setActive();
        newDock->status = (docks.size() == 1) ? Status::Docked : Status::Float;
        newDock->pos = ImVec2(0, 0);
        newDock->size = GetIO().DisplaySize;
        newDock->opened = opened;
        newDock->first = true;
        newDock->lastFrame = 0;
        newDock->invalidFrames = 0;
        newDock->location[0] = 0;

        return *newDock;
    }

    void putInBackground()
    {
        ImGuiWindow *win = GetCurrentWindow();
        ImGuiContext &g = *GImGui;
        if (g.Windows[0] == win)
            return;

        for (int i = 0; i < g.Windows.Size; ++i)
        {
            if (g.Windows[i] == win)
            {
                for (int j = i - 1; j >= 0; --j)
                {
                    g.Windows[j + 1] = g.Windows[j];
                }
                g.Windows[0] = win;
                break;
            }
        }
    }

    void splits()
    {
        if (GetFrameCount() == lastFrame)
            return;
        lastFrame = GetFrameCount();

        putInBackground();
        
        for (auto &dock : docks)
        {
            if (!dock->parent && (dock->status == Status::Docked))
                dock->setPosSize(workspacePos, workspaceSize);
        }

        ImU32 color = GetColorU32(ImGuiCol_Button);
        ImU32 colorHovered = GetColorU32(ImGuiCol_ButtonHovered);
        ImDrawList *drawList = GetWindowDrawList();
        ImGuiIO &io = GetIO();
        for (int i = 0; i < docks.size(); ++i)
        {
            Dock &dock = *docks[i];
            if (!dock.isContainer()) continue;

            PushID(i);
            if (!IsMouseDown(0))
                dock.status = Status::Docked;
            
            ImVec2 pos0 = dock.children[0]->pos;
            ImVec2 pos1 = dock.children[1]->pos;
            ImVec2 size0 = dock.children[0]->size;
            ImVec2 size1 = dock.children[1]->size;
            
            ImGuiMouseCursor cursor;

            ImVec2 dSize(0, 0);
            ImVec2 minSize0 = dock.children[0]->getMinSize();
            ImVec2 minSize1 = dock.children[1]->getMinSize();
            if (dock.isHorizontal())
            {
                cursor = ImGuiMouseCursor_ResizeEW;
                SetCursorScreenPos(ImVec2(dock.pos.x + size0.x, dock.pos.y));
                InvisibleButton("split", ImVec2(3, dock.size.y));
                if (dock.status == Status::Dragged)
                    dSize.x = io.MouseDelta.x;
                dSize.x = -ImMin(-dSize.x, dock.children[0]->size.x - minSize0.x);
                dSize.x = ImMin(dSize.x, dock.children[1]->size.x - minSize1.x);
                size0 += dSize;
                size1 -= dSize;
                pos0 = dock.pos;
                pos1.x = pos0.x + size0.x;
                pos1.y = dock.pos.y;
                size0.y = size1.y = dock.size.y;
                size1.x = ImMax(minSize1.x, dock.size.x - size0.x);
                size0.x = ImMax(minSize0.x, dock.size.x - size1.x);
            }
            else
            {
                cursor = ImGuiMouseCursor_ResizeNS;
                SetCursorScreenPos(ImVec2(dock.pos.x, dock.pos.y + size0.y - 3));
                InvisibleButton("split", ImVec2(dock.size.x, 3));
                if (dock.status == Status::Dragged)
                    dSize.y = io.MouseDelta.y;
                dSize.y = -ImMin(-dSize.y, dock.children[0]->size.y - minSize0.y);
                dSize.y = ImMin(dSize.y, dock.children[1]->size.y - minSize1.y);
                size0 += dSize;
                size1 -= dSize;
                pos0 = dock.pos;
                pos1.x = dock.pos.x;
                pos1.y = pos0.y + size0.y;
                size0.x = size1.x = dock.size.x;
                size1.y = ImMax(minSize1.y, dock.size.y - size0.y);
                size0.y = ImMax(minSize0.y, dock.size.y - size1.y);
            }

            dock.children[0]->setPosSize(pos0, size0);
            dock.children[1]->setPosSize(pos1, size1);

            if (IsItemHovered(ImGuiHoveredFlags_RectOnly))
                SetMouseCursor(cursor);

            if (IsItemHovered(ImGuiHoveredFlags_RectOnly) && IsMouseClicked(0))
                dock.status = Status::Dragged;

            drawList->AddRectFilled(GetItemRectMin(), GetItemRectMax(), IsItemHovered(ImGuiHoveredFlags_RectOnly) ? colorHovered : color);
            PopID();
        }
    }

    void checkNonexistent()
    {
        int frameLimit = ImMax(0, ImGui::GetFrameCount() - 2);
        for (auto dock : docks)
        {
            if (dock->isContainer()) continue;
            if (dock->status == Status::Float) continue;

            if (dock->lastFrame < frameLimit)
            {
                ++dock->invalidFrames;
                if (dock->invalidFrames > 2)
                {
                    doUndock(*dock);
                    dock->status = Status::Float;
                }
                return;
            }
            dock->invalidFrames = 0;
        }
    }

    [[nodiscard]] Dock *getDockAt() const
    {
        for (auto dock : docks)
        {
            if (dock->isContainer()) continue;
            if (dock->status != Status::Docked) continue;

            if (IsMouseHoveringRect(dock->pos, dock->pos + dock->size, false))
                return dock;
        }

        return nullptr;
    }

    static ImRect getDockedRect(const ImRect &rect, ImGuiDockSlot dockSlot)
    {
        ImVec2 halfSize = rect.GetSize() * 0.5f;
        switch (dockSlot)
        {
        default: return rect;
        case ImGuiDockSlot_Top: return ImRect(rect.Min, ImVec2(rect.Max.x, rect.Min.y + halfSize.y));
        case ImGuiDockSlot_Right: return ImRect(rect.Min + ImVec2(halfSize.x, 0), rect.Max);
        case ImGuiDockSlot_Bottom: return ImRect(rect.Min + ImVec2(0, halfSize.y), rect.Max);
        case ImGuiDockSlot_Left: return ImRect(rect.Min, ImVec2(rect.Min.x + halfSize.x, rect.Max.y));
        }
    }

    static ImRect getSlotRect(ImRect parentRect, ImGuiDockSlot dockSlot)
    {
        ImVec2 size = parentRect.Max - parentRect.Min;
        ImVec2 center = parentRect.Min + size * 0.5f;
        switch (dockSlot)
        {
        default: return ImRect(center - ImVec2(20, 20), center + ImVec2(20, 20));
        case ImGuiDockSlot_Top: return ImRect(center + ImVec2(-20, -50), center + ImVec2(20, -30));
        case ImGuiDockSlot_Right: return ImRect(center + ImVec2(30, -20), center + ImVec2(50, 20));
        case ImGuiDockSlot_Bottom: return ImRect(center + ImVec2(-20, +30), center + ImVec2(20, 50));
        case ImGuiDockSlot_Left: return ImRect(center + ImVec2(-50, -20), center + ImVec2(-30, 20));
        }
    }

    static ImRect getSlotRectOnBorder(ImRect parentRect, ImGuiDockSlot dockSlot)
    {
        ImVec2 size = parentRect.Max - parentRect.Min;
        ImVec2 center = parentRect.Min + size * 0.5f;
        switch (dockSlot)
        {
        case ImGuiDockSlot_Top:
            return ImRect(ImVec2(center.x - 20, parentRect.Min.y + 10),
                ImVec2(center.x + 20, parentRect.Min.y + 30));
        case ImGuiDockSlot_Left:
            return ImRect(ImVec2(parentRect.Min.x + 10, center.y - 20),
                ImVec2(parentRect.Min.x + 30, center.y + 20));
        case ImGuiDockSlot_Bottom:
            return ImRect(ImVec2(center.x - 20, parentRect.Max.y - 30),
                ImVec2(center.x + 20, parentRect.Max.y - 10));
        case ImGuiDockSlot_Right:
            return ImRect(ImVec2(parentRect.Max.x - 30, center.y - 20),
                ImVec2(parentRect.Max.x - 10, center.y + 20));
        default: IM_ASSERT(false);
        }
        IM_ASSERT(false);
        return ImRect();
    }

    [[nodiscard]] Dock *getRootDock()
    {
        for (auto &dock : docks)
        {
            if (!dock->parent && (dock->status == Status::Docked || dock->children[0]))
                return dock;
        }

        return nullptr;
    }

    bool dockSlots(Dock &dock, Dock *destDock, const ImRect &rect, bool onBorder)
    {
        ImDrawList *canvas = GetWindowDrawList();
        ImU32 color = GetColorU32(ImGuiCol_Button);
        ImU32 colorHovered = GetColorU32(ImGuiCol_ButtonHovered);
        ImVec2 mousePos = GetIO().MousePos;
        for (int i = 0; i < (onBorder ? 4 : 5); ++i)
        {
            ImRect r = onBorder ? getSlotRectOnBorder(rect, (ImGuiDockSlot)i) : getSlotRect(rect, (ImGuiDockSlot)i);
            bool hovered = r.Contains(mousePos);
            canvas->AddRectFilled(r.Min, r.Max, hovered ? colorHovered : color);
            if (!hovered) continue;

            if (!IsMouseDown(0))
            {
                doDock(dock, destDock ? destDock : getRootDock(), (ImGuiDockSlot)i);
                return true;
            }
            ImRect dockedRect = getDockedRect(rect, (ImGuiDockSlot)i);
            canvas->AddRectFilled(dockedRect.Min, dockedRect.Max, GetColorU32(ImGuiCol_Button));
        }
        return false;
    }

    void handleDrag(Dock &dock)
    {
        Dock *destDock = getDockAt();

        static const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                                                    ImGuiWindowFlags_AlwaysAutoResize;

        SetNextWindowSize(ImVec2(0, 0));
        SetNextWindowBgAlpha(0.f);
        Begin("##Overlay", nullptr, windowFlags);

        ImDrawList *canvas = GetWindowDrawList();
        canvas->PushClipRectFullScreen();

        ImU32 dockedColor = GetColorU32(ImGuiCol_FrameBg);
        dockedColor = (dockedColor & 0x00ffFFFF) | 0x80000000;
        dock.pos = GetIO().MousePos - dragOffset;
        if (destDock && dockSlots(dock, destDock, ImRect(destDock->pos, destDock->pos + destDock->size), false))
        {
            canvas->PopClipRect();
            End();
            return;
        }
        if (dockSlots(dock, nullptr, ImRect(workspacePos, workspacePos + workspaceSize), true))
        {
            canvas->PopClipRect();
            End();
            return;
        }
        canvas->AddRectFilled(dock.pos, dock.pos + dock.size, dockedColor);
        canvas->PopClipRect();

        if (!IsMouseDown(0))
        {
            dock.status = Status::Float;
            dock.location[0] = 0;
            dock.setActive();
        }

        End();
    }

    void fillLocation(Dock &dock)
    {
        if (dock.status == Status::Float) return;
        char *c = dock.location;
        Dock *tmp = &dock;
        while (tmp->parent)
        {
            *c = getLocationCode(tmp);
            tmp = tmp->parent;
            ++c;
        }
        *c = 0;
    }

    void doUndock(Dock &dock)
    {
        if (dock.prevTab)
            dock.prevTab->setActive();
        else if (dock.nextTab)
            dock.nextTab->setActive();
        else
            dock.active = false;
        Dock *container = dock.parent;

        if (container)
        {
            Dock &sibling = dock.getSibling();
            if (container->children[0] == &dock)
                container->children[0] = dock.nextTab;
            else if (container->children[1] == &dock)
                container->children[1] = dock.nextTab;

            bool removeContainer = !container->children[0] || !container->children[1];
            if (removeContainer)
            {
                if (container->parent)
                {
                    Dock *&child = container->parent->children[0] == container
                                   ? container->parent->children[0]
                                   : container->parent->children[1];
                    child = &sibling;
                    child->setPosSize(container->pos, container->size);
                    child->setParent(container->parent);
                }
                else
                {
                    if (container->children[0])
                    {
                        container->children[0]->setParent(nullptr);
                        container->children[0]->setPosSize(container->pos, container->size);
                    }
                    if (container->children[1])
                    {
                        container->children[1]->setParent(nullptr);
                        container->children[1]->setPosSize(container->pos, container->size);
                    }
                }
                for (int i = 0; i < docks.size(); ++i)
                {
                    if (docks[i] == container)
                    {
                        docks.erase(docks.begin() + i);
                        break;
                    }
                }
                if (container == nextParent)
                    nextParent = nullptr;
                container->~Dock();
                MemFree(container);
            }
        }
        if (dock.prevTab)
            dock.prevTab->nextTab = dock.nextTab;
        if (dock.nextTab)
            dock.nextTab->prevTab = dock.prevTab;
        dock.parent = nullptr;
        dock.prevTab = dock.nextTab = nullptr;
    }

    void drawTabbarListButton(Dock& dock)
    {
        if (!dock.nextTab) return;

        ImDrawList *drawList = GetWindowDrawList();
        if (InvisibleButton("list", ImVec2(16, 16)))
            OpenPopup("tab_list_popup");
        if (BeginPopup("tab_list_popup"))
        {
            Dock *tmp = &dock;
            while (tmp)
            {
                bool dummy = false;
                if (Selectable(tmp->label, &dummy))
                {
                    tmp->setActive();
                    nextParent = tmp;
                }
                tmp = tmp->nextTab;
            }
            EndPopup();
        }

        bool hovered = IsItemHovered();
        ImVec2 min = GetItemRectMin();
        ImVec2 max = GetItemRectMax();
        ImVec2 center = (min + max) * 0.5f;
        ImU32 textColor = GetColorU32(ImGuiCol_Text);
        ImU32 colorActive = GetColorU32(ImGuiCol_FrameBgActive);
        drawList->AddRectFilled(ImVec2(center.x - 4, min.y + 3),
                                ImVec2(center.x + 4, min.y + 5),
                                hovered ? colorActive : textColor);
        drawList->AddTriangleFilled(ImVec2(center.x - 4, min.y + 7),
                                    ImVec2(center.x + 4, min.y + 7),
                                    ImVec2(center.x, min.y + 12),
                                    hovered ? colorActive : textColor);
    }

    bool tabbar(Dock &dock, bool closeButton)
    {
        float tabbarHeight = 2 * GetTextLineHeightWithSpacing();
        ImVec2 size(dock.size.x, tabbarHeight);
        bool tabClosed = false;

        SetCursorScreenPos(dock.pos);
        char tmp[20];
        ImFormatString(tmp, IM_ARRAYSIZE(tmp), "tabs%d", (int)dock.id);
        if (BeginChild(tmp, size, true))
        {
            Dock *dockTab = &dock;

            ImDrawList *drawList = GetWindowDrawList();
            ImU32 color = GetColorU32(ImGuiCol_FrameBg);
            ImU32 colorActive = GetColorU32(ImGuiCol_FrameBgActive);
            ImU32 colorHovered = GetColorU32(ImGuiCol_FrameBgHovered);
            ImU32 textColor = GetColorU32(ImGuiCol_Text);
            float lineHeight = GetTextLineHeightWithSpacing();
            float tabBase;

            drawTabbarListButton(dock);

            while (dockTab)
            {
                SameLine(0, 15);

                const char *textEnd = FindRenderedTextEnd(dockTab->label);
                ImVec2 size(CalcTextSize(dockTab->label, textEnd).x, lineHeight);
                if (InvisibleButton(dockTab->label, size))
                {
                    dockTab->setActive();
                    nextParent = dockTab;
                }

                if (IsItemActive() && IsMouseDragging(0))
                {
                    dragOffset = GetMousePos() - dockTab->pos;
                    doUndock(*dockTab);
                    dockTab->status = Status::Dragged;
                }

                bool hovered = IsItemHovered(ImGuiHoveredFlags_RectOnly);
                ImVec2 pos = GetItemRectMin();
                if (dockTab->active && closeButton)
                {
                    size.x += 16 + GetStyle().ItemSpacing.x;
                    SameLine();
                    tabClosed = InvisibleButton("close", ImVec2(16, 16));
                    ImVec2 center = (GetItemRectMin() + GetItemRectMax()) * 0.5f;
                    drawList->AddLine(
                        center + ImVec2(-3.5f, -3.5f), center + ImVec2(3.5f, 3.5f), textColor);
                    drawList->AddLine(
                        center + ImVec2(3.5f, -3.5f), center + ImVec2(-3.5f, 3.5f), textColor);
                }
                tabBase = pos.y;
                drawList->PathClear();
                drawList->PathLineTo(pos + ImVec2(-15, size.y));
                drawList->PathBezierCurveTo(
                    pos + ImVec2(-10, size.y), pos + ImVec2(-5, 0), pos + ImVec2(0, 0), 10);
                drawList->PathLineTo(pos + ImVec2(size.x, 0));
                drawList->PathBezierCurveTo(pos + ImVec2(size.x + 5, 0),
                    pos + ImVec2(size.x + 10, size.y),
                    pos + ImVec2(size.x + 15, size.y),
                                            10);
                drawList->PathFillConvex(
                        hovered ? colorHovered : (dockTab->active ? colorActive : color));
                drawList->AddText(pos + ImVec2(0, 1), textColor, dockTab->label, textEnd);

                dockTab = dockTab->nextTab;
            }
            ImVec2 cp(dock.pos.x, tabBase + lineHeight);
            drawList->AddLine(cp, cp + ImVec2(dock.size.x, 0), color);
        }
        EndChild();
        return tabClosed;
    }

    static void setDockPosSize(Dock &dest, Dock &dock, ImGuiDockSlot dockSlot, Dock &container)
    {
        IM_ASSERT(!dock.prevTab && !dock.nextTab && !dock.children[0] && !dock.children[1]);

        dest.pos = container.pos;
        dest.size = container.size;
        dock.pos = container.pos;
        dock.size = container.size;

        switch (dockSlot)
        {
        case ImGuiDockSlot_Bottom:
            dest.size.y *= 0.5f;
            dock.size.y *= 0.5f;
            dock.pos.y += dest.size.y;
            break;
        case ImGuiDockSlot_Right:
            dest.size.x *= 0.5f;
            dock.size.x *= 0.5f;
            dock.pos.x += dest.size.x;
            break;
        case ImGuiDockSlot_Left:
            dest.size.x *= 0.5f;
            dock.size.x *= 0.5f;
            dest.pos.x += dock.size.x;
            break;
        case ImGuiDockSlot_Top:
            dest.size.y *= 0.5f;
            dock.size.y *= 0.5f;
            dest.pos.y += dock.size.y;
            break;
        default: IM_ASSERT(false); break;
        }
        dest.setPosSize(dest.pos, dest.size);

        if (container.children[1]->pos.x < container.children[0]->pos.x ||
            container.children[1]->pos.y < container.children[0]->pos.y)
        {
            Dock *tmp = container.children[0];
            container.children[0] = container.children[1];
            container.children[1] = tmp;
        }
    }

    void doDock(Dock &dock, Dock *dest, ImGuiDockSlot dockSlot)
    {
        IM_ASSERT(!dock.parent);
        if (!dest)
        {
            dock.status = Status::Docked;
            dock.setPosSize(workspacePos, workspaceSize);
        }
        else if (dockSlot == ImGuiDockSlot_Tab)
        {
            Dock *tmp = dest;
            while (tmp->nextTab) tmp = tmp->nextTab;

            auto inLinkList = [](const Dock *linkList, const Dock *checkNode){
                bool isLinkNode = (linkList == checkNode);

                const Dock *temp = linkList;
                while (!isLinkNode && temp->prevTab)
                {
                    temp = temp->prevTab;
                    isLinkNode = (temp == checkNode);
                }

                temp = linkList;

                while (!isLinkNode && temp->nextTab)
                {
                    temp = temp->nextTab;
                    isLinkNode = (temp == checkNode);
                }

                return isLinkNode;
            };

            if (!inLinkList(dest , &dock))
            {
                tmp->nextTab = &dock;
                dock.prevTab = tmp;
                dock.size = tmp->size;
                dock.pos = tmp->pos;
                dock.parent = dest->parent;
                dock.status = Status::Docked;
            }
        }
        else if (dockSlot == ImGuiDockSlot_None)
            dock.status = Status::Float;
        else
        {
            Dock *container = (Dock*)MemAlloc(sizeof(Dock));
            IM_PLACEMENT_NEW(container) Dock();
            docks.push_back(container);
            container->children[0] = &dest->getFirstTab();
            container->children[1] = &dock;
            container->nextTab = nullptr;
            container->prevTab = nullptr;
            container->parent = dest->parent;
            container->size = dest->size;
            container->pos = dest->pos;
            container->status = Status::Docked;
            container->label = ImStrdup("");

            if (!dest->parent)
            {
            }
            else if (&dest->getFirstTab() == dest->parent->children[0])
                dest->parent->children[0] = container;
            else
                dest->parent->children[1] = container;

            dest->setParent(container);
            dock.parent = container;
            dock.status = Status::Docked;

            setDockPosSize(*dest, dock, dockSlot, *container);
        }
        dock.setActive();
    }

    void rootDock(const ImVec2 &pos, const ImVec2 &size)
    {
        Dock *root = getRootDock();
        if (!root) return;

        ImVec2 minSize = root->getMinSize();
        ImVec2 requestedSize = size;
        root->setPosSize(pos, ImMax(minSize, requestedSize));
    }

    static ImGuiDockSlot getSlotFromLocationCode(char code)
    {
        switch (code)
        {
        case '1': return ImGuiDockSlot_Left;
        case '2': return ImGuiDockSlot_Top;
        case '3': return ImGuiDockSlot_Bottom;
        default: return ImGuiDockSlot_Right;
        }
    }

    static char getLocationCode(Dock *dock)
    {
        if (!dock) return '0';

        if (dock->parent->isHorizontal())
        {
            if (dock->pos.x < dock->parent->children[0]->pos.x) return '1';
            if (dock->pos.x < dock->parent->children[1]->pos.x) return '1';
            return '0';
        }
        else
        {
            if (dock->pos.y < dock->parent->children[0]->pos.y) return '2';
            if (dock->pos.y < dock->parent->children[1]->pos.y) return '2';
            return '3';
        }
    }

    void tryDockToStoredLocation(Dock &dock)
    {
        if (dock.status == Status::Docked) return;
        if (dock.location[0] == 0) return;

        Dock *tmp = getRootDock();
        if (!tmp) return;

        Dock *prev = nullptr;
        char *c = dock.location + strlen(dock.location) - 1;
        while (c >= dock.location && tmp)
        {
            prev = tmp;
            tmp = *c == getLocationCode(tmp->children[0]) ? tmp->children[0] : tmp->children[1];
            if(tmp) --c;
        }
        doDock(dock, tmp ? tmp : prev, tmp ? ImGuiDockSlot_Tab : getSlotFromLocationCode(*c));
    }

    bool begin(const char *label, bool *opened, ImGuiWindowFlags extraFlags)
    {
        ImGuiDockSlot nextSlot = nextDockSlot;
        nextDockSlot = ImGuiDockSlot_Tab;
        Dock &dock = getDock(label, !opened || *opened);
        if (!dock.opened && (!opened || *opened)) tryDockToStoredLocation(dock);
        dock.lastFrame = ImGui::GetFrameCount();
        if (strcmp(dock.label, label) != 0)
        {
            MemFree(dock.label);
            dock.label = ImStrdup(label);
        }

        endAction = EndAction::None;

        bool prevOpened = dock.opened;
        bool first = dock.first;
        if (dock.first && opened) *opened = dock.opened;
        dock.first = false;
        if (opened && !*opened)
        {
            if (dock.status != Status::Float)
            {
                fillLocation(dock);
                doUndock(dock);
                dock.status = Status::Float;
            }
            dock.opened = false;
            return false;
        }
        dock.opened = true;

        checkNonexistent();
        
        if (first || (prevOpened != dock.opened))
        {
            Dock *root = nextParent ? nextParent : getRootDock();
            if (root && (&dock != root) && !dock.parent)
                doDock(dock, root, nextSlot);
            nextParent = &dock;
        }

        current = &dock;
        if (dock.status == Status::Dragged)
            handleDrag(dock);

        bool isFloat = dock.status == Status::Float;

        if (isFloat)
        {
            SetNextWindowPos(dock.pos);
            SetNextWindowSize(dock.size);
            SetNextWindowBgAlpha(-1.0f);
            bool ret = Begin(label, opened, ImGuiWindowFlags_NoCollapse | extraFlags);
            endAction = EndAction::End;
            dock.pos = GetWindowPos();
            dock.size = GetWindowSize();

            ImGuiContext& g = *GImGui;

            if (g.ActiveId == GetCurrentWindow()->GetID("#MOVE") && g.IO.MouseDown[0])
            {
                dragOffset = GetMousePos() - dock.pos;
                doUndock(dock);
                dock.status = Status::Dragged;
            }
            return ret;
        }

        if (!dock.active && dock.status != Status::Dragged) return false;

        endAction = EndAction::EndChild;
        
        splits();

        PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        float tabbarHeight = GetTextLineHeightWithSpacing();
        if (tabbar(dock.getFirstTab(), opened != nullptr))
        {
            fillLocation(dock);
            *opened = false;
        }
        ImVec2 pos = dock.pos;
        ImVec2 size = dock.size;
        pos.y += tabbarHeight + GetStyle().WindowPadding.y;
        size.y -= tabbarHeight + GetStyle().WindowPadding.y;

        SetCursorScreenPos(pos);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                 extraFlags;
        bool ret = BeginChild(label, size, true, flags);
        PopStyleColor();
        
        return ret;
    }

    void end()
    {
        current = nullptr;
        if (endAction != EndAction::None)
        {
            if (endAction == EndAction::End)
                End();
            else if (endAction == EndAction::EndChild)
            {
                PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
                EndChild();
                PopStyleColor();
            }
        }
    }

    void debugWindow() {
        //SetNextWindowSize(ImVec2(300, 300));
        if (Begin("Dock Debug Info"))
        {
            for (int i = 0; i < docks.size(); ++i)
            {
                if (TreeNode((void*)i, "Dock %d (%p)", i, docks[i]))
                {
                    Dock &dock = *docks[i];
                    Text("pos=(%.1f %.1f) size=(%.1f %.1f)", 
                        dock.pos.x, dock.pos.y,
                        dock.size.x, dock.size.y);
                    Text("parent = %p\n",
                        dock.parent);
                    Text("isContainer() == %s\n",
                        dock.isContainer()?"true":"false");
                    Text("status = %s\n",
                        (dock.status == Status::Docked)?"Docked":
                            ((dock.status == Status::Dragged)?"Dragged": 
                                ((dock.status == Status::Float)?"Float": "?")));
                    TreePop();
                }            
            }
            
        }
        End();
    }
};

// --------------------------------------Wrap Function------------------------------------------
#include <map>
#include <string>

static std::map<std::string , DockContext> g_docklist;
static const char *curDockPanel = nullptr;

int getDockIndex(const DockContext &context , DockContext::Dock *dock)
{
    if (!dock) return -1;

    for (int i = 0; i < context.docks.size(); ++i)
    {
        if (dock == context.docks[i])
            return i;
    }

    IM_ASSERT(false);
    return -1;
}

DockContext::Dock *getDockByIndex(const DockContext& context, int idx)
{ 
    if (idx >= 0 && idx < context.docks.size())
        return context.docks[idx];

    return nullptr;
}

struct readHelper
{
    DockContext *context = nullptr;
    DockContext::Dock *dock = nullptr;
};
static readHelper rHelper;

static void *readOpen(ImGuiContext *ctx, ImGuiSettingsHandler *handler, const char *name)
{
    static std::string contextPanel;

    rHelper.context = nullptr;
    rHelper.dock    = nullptr;

    std::string tag(name);

    if (tag.substr(0, 6) == "panel:")
        contextPanel = tag.substr(6);
    else if (tag.substr(0, 5) == "Size:")
    {
        DockContext &context = g_docklist[contextPanel.c_str()];

        std::string size = tag.substr( 5 );
        int dockSize = atoi(size.c_str());

        for(int i = 0; i < dockSize; ++i)
        {
            auto *newDock = (DockContext::Dock*)MemAlloc(sizeof(DockContext::Dock));
            context.docks.push_back(IM_PLACEMENT_NEW(newDock)DockContext::Dock());
        }

        return nullptr;
    }
    else if (tag.substr(0, 5) == "Dock:")
    {
        if (g_docklist.find(contextPanel) != g_docklist.end())
        {
            DockContext &context = g_docklist[contextPanel];

            std::string indexStr = tag.substr( 5 );
            int index = atoi(indexStr.c_str());
            if(index >= 0 && index < (int)context.docks.size())
            {
                rHelper.dock = context.docks[index];
                rHelper.context = &context;
            }
        }
    }

    return (void*)&rHelper;
}

static void readLine(ImGuiContext *ctx, ImGuiSettingsHandler *handler, void *entry, const char *lineStart)
{
    auto *userdata = (readHelper*)entry;

    if (userdata)
    {
        int active, opened, status;
        int x, y, sizeX, sizeY;
        int prev, next, child0, child1, parent;
        char label[64], location[64];

        if (sscanf(lineStart, "label=%[^\n^\r]", label) == 1)
        {
            userdata->dock->label = ImStrdup(label);
            userdata->dock->id = ImHash( userdata->dock->label, 0);
        }
        else if (sscanf(lineStart, "x=%d", &x) == 1)
            userdata->dock->pos.x = (float)x;
        else if (sscanf(lineStart, "y=%d", &y) == 1)
            userdata->dock->pos.y = (float)y;
        else if (sscanf(lineStart, "size_x=%d", &sizeX) == 1)
            userdata->dock->size.x = (float)sizeX;
        else if (sscanf(lineStart, "size_y=%d", &sizeY) == 1)
            userdata->dock->size.y = (float)sizeY;
        else if (sscanf(lineStart, "active=%d", &active) == 1)
            userdata->dock->active = (bool)active;
        else if (sscanf(lineStart, "opened=%d", &opened) == 1)
            userdata->dock->opened = (bool)opened;
        else if (sscanf(lineStart, "location=%[^\n^\r]", location) == 1)
            strcpy(userdata->dock->location, location);
        else if (sscanf(lineStart, "status=%d", &status) == 1)
            userdata->dock->status = (DockContext::Status)status;
        else if (sscanf(lineStart, "prev=%d", &prev) == 1)
            userdata->dock->prevTab = getDockByIndex(*(userdata->context), prev);
        else if (sscanf(lineStart, "next=%d", &next) == 1)
            userdata->dock->nextTab = getDockByIndex(*(userdata->context), next);
        else if (sscanf(lineStart, "child0=%d", &child0) == 1)
            userdata->dock->children[0] = getDockByIndex(*(userdata->context), child0);
        else if (sscanf(lineStart, "child1=%d", &child1) == 1)
            userdata->dock->children[1] = getDockByIndex(*(userdata->context), child1);
        else if (sscanf(lineStart, "parent=%d", &parent) == 1)
            userdata->dock->parent = getDockByIndex(*(userdata->context), parent);
    }
}

static void writeAll(ImGuiContext *ctx, ImGuiSettingsHandler *handler, ImGuiTextBuffer *buf)
{
    int totalDockNum = 0;
    for (const auto &iter : g_docklist)
    {
        const DockContext &context = iter.second;
        totalDockNum += context.docks.size();
    }

    buf->reserve(buf->size() + totalDockNum * sizeof(DockContext::Dock) + 32 * (totalDockNum + (int)g_docklist.size() * 2));

    // output size
    for (const auto &iter : g_docklist)
    {
        const DockContext &context = iter.second;

        buf->appendf("[%s][panel:%s]\n", handler->TypeName, iter.first.c_str());
        buf->appendf("[%s][Size:%d]\n", handler->TypeName, (int)context.docks.size());

        for (int i = 0, dockSize = context.docks.size(); i < dockSize; ++i)
        {
            const DockContext::Dock *d = context.docks[i];

            // some docks invisible but do exist
            buf->appendf("[%s][Dock:%d]\n", handler->TypeName, i);
            buf->appendf("label=%s\n", d->label);
            buf->appendf("x=%d\n", (int)d->pos.x);
            buf->appendf("y=%d\n", (int)d->pos.y);
            buf->appendf("size_x=%d\n", (int)d->size.x);
            buf->appendf("size_y=%d\n", (int)d->size.y);
            buf->appendf("active=%d\n", (int)d->active);
            buf->appendf("opened=%d\n", (int)d->opened);
            buf->appendf("location=%s\n", d->location);
            buf->appendf("status=%d\n", (int)d->status);
            buf->appendf("prev=%d\n", (int)getDockIndex(context, d->prevTab));
            buf->appendf("next=%d\n", (int)getDockIndex(context, d->nextTab));
            buf->appendf("child0=%d\n", (int)getDockIndex(context, d->children[0]));
            buf->appendf("child1=%d\n", (int)getDockIndex(context, d->children[1]));
            buf->appendf("parent=%d\n", (int)getDockIndex(context, d->parent));
        }
    }
}

// ----------------------------------------------API-------------------------------------------------
void ImGui::ShutdownDock()
{
    for (auto &iter : g_docklist)
    {
        DockContext &context = iter.second;

        for (auto &dock : context.docks)
        {
            dock->~Dock();
            MemFree(dock);
            dock = nullptr;
        }
    }
    g_docklist.clear();
}

void ImGui::SetNextDock(const char *panel , ImGuiDockSlot slot)
{
    if (panel && g_docklist.find(panel) != g_docklist.end())
        g_docklist[panel].nextDockSlot = slot;
}

bool ImGui::BeginDockspace()
{
    ImGuiContext &g = *GImGui;
    curDockPanel = g.CurrentWindow->Name;

    IM_ASSERT(curDockPanel);

    if(!curDockPanel) return false;
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
    char childName[1024];
    sprintf(childName, "##%s", curDockPanel);
    bool result = BeginChild(childName, ImVec2(0, 0), false, flags);

    DockContext &dock = g_docklist[curDockPanel];
    dock.workspacePos = GetWindowPos();
    dock.workspaceSize = GetWindowSize();

    return result;
}

IMGUI_API void ImGui::EndDockspace()
{
    EndChild();

    curDockPanel = nullptr;
}

bool ImGui::BeginDock(const char *label, bool *opened, ImGuiWindowFlags extraFlags)
{
    IM_ASSERT(curDockPanel);

    if (!curDockPanel) return false;

    if (g_docklist.find(curDockPanel) != g_docklist.end())
    {
        DockContext &context = g_docklist[curDockPanel];

        char newLabel[128];
        sprintf(newLabel, "%s##%s", label, curDockPanel);

        return context.begin(newLabel, opened, extraFlags);
    }
    
    return false;
}

void ImGui::EndDock()
{
    IM_ASSERT(curDockPanel);

    if (!curDockPanel) return;

    if (g_docklist.find(curDockPanel) != g_docklist.end())
    {
        DockContext &context = g_docklist[curDockPanel];
        context.end();
    }
}

void ImGui::DockDebugWindow(const char *dockPanel)
{
    if (dockPanel && g_docklist.find(dockPanel) != g_docklist.end())
    {
        DockContext &context = g_docklist[dockPanel];
        context.debugWindow();
    }
}

void ImGui::InitDock()
{
    ImGuiContext &g = *GImGui;
    ImGuiSettingsHandler iniHandler;
    iniHandler.TypeName = "Dock";
    iniHandler.TypeHash = ImHash("Dock", 0, 0);
    iniHandler.ReadOpenFn = readOpen;
    iniHandler.ReadLineFn = readLine;
    iniHandler.WriteAllFn = writeAll;
    g.SettingsHandlers.push_front(iniHandler);
}
