#include "fake_window.h"

#include <menumanager/slider_driver.h>

#include <doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <ostream>
#include <utility>
#include <vector>

using namespace menumanager;

TEST_CASE("driving a slider to a value")
{
    // a native slider: Ok starts and stops adjusting, and while adjusting the raise key steps up and the other down.
    // Ranges of 100 and 127 steps, the raise key either way round, from both sides of the target
    for (const int range : { 100, 127 })
    {
        for (const auto raise : { FFXI::MenuKey::Right, FFXI::MenuKey::Left })
        {
            for (const auto [from, target] : { std::pair{ 0, 0.73f }, std::pair{ 100, 0.25f }, std::pair{ 40, 0.40f } })
            {
                int          value     = from * range / 100;
                bool         adjusting = false;
                bool         aimed     = false;
                int          frames    = 0;
                SliderDriver driver;
                driver.retarget(4, target);
                while (driver.button() != 0 && frames++ < 400)
                {
                    const auto step = driver.step(static_cast<float>(value) / range);
                    aimed           = aimed || step.moveCursor;
                    for (const auto key : step.keys)
                    {
                        if (key == FFXI::MenuKey::Ok)
                        {
                            adjusting = !adjusting;
                        }
                        else if (adjusting)
                        {
                            value = key == raise ? std::min(range, value + 1) : std::max(0, value - 1);
                        }
                    }
                }

                const float error = std::abs(static_cast<float>(value) / range - target);
                CHECK_MESSAGE((aimed && !adjusting && driver.button() == 0 && error <= 1.0f / range && frames <= 60), range << " steps " << from << " to " << target << ": at " << value << " after " << frames << " frames");
            }
        }
    }
}

TEST_CASE("moving to another slider while adjusting one")
{
    SliderDriver driver;
    driver.retarget(3, 0.5f);
    CHECK(driver.step(0.2f).moveCursor);
    driver.retarget(4, 0.7f);
    const auto leave = driver.step(0.2f);
    CHECK((!leave.moveCursor && leave.keys == std::vector{ FFXI::MenuKey::Ok } && leave.button == 4));
    const auto enter = driver.step(0.2f);
    CHECK((enter.moveCursor && enter.keys == std::vector{ FFXI::MenuKey::Ok }));
}

TEST_CASE("a slider that stops showing or never moves")
{
    SliderDriver driver;
    driver.retarget(3, 0.5f);
    driver.step(0.2f);
    CHECK((driver.step(-1).keys.empty() && driver.button() == 0));

    driver.retarget(3, 0.9f);
    SliderStep step;
    for (int frame = 0; frame < 10 && driver.button() != 0; ++frame)
    {
        step = driver.step(0.1f);
    }

    CHECK((driver.button() == 0 && step.keys == std::vector{ FFXI::MenuKey::Ok } && step.button == 3));
}

TEST_CASE("a step for a window without focus sends nothing")
{
    FakeWindow   w;
    SliderDriver driver;
    driver.retarget(1, 0.5f);
    const auto step     = driver.step(0.2f);
    w.manager.m_pCurMCD = 0; // keyDown and setCursol are null, so any input would crash
    CHECK_FALSE(step.apply(*w.menus, address(w.window)));
    CHECK(SliderStep{}.apply(*w.menus, address(w.window)));
}
