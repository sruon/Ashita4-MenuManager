#include "fake_window.h"

#include <algorithm>
#include <string_view>
#include <utility>

using namespace menumanager;
using namespace menumanager::raw;

FakeWindow::FakeWindow()
: menus(attachMenuManager(address(manager), 0, 0, kHelpDraw, kTitleDraw))
{
    file = { .shapes = address(shapes), .shape_num = 2 };
    std::ranges::copy(kArtFile, file.id);
    frameRecord.resourceCount  = 1;
    buttonRecord.resourceCount = 2;
    resources                  = { CTkResourceID{ .m_Shapes = address(file) }, CTkResourceID{ .m_kind = 4, .m_RscNum = 1, .m_Shapes = address(file) } };
    for (auto& resource : resources)
    {
        std::ranges::copy(kArtFile, resource.m_RscID);
    }

    frameResource  = { .m_pRR = address(frameRecord), .m_pResources = address(resources) };
    buttonResource = { .m_pRR = address(buttonRecord), .m_pResources = address(resources) };

    frame      = { .m_RecKind = std::to_underlying(FFXI::MenuObjectType::Frame), .m_pShape = shapes[0], .m_NeckTime = 11, .m_pMF = address(frameResource), .m_fadeMode = std::to_underlying(FFXI::MenuFade::In), .m_fadeTime = 27 };
    button     = { .m_RecKind = std::to_underlying(FFXI::MenuObjectType::Button), .m_pShape = shapes[1], .m_pMB = address(buttonResource), .m_MenuCtrlID = 5, .m_LocX = 12, .m_LocY = 34, .m_Width = 56, .m_Height = 78, .m_ButtonNo = 7 };
    cursor     = { .m_RecKind = std::to_underlying(FFXI::MenuObjectType::Cursor), .m_LocX = 9, .m_LocY = 37 };
    cursorNode = { .m_pOb = address(cursor) };
    buttonNode = { .m_pNextNode = address(cursorNode), .m_pOb = address(button) };
    frameNode  = { .m_pNextNode = address(buttonNode), .m_pOb = address(frame) };

    std::ranges::copy(std::string_view("menu    inventor"), menuData.m_ID);
    window     = { .m_pMD = address(menuData), .m_MenuCtrlID = std::to_underlying(FFXI::MenuStatus::Closing), .m_DrawShapes = { .m_pTopNode = address(frameNode) } };
    windowNode = { .m_pOb = address(window) };
    manager    = { .m_DrawDatas = { .m_pTopNode = address(windowNode) }, .m_pCurMCD = address(window) };

    // OnDrawPrimitive is the vtable's fifth entry
    helpVtable[4]  = kHelpDraw;
    titleVtable[4] = kTitleDraw;
    help.vtable    = address(helpVtable);
    title.vtable   = address(titleVtable);
}

auto FakeWindow::objects(const bool shapes) -> std::pair<std::vector<FFXI::menuobject_t>, bool>
{
    std::array<FFXI::menuobject_t, 256> buffer{};
    bool                                complete = false;
    const auto                          count    = menus->GetWindowObjects(address(window), buffer.data(), buffer.size(), shapes, &complete);
    return { { buffer.begin(), buffer.begin() + count }, complete };
}
