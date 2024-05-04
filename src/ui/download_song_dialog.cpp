#include "download_song_dialog.hpp"
#include "ui_download_song_dialog.h"

#include "../service/spotdl.hpp"
#include "mainwindow.hpp"

#include <QLineEdit>

void
DownloaderThread::run() {
    if (download_spotify_song(m_url, m_download_dir)) {
        m_main_window->get_notified_song_downloaded(m_download_dir);
    }
}

DownloadSongDialog::DownloadSongDialog(QWidget *parent) : QDialog(parent),
                                                          m_ui(new Ui::DownloadSongDialog)
{
    m_main_window = static_cast<MainWindow*>(parent);
    m_ui->setupUi(this);
    initialize_components();
}

DownloadSongDialog::~DownloadSongDialog()
{
    delete m_ui;
}

void DownloadSongDialog::initialize_components()
{
    m_song_url_input_box = m_ui->song_url_input_box;
    m_song_url_input_box->setPlaceholderText(
            "Spotify track URL (e.g. https://open.spotify.com/track/7MXVkk9YMctZqd1Srtv4MB)");

    m_download_song_button = m_ui->download_button;
    connect(m_download_song_button,
            SIGNAL(clicked()),
            this,
            SLOT(download_song_button_clicked()));
}

void DownloadSongDialog::download_song_button_clicked()
{
    std::string url = m_song_url_input_box->text().toStdString();
    DownloaderThread* downloader = new DownloaderThread(url, m_download_dir, m_main_window);
    downloader->start();
}
