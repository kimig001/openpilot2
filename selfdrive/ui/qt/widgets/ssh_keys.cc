#include "selfdrive/ui/qt/widgets/ssh_keys.h"

#include <QHBoxLayout>
#include <QNetworkReply>

#include <QProcess> // opkr
#include <QAction> // opkr
#include <QMenu> // opkr
#include <QDateTime> //opkr

#include "selfdrive/common/params.h"
#include "selfdrive/ui/qt/api.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/ui.h" // opkr

SshControl::SshControl() : ButtonControl("SSH Keys", "", "Warning: This grants SSH access to all public keys in your GitHub settings. Never enter a GitHub username other than your own. A comma employee will NEVER ask you to add their GitHub username.") {
  username_label.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  username_label.setStyleSheet("color: #aaaaaa");
  hlayout->insertWidget(1, &username_label);

  QObject::connect(this, &ButtonControl::clicked, [=]() {
    if (text() == "ADD") {
      QString username = InputDialog::getText("Enter your GitHub username", this);
      if (username.length() > 0) {
        setText("LOADING");
        setEnabled(false);
        getUserKeys(username);
      }
    } else {
      params.remove("GithubUsername");
      params.remove("GithubSshKeys");
      refresh();
    }
  });

  refresh();
}

void SshControl::refresh() {
  QString param = QString::fromStdString(params.get("GithubSshKeys"));
  if (param.length()) {
    username_label.setText(QString::fromStdString(params.get("GithubUsername")));
    setText("REMOVE");
  } else {
    username_label.setText("");
    setText("ADD");
  }
  setEnabled(true);
}

void SshControl::getUserKeys(const QString &username) {
  HttpRequest *request = new HttpRequest(this, false);
  QObject::connect(request, &HttpRequest::receivedResponse, [=](const QString &resp) {
    if (!resp.isEmpty()) {
      params.put("GithubUsername", username.toStdString());
      params.put("GithubSshKeys", resp.toStdString());
    } else {
      ConfirmationDialog::alert("Username '" + username + "' has no keys on GitHub", this);
    }
    refresh();
    request->deleteLater();
  });
  QObject::connect(request, &HttpRequest::failedResponse, [=] {
    ConfirmationDialog::alert("Username '" + username + "' doesn't exist on GitHub", this);
    refresh();
    request->deleteLater();
  });
  QObject::connect(request, &HttpRequest::timeoutResponse, [=] {
    ConfirmationDialog::alert("Request timed out", this);
    refresh();
    request->deleteLater();
  });

  request->sendRequest("https://github.com/" + username + ".keys");
}

// opkr
AutoScreenOff::AutoScreenOff() : AbstractControl("EON 화면끄기 시간", "주행 시작 후 화면보호를 위해 이온화면이 꺼지게 하거나 밝기를 줄입니다. 터치나 이벤트 발생시 자동으로 밝아지거나 켜집니다.", "../assets/offroad/icon_shell.png")
{

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoScreenOff"));
    int value = str.toInt();
    value = value - 1;
    if (value <= -2 ) {
      value = -2;
    }
    QUIState::ui_state.scene.scr.autoScreenOff = value;
    QString values = QString::number(value);
    params.put("AutoScreenOff", values.toStdString());
    refresh();
  });

  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("AutoScreenOff"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 10 ) {
      value = 10;
    }
    QUIState::ui_state.scene.scr.autoScreenOff = value;
    QString values = QString::number(value);
    params.put("AutoScreenOff", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoScreenOff::refresh()
{
  QString option = QString::fromStdString(params.get("AutoScreenOff"));
  if (option == "-2") {
    label.setText(QString::fromStdString("항상켜기"));
  } else if (option == "-1") {
    label.setText(QString::fromStdString("15초"));
  } else if (option == "0") {
    label.setText(QString::fromStdString("30초"));
  } else {
    label.setText(QString::fromStdString(params.get("AutoScreenOff")) + "분");
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

BrightnessControl::BrightnessControl() : AbstractControl("EON 밝기 조절(%)", "EON화면의 밝기를 수동 조절합니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("Brightness"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 0 ) {
      value = 0;
    }
    QUIState::ui_state.scene.scr.brightness = value;
    QString values = QString::number(value);
    params.put("Brightness", values.toStdString());
    refresh();
  });

  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("Brightness"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 100 ) {
      value = 100;
    }
    QUIState::ui_state.scene.scr.brightness = value;
    QString values = QString::number(value);
    params.put("Brightness", values.toStdString());
    refresh();
  });
  refresh();
}

void BrightnessControl::refresh() {
  QString option = QString::fromStdString(params.get("Brightness"));
  if (option == "0") {
    label.setText(QString::fromStdString("자동조절"));
  } else {
    label.setText(QString::fromStdString(params.get("Brightness")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

//LateralControlSelect
LateralControlSelect::LateralControlSelect() : AbstractControl("LateralControl [√]", "조향로직을 선택합니다. (PID/INDI/LQR)", "../assets/offroad/icon_logic.png") {
 
  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 45px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 45px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(120, 100);
  btnplus.setFixedSize(120, 100);

  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("LateralControlSelect"));
    int latcontrol = str.toInt();
    latcontrol = latcontrol - 1;
    if (latcontrol <= 0 ) {
      latcontrol = 0;
    }
    QString latcontrols = QString::number(latcontrol);
    Params().put("LateralControlSelect", latcontrols.toStdString());
    refresh();
  });

  QObject::connect(&btnplus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("LateralControlSelect"));
    int latcontrol = str.toInt();
    latcontrol = latcontrol + 1;
    if (latcontrol >= 2 ) {
      latcontrol = 2;
    }
    QString latcontrols = QString::number(latcontrol);
    Params().put("LateralControlSelect", latcontrols.toStdString());
    refresh();
  });
  refresh();
}

void LateralControlSelect::refresh() {
  QString latcontrol = QString::fromStdString(Params().get("LateralControlSelect"));
  if (latcontrol == "0") {
    label.setText(QString::fromStdString("PID"));
  } else if (latcontrol == "1") {
    label.setText(QString::fromStdString("INDI"));
  } else if (latcontrol == "2") {
    label.setText(QString::fromStdString("LQR"));
  }
  btnminus.setText("◀");
  btnplus.setText("▶");
}

OpenpilotView::OpenpilotView() : AbstractControl("주행화면 미리보기", "주행화면 미리보기 실행", "") {

  // setup widget
  hlayout->addStretch(1);

  btn.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btn.setFixedSize(250, 100);
  hlayout->addWidget(&btn);

  QObject::connect(&btn, &QPushButton::clicked, [=]() {
    bool stat = params.getBool("IsOpenpilotViewEnabled");
    if (stat) {
      params.putBool("IsOpenpilotViewEnabled", false);
    } else {
      params.putBool("IsOpenpilotViewEnabled", true);
    }
    refresh();
  });
  refresh();
}

void OpenpilotView::refresh() {
  bool param = params.getBool("IsOpenpilotViewEnabled");
  if (param) {
    btn.setText("View Off");
  } else {
    btn.setText("View On");
  }
}
