#ifndef WIDGET_H
#define WIDGET_H

#include <QTime>
#include <QWidget>
#include <QTimer>
#include <QListView>
#include <QStringListModel>
#include <QSystemTrayIcon>
#include <QMouseEvent>
#include <QMenu>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void showDateTime();
    void showTimer();

    void toogleTimer();
    void pauseTimer();
    void lineTimer();

    void clearLinesTimer();
    void resetAll();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    std::unique_ptr<QTimer> timer;
    QStringList timelines;
    QStringListModel timelines_model;

    std::unique_ptr<QSystemTrayIcon> tray_icon;
    std::unique_ptr<Ui::Widget> ui;
    QMenu *tray_icon_menu;

private:
    QTime start_time;
    QTime paused_time;

    bool is_timer_working;
};
#endif // WIDGET_H
