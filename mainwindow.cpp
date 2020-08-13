#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QStringListModel>
#include <QFileDialog>
#include <QErrorMessage>
#include <QMenu>

#include "ASCIIConverter.h"


static std::map<QString, QString> charset_map {
    {"STANDARD", ASCIIConverter::CHARS::STANDARD},
    {"GRAY", ASCIIConverter::CHARS::GRAY},
    {"GRAY MIN", ASCIIConverter::CHARS::STD_MIN},
    {"GRAY LV 0", ASCIIConverter::CHARS::GRAY_0},
    {"GRAY LV 1", ASCIIConverter::CHARS::GRAY_1},
    {"GRAY LV 2", ASCIIConverter::CHARS::GRAY_2},
    {"CONTRAST", ASCIIConverter::CHARS::DENSE},
    {"MIXED", ASCIIConverter::CHARS::MIXED}
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // file
    QMenu* fileMenu = ui->menubar->addMenu(tr("&File"));
    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setStatusTip(tr("Open an existing file"));
    fileMenu->addAction(openAct);
    
    // character set
    QStringList char_list;
    for (auto value : charset_map) {
        char_list << value.first;
    }
    QStringListModel* char_model = new QStringListModel;
    char_model->setStringList(char_list);
    ui->charset_combo->setModel(char_model);    
    
    // events    
    
    connect(openAct, &QAction::triggered, this, &MainWindow::onFileClick);
    connect(ui->convert_btn, SIGNAL(clicked()), SLOT(onConvert()));
    connect(ui->img_scale_slider, SIGNAL(sliderMoved(int)), SLOT(onSliderMoved(int)));
    connect(ui->img_scale_slider, SIGNAL(sliderReleased()), SLOT(onSliderReleased()));    
    // connect(ui->text_zoom, SIGNAL(sliderMoved(int)), SLOT(onTextSliderZoom(int)));        
    connect(ui->text_zoom, SIGNAL(sliderReleased()), SLOT(onTextSliderZoom()));        
    connect(ui->charset_combo, SIGNAL(currentIndexChanged(int)), SLOT(onCharSetChange(int)));
    
    // init
    setCharSet( charset_map[ui->charset_combo->itemText(0)] );
    logs("");
    ui->convert_progress->hide();
    
    // window properties
    // 0 ... 200% (0 ~ 2)
    ui->text_zoom->setRange(0, 150);
    ui->img_scale_slider->setRange(0, 200);
    setFixedSize(size()); // fixed size
    setWindowTitle(tr("afImage2ASCII Converter v1.0 - afmika"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayImage(QImage* image) {
    QSize size = image->size();
    int w = size.width(), 
        h = size.height();
    std::string text = "Size ";
    text += std::to_string(w) + " x " + std::to_string(h);
    logs(QString::fromStdString(text));
    
    // display
    QGraphicsView* view = ui->graphicsView;    
    QGraphicsScene* scene = new QGraphicsScene;
    // scene->addRect(QRectF(-10, -10, 20, 20));
    QPixmap pixmap = QPixmap::fromImage(*image);
    scene->addPixmap(pixmap);
    view->setScene(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->show();  
}

void MainWindow::logs(QString text, bool isError) {
    if ( isError ) {
        ui->logs_text->setText("");
        QErrorMessage error_diag(this);
        error_diag.setToolTip(text);
        error_diag.showMessage(text);
        // blocks input to other visible windows in the same application
        error_diag.setModal(true); 
        error_diag.exec();
    } else {
        ui->logs_text->setText(text);
    }    
    qDebug() << text;    
}


void MainWindow::setCharSet(QString value) {
    ui->current_charset_text->setText(value);
}

void MainWindow::loadImage(QString image_path, qreal scale_factor) {
    
    if ( image ) {
        current_image_path = "";
        delete image;
        image = nullptr;
    }
    
    ui->text_zoom->setValue(150 / 2);
    if ( !image_path.trimmed().isEmpty() ) {
        current_image_path = *(new QUrl(image_path));
        image = new QImage(current_image_path.toString());
        QSize new_size = scale_factor * image->size();
        image = new QImage(image->scaled(new_size));
        displayImage(image);   
    } else {
        logs("No image selected!", true);
    }
}

void MainWindow::defineOutputTextFont() {
    int px = std::max(ui->text_zoom->value() / 10, 1);    
    QFont font;
    font.setFamily("Consolas");
    font.setPixelSize(px);
    ui->text_output->setFont(font);
}

void MainWindow::onConvert() {
    if ( image ) {
        QUrl dest_folder = current_image_path.adjusted(QUrl::RemoveFilename);
        QString filename = current_image_path.fileName();
        QString destpath = QString("%1/ASCII_%2.txt").arg(dest_folder.toString(), filename);
        QString mode = ui->current_charset_text->text();         
        ASCIIConverter::convert(image, ui->convert_progress, destpath, mode);
        
        QFile file(destpath);
        if ( file.open(QFile::ReadOnly) ) {
            defineOutputTextFont();
            QString content = QString::fromStdString(file.readAll().toStdString());
            ui->text_output->setText(content);
            file.close();
        }
        
        logs(QString("Converted! %1").arg(filename));        
    } else {
        logs("No image selected!", true);        
    }
}

void MainWindow::onFileClick() {
    QString filetype = tr("Image Files (*.png *.jpg *.bmp)");
    QString label = tr("Open Image");
    QString fileName = QFileDialog::getOpenFileName(this, label, "", filetype);
    if ( !fileName.trimmed().isEmpty() ) {
        ui->img_scale_slider->setValue(50);
        qreal scale_factor = qreal(ui->img_scale_slider->value()) / 100;    
        loadImage(fileName, scale_factor);   
    }
}

void MainWindow::onSliderMoved(int index) {
    // blabla
}

void MainWindow::onSliderReleased() {
    qreal scale_factor = qreal(ui->img_scale_slider->value()) / 100;
    loadImage(current_image_path.toString(), scale_factor);        
}

void MainWindow::onCharSetChange(int index) {
    QString selected_text = ui->charset_combo->itemText(index);
    setCharSet(charset_map[selected_text]);
}

void MainWindow::onTextSliderZoom() {
    // max 15px , min 1 px     
    defineOutputTextFont();
}
