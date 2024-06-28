#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "nlohmann/json.hpp"
#include "qapplication.h"
#include "qpushbutton.h"
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using json   = nlohmann::json;


string toString(QString const& qstr) {
    return string((const char*)qstr.toLocal8Bit()); // QString转string(toStdString会崩溃)
}

string replace(string strSrc, string const& oldStr, string const& newStr, size_t count = -1) {
    string strRet  = strSrc;
    size_t pos     = 0;
    int    l_count = 0;
    if (-1 == count) count = strRet.size();
    while ((pos = strRet.find(oldStr, pos)) != string::npos) {
        strRet.replace(pos, oldStr.size(), newStr);
        if (++l_count >= count) break;
        pos += newStr.size();
    }
    return strRet;
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
    {
        auto   inputSavePath = ui->mSettingInputSavePath->text();
        string inputstr      = toString(inputSavePath);
        // 替换路径变量 {APP}
        mDataSavePath = replace(inputstr, "{APP}", mAppDir.string());
    }
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
