#include "ui/mainwindow.hpp"
#include "service/spotdl.hpp"

#include <QApplication>


void system_init() {
    download_spotdl();
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setFixedSize(w.size());
    system_init();
    return a.exec();
}
