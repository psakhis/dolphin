// Copyright 2010 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Common/Config/Config.h"
#include "Core/Config/GraphicsSettings.h"
#include <VideoCommon/mister/groovymister_wrapper.h>

#include "InputCommon/ControllerInterface/MisterInput/MisterInput.h"

namespace ciface::MisterInput
{
struct ButtonDef
{
  const char* name;
  WORD bitmask;
};
static constexpr std::array<ButtonDef, 14> named_buttons{{
    {"Pad N", 0},
    {"Pad E", 1},
    {"Pad S", 2},
    {"Pad W", 3},
    {"Button 0", 4},
    {"Button 1", 5},
    {"Button 2", 6},
    {"Button 3", 7},
    {"Button 4", 8},
    {"Button 5", 9},
    {"Button 6", 10},
    {"Button 7", 11},
    {"Button 8", 12},
    {"Button 9", 13}
}};

static constexpr std::array named_axes{"Left X", "Left Y", "Right X", "Right Y"};

class Button final : public Core::Device::Input
{
public:
  Button(uint8_t index, uint8_t map) : m_index(index), m_map(map) {};
  std::string GetName() const override { return named_buttons[m_index].name; }
  ControlState GetState() const override
  {
     gmw_fpgaJoyInputs joyInputs;
     gmw_getJoyInputs(&joyInputs);
     int map = (m_map == 0) ? joyInputs.joy1 : joyInputs.joy2;
     switch (m_index)
     {
     case 0:
       return map & GMW_JOY_UP;
     case 1:
       return map & GMW_JOY_RIGHT;
     case 2:
       return map & GMW_JOY_DOWN;
     case 3:
       return map & GMW_JOY_LEFT;      
     case 4:
       return map & GMW_JOY_B1;
     case 5:
       return map & GMW_JOY_B2;
     case 6:
       return map & GMW_JOY_B3;
     case 7:
       return map & GMW_JOY_B4;
     case 8:
       return map & GMW_JOY_B5;
     case 9:
       return map & GMW_JOY_B6;
     case 10:
       return map & GMW_JOY_B7;
     case 11:
       return map & GMW_JOY_B8;
     case 12:
       return map & GMW_JOY_B9;
     case 13:
       return map & GMW_JOY_B10;
     default:
       return 0;
     }	            
  }

private:
  const uint8_t m_map;
  const uint8_t m_index;  
};

class Axis final : public Core::Device::Input
{
public:
  Axis(uint8_t index, const SHORT& axis, SHORT range, uint8_t map)
      : m_axis(axis), m_range(range), m_index(index), m_map(map)
  {
  }
  std::string GetName() const override
  {
    return std::string(named_axes[m_index]) + (m_range < 0 ? '-' : '+');
  }
  ControlState GetState() const override
  {
    gmw_fpgaJoyInputs joyInputs;
    gmw_getJoyInputs(&joyInputs);
    int value = 0;
    
    if (m_index == 0)    
       value = (m_map == 0) ? ((joyInputs.joy1LXAnalog << 8) + joyInputs.joy1LXAnalog) :
                              ((joyInputs.joy2LXAnalog << 8) + joyInputs.joy2LXAnalog);
     
    if (m_index == 1)
      value = (m_map == 0) ? ((joyInputs.joy1LYAnalog << 8) + joyInputs.joy1LYAnalog) :
                             ((joyInputs.joy2LYAnalog << 8) + joyInputs.joy2LYAnalog);

    if (m_index == 2)
      value = (m_map == 0) ? ((joyInputs.joy1RXAnalog << 8) + joyInputs.joy1RXAnalog) :
                             ((joyInputs.joy2RXAnalog << 8) + joyInputs.joy2RXAnalog);

    if (m_index == 3)
      value = (m_map == 0) ? ((joyInputs.joy1RYAnalog << 8) + joyInputs.joy1RYAnalog) :
                             ((joyInputs.joy2RYAnalog << 8) + joyInputs.joy2RYAnalog);

    if (value < -32768)
      value = -32768;
    if (value > 32767)
      value = 32767;

    return ControlState(value) / m_range;  
  }

private:
  const SHORT& m_axis;
  const SHORT m_range;
  const uint8_t m_index;
  const uint8_t m_map;
};


static bool s_have_guide_button = false;

void Init()
{

}

void PopulateDevices()
{
  if (Config::Get(Config::GFX_GROOVY_ENABLE))
  {
    g_controller_interface.RemoveDevice(
        [](const auto* dev) { return dev->GetSource() == "MiSTer"; });

    for (int i = 0; i != 2; ++i)
      g_controller_interface.AddDevice(std::make_shared<Device>(i));
  }
}

void DeInit()
{
  
}

Device::Device(const uint8_t index) : m_index(index)
{
  // Buttons.
  for (uint8_t i = 0; i != 14; ++i)
    AddInput(new Button(i, m_index));
    
  // Triggers.  
  // AddInput(new Trigger(u8(i), (&m_state_in.Gamepad.bLeftTrigger)[i], 255));

  // Axes.

  for (uint8_t i = 0; i != 4; ++i)
  {
    //const SHORT& ax = (&m_state_in.Gamepad.sThumbLX)[i];

    // Each axis gets a negative and a positive input instance associated with it.
    AddInput(new Axis(i, -32768, -32768, m_index));
    AddInput(new Axis(i, 32767, 32767, m_index));
  }
  
  // Rumble motors.  
  //AddOutput(new Motor(u8(i), this, (&m_state_out.wLeftMotorSpeed)[i], 65535));

  //AddInput(new Battery(&m_battery_level));
}

std::string Device::GetName() const
{
  return "Gamepad";
}

std::string Device::GetSource() const
{
  return "MiSTer";
}

Core::DeviceRemoval Device::UpdateInput()
{
  gmw_pollInputs(); 
    
  return Core::DeviceRemoval::Keep;
}

std::optional<int> Device::GetPreferredId() const
{
  return m_index;
}

}  // namespace ciface::MisterInput
