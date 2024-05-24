#ifndef TITLE_CONTROLLER_HPP
#define TITLE_CONTROLLER_HPP

#include <QLabel>
#include <QThread>
#include <QTimer>

#include <string>

class TitleController : public QThread {
    Q_OBJECT

public:
    TitleController(QLabel* title_widget);

    ~TitleController();

    void change_title(std::string new_title);

private:
    void run();

    QLabel* m_title_widget;
    bool m_stop;
    bool m_no_animation;
    bool m_hold;
    long m_hold_time_left;
};

#endif // TITLE_CONTROLLER_HPP
