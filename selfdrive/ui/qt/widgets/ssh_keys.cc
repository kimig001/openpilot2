#include "selfdrive/ui/qt/widgets/ssh_keys.h"

#include "selfdrive/common/params.h"
#include "selfdrive/ui/qt/api.h"
#include "selfdrive/ui/qt/widgets/input.h"

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
