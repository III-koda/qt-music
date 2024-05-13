#include "download_song_dialog.hpp"
#include "ui_download_song_dialog.h"

#include <QFileDialog>

#include "../service/spotdl.hpp"
#include "../service/ytdlp.hpp"
#include "../utils/network.hpp"
#include "../utils/string.hpp"
#include "mainwindow.hpp"

#include "../extlib/logger.hpp"

#define YOUTUBE_BASE_URL "https://www.youtube.com"
#define SPOTIFY_BASE_URL "https://open.spotify.com"


static bool
url_valid(const std::string url) {
    if (!starts_with(url, YOUTUBE_BASE_URL) && !starts_with(url, SPOTIFY_BASE_URL)){
        return false;
    }
    HTTPResult res = make_http_request(HTTPMethod::HEAD, url);
    return res.successful && res.status == 200;
}

void
DownloaderThread::run() {
    bool download_res;

    switch (m_platform) {
    case MusicPlatform::SPOTIFY:
        download_res = download_spotify_song(m_url, m_download_dir);
        break;
    case MusicPlatform::YOUTUBE:
        download_res = download_youtube_audio(m_url, m_download_dir);
        break;
    }
    DownloadStatus status = download_res
            ? DownloadStatus::DONE
            : DownloadStatus::FAILED;

    std::string log_msg = "Download URL: " + m_url + (download_res ? " Success" : "Failed");
    Logger::get_instance()->log(LogLevel::INFO, log_msg);

    m_caller->download_finished_callback(m_url, status);
}

QString
SongDownloadItem::generate_text_content(std::string url, DownloadStatus status) {
    std::string status_emoji = stringify_download_status(status, true /* is emoji */);
    return QString((status_emoji + "\t" + url).c_str());
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
            "Spotify track URL (e.g. https://open.spotify.com/track/7MXVkk9YMctZ)");

    m_download_progress_list = m_ui->download_progress_list;
    std::string header_content =
            "Status\tURL";
    m_download_progress_list_header = new QListWidgetItem(tr(header_content.c_str()),
                                                          m_download_progress_list);
    m_warning_label = m_ui->warning_label;
    m_dir_label = m_ui->dir_label;

    m_dir_select_button = m_ui->dir_select_button;
    connect(m_dir_select_button,
            SIGNAL(clicked()),
            this,
            SLOT(dir_select_button_clicked()));

    m_download_song_button = m_ui->download_button;
    connect(m_download_song_button,
            SIGNAL(clicked()),
            this,
            SLOT(download_song_button_clicked()));
}

void 
DownloadSongDialog::dir_select_button_clicked() {
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Choose Download Directory"),
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (dir.isEmpty()){
            return;
        }
        m_dir_label->setText((dir.toStdString()).c_str());
        m_download_dir = dir.toStdString();
        Logger::get_instance()->log(LogLevel::INFO, "Download DIR select: " + m_download_dir);
}

void 
DownloadSongDialog::download_song_button_clicked()
{
    std::string url = m_song_url_input_box->text().toStdString();

    if (!validate_input()) {
        return;
    }

    MusicPlatform platform;
    if (starts_with(url, YOUTUBE_BASE_URL)) {
        platform = MusicPlatform::YOUTUBE;
    }
    else if (starts_with(url, SPOTIFY_BASE_URL)) {
        platform = MusicPlatform::SPOTIFY;
    }

    SongDownloadItem* song_download_item = new SongDownloadItem(m_download_progress_list,
                                                                url,
                                                                m_download_progress_list->count());
    m_download_progress_list->addItem(song_download_item);

    DownloaderThread* downloader = new DownloaderThread(url, platform, m_download_dir, this);
    downloader->start();

    Logger::get_instance()->log(LogLevel::INFO, "Download requested for URL: " + url);
}

bool
DownloadSongDialog::validate_input() {
    std::string url = m_song_url_input_box->text().toStdString();
    if (url.empty()) {
        warning_popup("Missing URL");
        Logger::get_instance()->log(LogLevel::WARNING, "NO URL");
        return false;
    }
    if (!url_valid(url)) {
        warning_popup("Invalid URL");
        Logger::get_instance()->log(LogLevel::WARNING, "invalid URL: " + url);
        return false;
    }

    if (m_download_dir.empty()){
        warning_popup("Please select a folder to download to");
        Logger::get_instance()->log(LogLevel::WARNING, "NO Download dir");
        return false;
    }
    return true;
}

void DownloadSongDialog::warning_popup(std::string error_message) {
    m_warning_label->setText(error_message.c_str());
    QTimer::singleShot(3000, [=]() {
        m_warning_label->clear();
    });
}

void
DownloadSongDialog::download_finished_callback(std::string url, DownloadStatus status) {
    // Find the corresponding SongDownloadItem instance and update status
    // Start at index 1 since header will be at 0
    for (unsigned int i = 1; i < m_download_progress_list->count(); i++) {
        auto item = static_cast<SongDownloadItem*>(m_download_progress_list->item(i));
        if (item->url() == url) {
            item->set_status(status);
            break;
        }
    }

    if (status == DownloadStatus::DONE) {
        m_main_window->get_notified_song_downloaded(m_download_dir);
    }
}
