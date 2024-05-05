#include "download_song_dialog.hpp"
#include "ui_download_song_dialog.h"

#include "../service/spotdl.hpp"
#include "mainwindow.hpp"

#include <QLineEdit>

#include <QDebug>

void
DownloaderThread::run() {
    DownloadStatus status = (download_spotify_song(m_url, m_download_dir))
        ? DownloadStatus::DONE
        : DownloadStatus::FAILED;
    m_caller->download_finished_callback(m_url, status);
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
    delete m_download_progress_list_header;
    delete m_ui;
}

void DownloadSongDialog::initialize_components()
{
    m_song_url_input_box = m_ui->song_url_input_box;
    m_song_url_input_box->setPlaceholderText(
            "Spotify track URL (e.g. https://open.spotify.com/track/7MXVkk9YMctZqd1Srtv4MB)");

    m_download_progress_list = m_ui->download_progress_list;
    m_download_progress_list_header = new QListWidgetItem(tr("Status\t\tURLs"),
                                                          m_download_progress_list);

    m_download_song_button = m_ui->download_button;
    connect(m_download_song_button,
            SIGNAL(clicked()),
            this,
            SLOT(download_song_button_clicked()));
}

void DownloadSongDialog::download_song_button_clicked()
{
    std::string url = m_song_url_input_box->text().toStdString();

    SongDownloadItem song_download_item(m_download_progress_list, 
                                        url, 
                                        m_download_progress_list->count());

    // m_download_progress_list->addItem(song_download_item);

    DownloaderThread* downloader = new DownloaderThread(url, m_download_dir, this);
    downloader->start();
}

void DownloadSongDialog:: sample_qlistwidget() {
    // //Initial headers
    // std::string done = "DONE";
    // std::string downloading = "Downloading";
    // std::string urls = "https://open.spotify.com/track/7MXVkk9YMctZqd1Srtv4MB";
    // std::string itemText = downloading + "\t" + urls;
    // m_download_progress_list->addItem(QListWidgetItem(""));
    // m_download_progress_list->addItem(itemText.c_str());
    // m_download_progress_list->addItem(itemText.c_str());
    
    // qDebug() << QString((m_download_progress_list->count()));
}

void
DownloadSongDialog::download_finished_callback(std::string url, DownloadStatus status) {
    if (status == DownloadStatus::DONE) {
        m_main_window->get_notified_song_downloaded(m_download_dir);
    } else if (status == DownloadStatus::FAILED) {
    }
}
