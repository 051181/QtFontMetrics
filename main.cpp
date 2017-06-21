#include <QtGui/QGuiApplication>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QDebug>
#include <QVector>
#include <QVectorIterator>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#define MIN_FONT_SIZE 6
#define MAX_FONT_SIZE 32

QVector <QString> chafont_faces {
//    "Calibri",
//    "Trebuchet MS",
    "Arial",
    "Courier New",
    "Tahoma",
    "Helvetica",
    "Verdana",
    "Roboto",
    "Lucida Console",
    "System",
    "Microsoft Sans Serif"
};

QString output_folder = QLatin1String("/tmp/");
//QString output_folder = QLatin1String("C:\\Users\\test\\Documents\\font_metrics\\");

QString text = QLatin1String("0123456789");
static int text_length = text.size();

void saveJson(QJsonDocument document, QString fileName) {
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(document.toJson());
}

QJsonObject calculateMetric(QFont font, QString fontName) {
    QJsonObject jsonFont;

    int pWidth,
        pHeight,
        pMaxWidth,
        pAverageCharWidth,
        pFullHeight,
        widewidth,
        font_size;

    font.setFamily(fontName);
    qDebug() << "font family: " << font.family();

    for (font_size = MIN_FONT_SIZE; font_size <= MAX_FONT_SIZE; font_size++) {
//        font.setPixelSize(font_size);
        font.setPointSize(font_size);
        QFontMetrics fm(font);
        pWidth = fm.width(text, -1) / text_length; // width
        pAverageCharWidth = fm.averageCharWidth();
        if ( pAverageCharWidth > pWidth) {
            pWidth = pAverageCharWidth;
        }
        pHeight = fm.height(); // height
        pMaxWidth = fm.maxWidth(); // max width

        widewidth = ( pWidth + pMaxWidth ) / 2; // wide width

        pFullHeight = fm.leading() + pHeight; // full height

//        qDebug() << fm.boundingRect(fontName);

//        QRectF rect = fm.boundingRect(fontName);
//        int rectW = rect.width() / 10;
//        int rectH = rect.height();

        QJsonObject jsonFontEl;
        jsonFontEl.insert("w", pWidth);
        jsonFontEl.insert("h", pHeight);
//        jsonFontEl.insert("rw", rectW);
//        jsonFontEl.insert("rh", rectH);
        jsonFontEl.insert("ww", widewidth);
        jsonFontEl.insert("mw", pMaxWidth);
        jsonFontEl.insert("fh", pFullHeight);
        jsonFontEl.insert("fxd", font.fixedPitch() ? 1 : 0);
        jsonFont[QString::number(font_size)] = jsonFontEl;
    }
   return jsonFont;
}

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    QFont font;
    QVectorIterator<QString> i(chafont_faces);
    QJsonObject jsonObject;
    QString tmp_font;

    font.setBold(false);
    font.setItalic(false);

    while (i.hasNext()) {
        tmp_font = i.next();
        jsonObject[tmp_font.toLower()] = calculateMetric(font, tmp_font);
        font.setBold(true);
        jsonObject[tmp_font.toLower() + " bold"] = calculateMetric(font, tmp_font);
        font.setBold(false);
        font.setItalic(true);
        jsonObject[tmp_font.toLower() + " italic"] = calculateMetric(font, tmp_font);
        font.setItalic(false);
    }

    QJsonDocument jsonDoc(jsonObject);
//    qDebug() << jsonObject;
    saveJson(jsonDoc, output_folder + "out.json");

    return 0;
}
