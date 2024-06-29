#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "db/KeyValueDB.h"
#include "qsystemtrayicon.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <filesystem>
#include <memory>
#include <unordered_map>


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

    // 全局变量
    fs::path mAppDir;


    // 配置文件
    string mDataSavePath;      // 保存路径
    bool   mIsAutoStart;       // 开机自启
    bool   mIsListenClipboard; // 监听剪贴板
    bool   mIsMinTray;         // 最小化托盘
    bool   loadConfig();
    bool   saveConfig();
    bool   updateWidgetToThis();
    bool   updateThisToWidget();

    // 数据库
    std::unique_ptr<KeyValueDB> db; // 数据库

    // 控件
    QSystemTrayIcon* mTray;

    void createSystemTray();
    void updateOnSystemStartedRun(); // 开机自启
    void addItemToListWidget(QString const& text, bool const insertToTop = true, bool const saveToDB = true);

private slots:
    void on_mSettingSave_clicked();

    void on_mAddButton_clicked();

    void on_mDeleteButton_clicked();

    void on_mEditButton_clicked();

    void on_mCopyButton_clicked();

    void on_mPasteButton_clicked();

    void on_mOpenButton_clicked();

    void on_mFilterButton_clicked();

private:
    void closeEvent(QCloseEvent* ev) override; // 重写关闭事件

private:
    Ui::MainWindow* ui;
};
#endif // MAINWINDOW_H
