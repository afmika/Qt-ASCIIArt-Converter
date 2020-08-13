#ifndef ASCIICONVERTER_H
#define ASCIICONVERTER_H

#include <QString>
#include <QColor>
#include <QFile>
#include <QImage>
#include <QDebug>
#include <QProgressBar>

namespace ASCIIConverter {
    namespace CHARS {
        static QString GRAY_0 = "$$PPXX00wwooIIccvv::++oooooooooooooooooooooo";
        static QString GRAY_1 = "XX00wwooIIccvv::++!!~~::::::::::::::::::::::";
        static QString GRAY_2 = "wooIIccvv::++!!~~""..,......................";
        static QString GRAY = "@@@@@@@@@@@@%%%%%%%%#########********+++++++++====";
        static QString STANDARD = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^'. ";
        static QString STD_MIN = "@%#*+=-:. "; 
        static QString DENSE = "█▀▄░ ";
        static QString MIXED = "XX░░00wwooIIccvv:::~cv::++!!~~::::.. ";
    };
    
    QChar convert(const QColor& pixel, const QString mode = CHARS::STD_MIN ) {
        int nbcolor = (pixel.red() + pixel.green() + pixel.blue()) / 3;
        qreal size = mode.size();        
        qreal idx = (size - 1) * (qreal(nbcolor) / qreal(255));
        return mode.at( static_cast<int>(idx) );
    }
    
    void convert(const QImage* image, QProgressBar* progress, const QString& dest_path, const QString mode = CHARS::STD_MIN ) {
        QFile file(dest_path);
        bool can_open = false;
        
        if ( file.exists() ) {
            if ( file.open(QFile::WriteOnly | QFile::Text) ) {
                // clear the previous content
                file.write("");
                file.close();                
            }   
        }
        
        if ( file.open(QFile::WriteOnly | QFile::Text | file.Append) ) {
            progress->show();
            can_open = true;
            int value = 0;
            int total = image->height() * image->width();
            int mod = 1 + total / 100;
            progress->setRange(0, total);
            for (int y = 0; y < image->height(); y++) {
                QString temp;
                for (int x = 0; x < image->width(); x++) {
                    QChar res = convert(image->pixelColor(x, y), mode);   
                    temp += res;
                    value++;
                    if ( value % mod == 0 ) {
                        progress->setValue(value);                        
                    }
                }
                temp += "\n";
                file.write(temp.toStdString().c_str());                
            }
            file.close();   
            progress->hide();
        }
        
        if (!can_open) {
            qDebug() << QString("Can not open file %1").arg(dest_path);
        }
    }
};

#endif // ASCIICONVERTER_H
