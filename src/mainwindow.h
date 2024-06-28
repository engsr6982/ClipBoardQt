#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qsystemtrayicon.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <filesystem>


using string = std::string;
namespace fs = std::filesystem;


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    fs::path         mAppDir;
    QSystemTrayIcon* mTray;

    // 配置信息
    string mDataSavePath;              // 保存路径
    bool   mIsAutoStart       = true;  // 开机自启
    bool   mIsListenClipboard = false; // 监听剪贴板
    bool   mIsMinTray         = true;  // 最小化托盘
    bool   mIsDeheavy         = true;  // 去重


    bool loadConfig();
    bool saveConfig();
    bool updateWidgetToThis();
    bool updateThisToWidget();

    void createSystemTray();

private slots:
    void on_mSettingSave_clicked();

    void on_mAddButton_clicked();

private:
    void closeEvent(QCloseEvent* ev) override; // 重写关闭事件

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
