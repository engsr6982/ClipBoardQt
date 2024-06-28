#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QMessageBox.h"
#include "nlohmann/json.hpp"
#include "qaction.h"
#include "qapplication.h"
#include "qicon.h"
#include "qpushbutton.h"
#include "qstringliteral.h"
#include "qsystemtrayicon.h"
#include <QMenu>
#include <QSettings.h>
#include <QSystemTrayIcon>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>


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

    mAppDir = toString(QApplication::applicationDirPath());

    // 初始化
    setMaximumHeight(height());
    setMinimumHeight(height());
    setMaximumWidth(width());
    setMinimumWidth(width());
    setWindowIcon(QIcon(":/logo.ico"));


    // 应用按钮 连接 保存按钮
    connect(ui->mSettingSave2, &QPushButton::clicked, this, &MainWindow::on_mSettingSave_clicked);

    updateWidgetToThis();
    loadConfig();

    // 托盘
    createSystemTray();
}


void MainWindow::closeEvent(QCloseEvent* ev) {
    if (mIsMinTray) {
        this->hide();
        ev->ignore();
        mTray->showMessage("Tip", "已最小化到托盘");
    } else ev->accept();
}

void MainWindow::createSystemTray() {
    // 托盘菜单按钮
    auto show = new QAction(QStringLiteral("主界面"));
    connect(show, &QAction::triggered, this, [this]() { this->show(); });

    auto exit = new QAction(QStringLiteral("退出"));
    connect(exit, &QAction::triggered, qApp, []() { QApplication::exit(); });

    // 托盘菜单
    QMenu* trayMenu = new QMenu(this);
    trayMenu->addAction(show);
    trayMenu->addAction(exit);

    // 托盘
    mTray = new QSystemTrayIcon(this);
    mTray->show();
    mTray->setIcon(QIcon(":/logo.ico"));
    mTray->setContextMenu(trayMenu);
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

        updateOnSystemStartedRun();
        return updateThisToWidget();
    } catch (...) {
        return false;
    }
}
bool MainWindow::saveConfig() {
    updateOnSystemStartedRun();
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
void MainWindow::updateOnSystemStartedRun() {
    QString appName = QApplication::applicationName();

    QSettings* set =
        new QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (mIsAutoStart) {
        QString dir = QApplication::applicationFilePath();
        set->setValue(appName, dir.replace("/", "\\"));
    } else {
        set->remove(appName);
    }
}

void MainWindow::on_mSettingSave_clicked() {
    updateWidgetToThis();
    saveConfig();
}

void MainWindow::on_mAddButton_clicked() {}
