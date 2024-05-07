#ifndef EMPTY_SONG_DIR_DIALOG_HPP
#define EMPTY_SONG_DIR_DIALOG_HPP

#include <QDialog>
#include <QCommandLinkButton>

#include "download_song_dialog.hpp"

namespace Ui {
class MainWindow;
class EmptySongDirDialog;
}

class EmptySongDirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmptySongDirDialog(QWidget *parent = nullptr);
    ~EmptySongDirDialog();

private slots:
    void nav_to_download_button_clicked();

private:
    void initialize_components();

    Ui::EmptySongDirDialog *m_ui;

    MainWindow* m_main_window;
    QCommandLinkButton* m_nav_to_download_button;
    DownloadSongDialog* m_download_song_dialog;

};

#endif // EMPTY_SONG_DIR_DIALOG_HPP
