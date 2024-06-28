#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "nlohmann/json.hpp"
#include "qapplication.h"
#include "qpushbutton.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using json   = nlohmann::json;


string toString(QString const& qstr) {
    return string((const char*)qstr.toLocal8Bit()); // QString转string(toStdString会崩溃)
}


MainWindow::~MainWindow() { delete ui; }
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 限制缩放
    setMaximumHeight(height());
    setMinimumHeight(height());
    setMaximumWidth(width());
    setMinimumWidth(width());

    mAppDir = toString(QApplication::applicationDirPath());

    // 应用按钮 连接 保存按钮
    connect(ui->mSettingSave2, &QPushButton::clicked, this, &MainWindow::on_mSettingSave_clicked);

    updateWidgetToThis();
    loadConfig();
}


bool MainWindow::updateWidgetToThis() {
    QString inputSavePath = ui->mSettingInputSavePath->text();
    if (!inputSavePath.isEmpty()) mDataSavePath = toString(inputSavePath);
    mIsAutoStart       = ui->mSettingAutoStart->isChecked();
    mIsListenClipboard = ui->mSettingListenSystemClipBoard->isChecked();
    mIsMinTray         = ui->mSettingMinTray->isChecked();
    mIsDeheavy         = ui->mSettingDeheavy->isChecked();
    return true;
}
bool MainWindow::loadConfig() {
    fs::path cfg = mAppDir / "config.json";

    if (!fs::exists(cfg)) saveConfig();

    json cache;
    try {
        std::ifstream i(cfg);
        i >> cache;

        mDataSavePath      = cache["save_path"].get<std::string>();
        mIsAutoStart       = cache["auto_start"].get<bool>();
        mIsListenClipboard = cache["listen_system_clipboard"].get<bool>();
        mIsMinTray         = cache["min_tray"].get<bool>();
        mIsDeheavy         = cache["deheavy"].get<bool>();

        return updateThisToWidget();
    } catch (...) {
        return false;
    }
}
bool MainWindow::saveConfig() {
    fs::path cfg = mAppDir / "config.json";

    json c;
    c["save_path"]               = mDataSavePath;
    c["auto_start"]              = mIsAutoStart;
    c["listen_system_clipboard"] = mIsListenClipboard;
    c["min_tray"]                = mIsMinTray;
    c["deheavy"]                 = ui->mSettingDeheavy->isChecked();
    std::ofstream o(cfg);
    o << c.dump(4);
    return true;
}
bool MainWindow::updateThisToWidget() {
    ui->mSettingInputSavePath->setText(mDataSavePath.c_str());
    ui->mSettingAutoStart->setChecked(mIsAutoStart);
    ui->mSettingListenSystemClipBoard->setChecked(mIsListenClipboard);
    ui->mSettingMinTray->setChecked(mIsMinTray);
    ui->mSettingDeheavy->setChecked(mIsDeheavy);
    return true;
}

void MainWindow::on_mSettingSave_clicked() {
    updateWidgetToThis();
    saveConfig();
}
