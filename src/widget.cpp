#include "widget.h"
#include "./ui_widget.h"

#include <QTime>
#include <QTimer>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QListView>
#include <QShortcut>
#include <QPalette>
#include <QAction>

void Widget::showDateTime()
{
    ui->dateTime_lcdNumber->display(
                QTime::currentTime().toString("hh:mm"));
}

void Widget::showTimer()
{
    qint64 elapsed = start_time.msecsTo(QTime::currentTime());
    QString text = QTime::fromMSecsSinceStartOfDay(elapsed).toString("h mm:ss");

    ui->timer_lcdNumber->display(text);
}

void Widget::toogleTimer()
{
    if (timer->isActive()) {
        timer->stop();
        start_time = QTime::currentTime();
        ui->timer_lcdNumber->display("-- --:--");
    } else {
        start_time = QTime::currentTime();
        timer->start(500);
    }
}

void Widget::pauseTimer()
{
    if (timer->isActive()) {
        paused_time = QTime::currentTime();
        timer->stop();
    } else {
        qint64 elapsed_pause = paused_time.msecsTo(QTime::currentTime());
        start_time = start_time.addMSecs(elapsed_pause);
        timer->start(500);
    }
}

void Widget::resetAll() {
    clearLinesTimer();
    toogleTimer(); // stop
    toogleTimer(); // start
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (this->isVisible()) {
        hide();
        tray_icon->show();
    }
}

void Widget::clearLinesTimer()
{
    timelines.clear();
    ui->timelines_listView->setModel(new QStringListModel(timelines, this));
}

void Widget::lineTimer()
{
    if (!timer->isActive())
        return;

    QTime end_time = QTime::currentTime();
    int elapsed = start_time.msecsTo(end_time);

    QTime time = QTime::fromMSecsSinceStartOfDay(elapsed);

    QString text = time.toString("mm:ss");
    if (start_time.msecsTo(QTime::currentTime()) >= 1000*60*60) {// if more then 1 hour
        QString hours = time.toString("h") + "h ";
        text = hours + text;
    }
    timelines.insert(0, QString("%0 (%1-%2)")
                     .arg(text)
                     .arg(start_time.toString("hh:mm"))
                     .arg(end_time.toString("hh:mm")));


    ui->timelines_listView->setModel(new QStringListModel(timelines, this));
    ui->timelines_listView->setVisible(true);

    int delta_height = 18;
    auto geometry = this->geometry();
    geometry.adjust(0, -delta_height, 0, 0);
    this->setGeometry(geometry);

    this->update();
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),
      is_timer_working(false)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow);
    tray_icon = std::make_unique<QSystemTrayIcon>(QIcon(":/res/timer_png"), this);
    connect(tray_icon.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(show()));
    tray_icon_menu = new QMenu(this);

    quitAction = std::make_unique<QAction>(tr("&Quit"), this);
    connect(quitAction.get(), SIGNAL(triggered()), qApp, SLOT(quit()));
    tray_icon_menu->addAction(quitAction.get());
    tray_icon->setContextMenu(tray_icon_menu);


    ui->timelines_listView->setVisible(false);

    // ----------------------
    const auto date_timer = new QTimer(this);
    connect(date_timer, &QTimer::timeout, this, &Widget::showDateTime);
    date_timer->start(1000);

    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, &Widget::showTimer);

    toogleTimer();
    showTimer();
    showDateTime();

    //------------------------------------
    auto shortcut = new QShortcut(QKeySequence(tr("R", "Start|Stop")), this);
    connect(shortcut, &QShortcut::activated, this, &Widget::toogleTimer);
    shortcut = new QShortcut(QKeySequence(tr("P", "Pause")), this);
    connect(shortcut, &QShortcut::activated, this, &Widget::pauseTimer);
    shortcut = new QShortcut(QKeySequence(tr("L", "Timeline")), this);
    connect(shortcut, &QShortcut::activated, this, &Widget::lineTimer);
    shortcut = new QShortcut(QKeySequence(tr("Shift+L", "Timeline clear")), this);
    connect(shortcut, &QShortcut::activated, this, &Widget::clearLinesTimer);
    shortcut = new QShortcut(QKeySequence(tr("Shift+R", "Resel all")), this);
    connect(shortcut, &QShortcut::activated, this, &Widget::resetAll);

    shortcut = new QShortcut(QKeySequence(tr("Ctrl+Q", "Start|Stop")), this);
    connect(shortcut, &QShortcut::activated, this, &Widget::close);
    //----------------------------

    // Cannot save inactive palete from UI (bug in qt?)
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Inactive, QPalette::Window, palette.brush(QPalette::Window));
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, palette.brush(QPalette::WindowText));
    palette.setBrush(QPalette::Inactive, QPalette::Base, palette.brush(QPalette::Base));
    palette.setBrush(QPalette::Inactive, QPalette::Text, palette.brush(QPalette::Text));
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, palette.brush(QPalette::ButtonText));
    this->setPalette(palette);

    //---------------------------
    this->setLayout(ui->gridLayout);

    QScreen *s = QGuiApplication::primaryScreen();
    QSize screen = s->availableSize();
    this->move(screen.width() - this->width(),
               screen.height() - this->height()-50);

    tray_icon->show();
}

Widget::~Widget()
{
}
