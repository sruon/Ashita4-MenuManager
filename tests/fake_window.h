#pragma once

#include <menumanager/menu_manager.h>
#include <menumanager/raw/structs.h>

#include <array>
#include <bit>
#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

// The address of a test object, as the game would store it
auto address(const auto& value) -> uint32_t
{
    return static_cast<uint32_t>(std::bit_cast<uintptr_t>(&value));
}

// The art file the fake window's frame and button draw from
constexpr std::string_view kArtFile = "menu    windowps";

// The draws the fake help line and title box report. Nothing is ever called there
constexpr uintptr_t kHelpDraw  = 0x500000;
constexpr uintptr_t kTitleDraw = 0x510000;

// A fake "inventor" window in process memory: a frame, a button and a cursor, with one art file the frame and button
// draw from. It is the manager's only window and has focus. A help line and a title box sit ready for a test to make
// them the window's class. Tests change fields in place and read them back through the real library
struct FakeWindow
{
    std::array<uint32_t, 2>                        shapes{ 0x120000, 0x130000 };
    menumanager::raw::YmMenuShapeFile              file{};
    menumanager::raw::FRSOURCEREC                  frameRecord{};
    menumanager::raw::BRSOURCEREC                  buttonRecord{};
    std::array<menumanager::raw::CTkResourceID, 2> resources{};
    menumanager::raw::CTkMenuFrame                 frameResource{};
    menumanager::raw::CTkMenuButton                buttonResource{};
    menumanager::raw::CTkDrawCtrlFrame             frame{};
    menumanager::raw::CTkDrawCtrlButton            button{};
    menumanager::raw::CTkDrawCtrlCursol            cursor{};
    menumanager::raw::CTkNode                      cursorNode{};
    menumanager::raw::CTkNode                      buttonNode{};
    menumanager::raw::CTkNode                      frameNode{};
    menumanager::raw::CTkMenuData                  menuData{};
    menumanager::raw::CTkMenuCtrlData              window{};
    menumanager::raw::CTkNode                      windowNode{};
    menumanager::raw::CTkMenuMng                   manager{};
    std::array<uint32_t, 5>                        helpVtable{};
    std::array<uint32_t, 5>                        titleVtable{};
    menumanager::raw::CTkHelp                      help{};
    menumanager::raw::CTkTitle                     title{};
    std::unique_ptr<menumanager::IMenuManager>     menus;

    FakeWindow();
    FakeWindow(const FakeWindow&) = delete; // the nodes point at each other's addresses

    // the window's draw objects, and whether the walk was complete
    auto objects(bool shapes = true) -> std::pair<std::vector<menumanager::FFXI::menuobject_t>, bool>;
};
