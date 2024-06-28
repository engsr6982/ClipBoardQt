#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QMessageBox.h"
#include "nlohmann/json.hpp"
#include "qaction.h"
#include "qapplication.h"
#include "qicon.h"
#include "qnamespace.h"
#include "qpushbutton.h"
#include "qstringliteral.h"
#include "qsystemtrayicon.h"
#include <QInputDialog>
#include <QMenu>
#include <QSettings.h>
#include <QSystemTrayIcon>
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>


namespace fs = std::filesystem;
using json   = nlohmann::json;


// 工具函数
string x_string(QString const& qstr) {
    return string((const char*)qstr.toUtf8()); // QString转string(toStdString会崩溃)
}
string x_replace(string strSrc, string const& oldStr, string const& newStr, size_t count = -1) {
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


// 构造
MainWindow::~MainWindow() { delete ui; }
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    mAppDir = x_string(QApplication::applicationDirPath());

    // 初始化
    setMaximumHeight(height());
    setMinimumHeight(height());
    setMaximumWidth(width());
    setMinimumWidth(width());
    setWindowIcon(QIcon(":/logo.ico"));

    // 应用按钮 连接 保存按钮
    connect(ui->mSettingSave2, &QPushButton::clicked, this, &MainWindow::on_mSettingSave_clicked);

    updateWidgetToThis(); // 更新控件
    loadConfig();         // 加载配置
    createSystemTray();   // 托盘

    // 从数据库初始化
    db = std::make_unique<KeyValueDB>(mAppDir);
    db->iter([this](std::string_view, std::string_view val) {
        ui->mListWidget->addItem(QString(string(val).c_str()));
        return true;
    });
}


// 配置文件
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
    std::ofstream o(cfg);
    o << c.dump(4);
    return true;
}
bool MainWindow::updateThisToWidget() {
    ui->mSettingInputSavePath->setText(mDataSavePath.c_str());
    ui->mSettingAutoStart->setChecked(mIsAutoStart);
    ui->mSettingListenSystemClipBoard->setChecked(mIsListenClipboard);
    ui->mSettingMinTray->setChecked(mIsMinTray);
    return true;
}
bool MainWindow::updateWidgetToThis() {
    {
        auto   inputSavePath = ui->mSettingInputSavePath->text();
        string inputstr      = x_string(inputSavePath);
        // 替换路径变量 {APP}
        mDataSavePath = x_replace(inputstr, "{APP}", mAppDir.string());
    }
    mIsAutoStart       = ui->mSettingAutoStart->isChecked();
    mIsListenClipboard = ui->mSettingListenSystemClipBoard->isChecked();
    mIsMinTray         = ui->mSettingMinTray->isChecked();
    return true;
}


// 控件
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
void MainWindow::closeEvent(QCloseEvent* ev) {
    if (mIsMinTray) {
        this->hide();
        ev->ignore();
        mTray->showMessage("Tip", "已最小化到托盘");
    } else ev->accept();
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

void MainWindow::on_mAddButton_clicked() {
    bool    ok;
    QString text = QInputDialog::getMultiLineText(this, "插入", "输入要插入的内容", "", &ok);

    if (ok && !text.isEmpty()) {
        string val = x_string(text);
        if (!db->has(val)) {
            db->set(val, val);
            ui->mListWidget->addItem(text);
        } else {
            auto its = ui->mListWidget->findItems(text, Qt::MatchExactly);
            if (!its.isEmpty()) {
                int  row = ui->mListWidget->row(its.first());
                auto i   = ui->mListWidget->item(row);
                i->setSelected(true);
                ui->mListWidget->scrollToItem(i); // 移动到可见区域
            }
        }
    }
}

void MainWindow::on_mDeleteButton_clicked() {
    int row = ui->mListWidget->currentRow();
    if (row == -1) return;
    auto it = ui->mListWidget->takeItem(row);
    db->del(x_string(it->text()));
    delete it;
}
