#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMouseEvent>
#include <QXmlStreamReader>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QMessageBox>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

    enum MouseType {
        None = 0,
        Top,
        Bottom,
        Left,
        Right,
        Move
    };
    QMap<QString, QString> map;
    QVector<QString> vectorplaylist;
    int indexpl = 0;
    int indexqm;
    QString filename_download;
    QMessageBox msgBox;
    bool isDownload_playlist = false;
    //Config
    //debug conf
//    QString site_url = "http://k92341zu.beget.tech/";
//    QString music_dir = "D:/playlist/";
//    QString playlistfile = "playlist.xml";
    //release
    QString site_url = "http://k92341zu.beget.tech/";
    QString music_dir = "./playlist/";
    QString playlistfile = "playlist.xml";

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    QPoint previousPosition() const;

public slots:
    void setPreviousPosition(QPoint previousPosition);
    bool xmlFile(QString playfile);
    QMap<QString, QString> addElementDataToAudioMap2(QXmlStreamReader& xml);
    void addElementDataToMap(QXmlStreamReader& xml, QMap<QString, QString>& map);
    void parsePlayList(QXmlStreamReader& xml);
    void parseTag(QXmlStreamReader& xml, QMap<QString, QString>& audio);
    void addElementToVector(QVector<QString>& vectorplaylist, QMap<QString, QString>& map);
    void sort_vector();
    void getDataDownload();
    void onResultDownload(QNetworkReply *reply);

signals:
    void previousPositionChanged(QPoint previousPosition);
    void onReadyDownload();

private slots:
    void on_btn_add_clicked();

    void on_btn_i_clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::Widget *ui;
    QStandardItemModel  *m_playListModel;
    QMediaPlayer        *m_player;
    QMediaPlaylist      *m_playlist;

    MouseType m_leftMouseButtonPressed;
    QPoint m_previousPosition;

    MouseType checkResizableField(QMouseEvent *event);
    QNetworkAccessManager *DownloadManager;
    //Downloader *downloader;
};

#endif // WIDGET_H
