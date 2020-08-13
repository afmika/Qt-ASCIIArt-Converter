#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void setCharSet(QString value);
    void loadImage(QString image_path, qreal scale_factor = 1.0);
    void displayImage(QImage* image);
    void logs(QString text, bool isError = false);
    void defineOutputTextFont();
public slots:
    void onConvert();
    void onFileClick();
    void onSliderMoved(int index);
    void onSliderReleased();
    void onCharSetChange(int index);
    void onTextSliderZoom();
private:
    QImage* image = nullptr;
    QUrl current_image_path;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
