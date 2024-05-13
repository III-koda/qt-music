#include "ui/mainwindow.hpp"
#include "service/spotdl.hpp"
#include "service/ytdlp.hpp"
#include "utils/filesys.hpp"
#include "extlib/logger.hpp"

#include <QApplication>

#define LOG_FILEPATH "./qtmusic.log"

void system_init() {
    create_directory(app_directory());
    download_spotdl();
    download_ytdlp();
    Logger::get_instance()->set_log_filepath(LOG_FILEPATH);
}


int main(int argc, char *argv[]) {
    system_init();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setFixedSize(w.size());
    return a.exec();
}
