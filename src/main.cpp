#include "ui/mainwindow.hpp"
#include "service/spotdl.hpp"
#include "service/ytdlp.hpp"
#include "utils/filesys.hpp"

#include <QApplication>


void system_init() {
    create_directory(app_directory());
    download_spotdl();
    download_ytdlp();
}


int main(int argc, char *argv[]) {
    system_init();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setFixedSize(w.size());
    return a.exec();
}


// #include <QApplication>
// #include <QLabel>
// #include <QTimer>
// #include <iostream>

// int main(int argc, char *argv[]) {
//     QApplication a(argc, argv);

//     // Create a QLabel widget
//     std::string s = "Moving Text12345";

//     QLabel label(s.c_str());
//     label.setAlignment(Qt::AlignCenter);


//     if (label.fontMetrics().width(label.text()) > label.width())
//     {
//         QTimer timer;
//         QObject::connect(&timer, &QTimer::timeout, [&]() {
//             // Move the text to the right
//             QString text = label.text();
//             text.push_back(text.front());
//             text.remove(0, 1);
//             label.setText(text);
//         });
//         timer.start(150);
//     }

//     label.show();

//     return a.exec();
// }
