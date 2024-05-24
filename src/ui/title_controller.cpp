#include "title_controller.hpp"

#include <unistd.h>


#define SEC_TO_MICROSEC(s) s * 1000000


TitleController::TitleController(QLabel* title_widget) :
            m_title_widget(title_widget),
            m_no_animation(true),
            m_stop(false),
            m_hold(true) {}

TitleController::~TitleController() {
    m_stop = true;
    usleep(SEC_TO_MICROSEC(0.2));
}

inline bool
needs_animation(QLabel* title_widget, std::string title) {
    return title_widget->fontMetrics().width(title.c_str()) >= title_widget->width();
}

void
TitleController::change_title(std::string new_title) {
    m_no_animation = !needs_animation(m_title_widget, new_title);

    m_hold = true;
    m_hold_time_left = SEC_TO_MICROSEC(2);
    usleep(SEC_TO_MICROSEC(0.2));

    if (m_no_animation) {
        m_title_widget->setText(new_title.c_str());
        m_title_widget->setAlignment(Qt::AlignCenter);
        return;
    }
    size_t extra_space_count =
            m_title_widget->width() / QFontMetrics(m_title_widget->font()).width(' ');
    m_title_widget->setText((new_title + std::string(extra_space_count, ' ')).c_str());
    m_title_widget->setAlignment(Qt::AlignLeft);
}

void
TitleController::run() {
    while (!m_stop) {
        if (m_no_animation) {
            usleep(SEC_TO_MICROSEC(0.2));
            continue;
        }
        if (m_hold) {
            size_t sleep_time = SEC_TO_MICROSEC(0.2);
            m_hold_time_left -= sleep_time;
            if (m_hold_time_left <= 0) {
                m_hold = false;
            } else {
                usleep(sleep_time);
                continue;
            }
        }
        // Make moving title
        usleep(SEC_TO_MICROSEC(0.2));
        QString text = m_title_widget->text();
        text.push_back(text.front());
        text.remove(0, 1);
        m_title_widget->setText(text);
    }
}
