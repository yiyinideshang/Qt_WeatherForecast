#include "widget.h"     // widget.h：自定义窗口类 Widget 的声明，通常继承自 QWidget，是程序的主界面。

#include <QApplication> // Qt 应用程序类，负责管理 GUI 程序的控制流、主要设置和事件处理。

int main(int argc, char *argv[])
{
    //每个 Qt GUI 程序必须有且仅有一个 QApplication（或 QGuiApplication）实例。
    // 它负责初始化窗口系统、处理事件（鼠标点击、键盘输入等），并解析命令行参数（如 -style 等 Qt 内置选项）。
    QApplication a(argc, argv);

    //定义了一个全局标识，主要用于 QSettings 自动存储用户配置时的路径。
    a.setOrganizationName("Personal");
    a.setApplicationName("WeatherForecast");

    //在栈上创建 Widget 对象。Widget 是自定义的窗口类
    Widget w;
    //使窗口可见。如果不调用 show()，窗口不会显示（但程序仍在运行）。
    w.show();

    //启动 Qt 的事件循环，程序会一直在此处运行，等待并分发事件（如鼠标点击、定时器、网络响应等）。
    // 只有当最后一个窗口关闭（或调用 QApplication::quit()）时，exec() 才返回，main 结束，程序退出。
    // 返回值通常传给操作系统作为退出码。
    return a.exec();
}
