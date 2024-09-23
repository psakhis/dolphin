// Copyright 2010 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later


#pragma once

#include "InputCommon/ControllerInterface/ControllerInterface.h"

namespace ciface::MisterInput
{
void Init();
void PopulateDevices();
void DeInit();

class Device final : public Core::Device
{
public: 
  Device(uint8_t index);

  std::string GetName() const override;
  std::string GetSource() const override;
  std::optional<int> GetPreferredId() const override;
  int GetSortPriority() const override { return -2; }

  Core::DeviceRemoval UpdateInput() override;

  //void UpdateMotors();

private: 
  const uint8_t m_index;
};
}  // namespace ciface::MisterInput
