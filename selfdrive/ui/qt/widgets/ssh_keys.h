#pragma once

#include <QPushButton>

#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"

// SSH enable toggle
class SshToggle : public ToggleControl {
  Q_OBJECT

public:
  SshToggle() : ToggleControl("Enable SSH", "", "", Hardware::get_ssh_enabled()) {
    QObject::connect(this, &SshToggle::toggleFlipped, [=](bool state) {
      Hardware::set_ssh_enabled(state);
    });
  }
};

// SSH key management widget
class SshControl : public ButtonControl {
  Q_OBJECT

public:
  SshControl();

private:
  Params params;

  QLabel username_label;

  void refresh();
  void getUserKeys(const QString &username);
};

// LateralControlSelect
class LateralControlSelect : public AbstractControl {
  Q_OBJECT

public:
  LateralControlSelect();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;

  void refresh();
};

//
class KRDateToggle : public ToggleControl {
  Q_OBJECT

public:
  KRDateToggle() : ToggleControl("주행화면 날짜 표시", "주행화면에 현재 날짜를 표시합니다.", "../assets/offroad/icon_shell.png", Params().getBool("KRDateShow")) {
    QObject::connect(this, &KRDateToggle::toggleFlipped, [=](int state) {
      char value = state ? '1' : '0';
      Params().put("KRDateShow", &value, 1);
    });
  }
};

class KRTimeToggle : public ToggleControl {
  Q_OBJECT

public:
  KRTimeToggle() : ToggleControl("주행화면 시간 표시", "주행화면에 현재 시간을 표시합니다.", "../assets/offroad/icon_shell.png", Params().getBool("KRTimeShow")) {
    QObject::connect(this, &KRTimeToggle::toggleFlipped, [=](int state) {
      char value = state ? '1' : '0';
      Params().put("KRTimeShow", &value, 1);
    });
  }
};

class GitHash : public AbstractControl {
  Q_OBJECT
    
public:
  GitHash();

private:
  QLabel local_hash;
  QLabel remote_hash;
  Params params;
};

// openpilot Preview
class OpenpilotView : public AbstractControl {
  Q_OBJECT

public:
  OpenpilotView();

private:
  QPushButton btn;
  Params params;
  
  void refresh();
};
