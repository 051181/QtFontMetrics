#include <QtGui/QGuiApplication>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QFontInfo>
#include <QDebug>
#include <QVector>
#include <QVectorIterator>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QtCore/qmath.h>
#include <QScreen>

#define MIN_FONT_SIZE 5
#define MAX_FONT_SIZE 32

QVector <QString> chafont_faces {
//    "Calibri",
//    "Trebuchet MS",
//    "System",
    "Arial",
    "Courier New",
    "Tahoma",
    "Helvetica",
    "Verdana",
    "Roboto",
    "Lucida Console",
    "Microsoft Sans Serif"
};

//QString output_folder = QLatin1String("/tmp/");
QString output_folder = QLatin1String("C:\\Users\\test\\Documents\\font_metrics\\");

//const QString text ="0123456789";
const QString text = "In questa riga viene visualizzata la stessa riga di testo in formato carattere non proporzionale . . . . . . . . . . . .";
//const QString text = "x";
static int text_length = text.size();

void saveJson(QJsonDocument document, QString fileName) {
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    jsonFile.write(document.toJson());
}

QJsonObject calculateMetric(QFont font, QString fontName, QScreen *screen) {
    QJsonObject jsonFont;

    int pWidth,
        pHeight,
        pMaxWidth,
        pAverageCharWidth,
        pFullHeight,
        wideWidth,
        font_size;

        qreal multiply_factor = 72 / screen->logicalDotsPerInchX();

//    qreal pfont_size;

    font.setFamily(fontName);
//    qDebug() << "font family: " << font.family();
//    qDebug() << QFontInfo(font).exactMatch();

    for (font_size = MIN_FONT_SIZE; font_size <= MAX_FONT_SIZE; font_size++) {
//        pfont_size = (font_size * screen->logicalDotsPerInchY()) / 72;
        font.setPointSize(font_size);
        QFontMetrics fm(font);

//        QRectF rect = fm.boundingRect(text);
//        pWidth = rect.width() / text_length; // width

        pWidth = qCeil((fm.width(text, -1) * multiply_factor)/ text_length); // width

        pAverageCharWidth = fm.averageCharWidth() * multiply_factor;
        if ( pAverageCharWidth > pWidth) {
            pWidth = pAverageCharWidth;
        }
//        pHeight = (fm.height() - 1) * multiply_factor; // height - qt aggiunge 1 x baseline, windows non lo fa
        pHeight = fm.height() * multiply_factor;

        pMaxWidth = fm.maxWidth() * multiply_factor; // max width
        if (!font.fixedPitch() && pMaxWidth > 16) {
            int tmp_wdth, wdth = 0;
            for(QString::const_iterator it = text.begin(); it != text.end(); ++it) {
                tmp_wdth = qCeil(fm.width(*it) * multiply_factor);
                if ( tmp_wdth > wdth) wdth = tmp_wdth;
            }
            if (wdth < pMaxWidth) pMaxWidth = wdth;
        }

        if (pWidth > pMaxWidth) pWidth = pMaxWidth;

        wideWidth = (pWidth + pMaxWidth) / 2; // wide width

        pFullHeight = fm.leading() * multiply_factor + pHeight; // full height

        if (wideWidth > pMaxWidth) wideWidth = pMaxWidth;
        if (wideWidth < pWidth) wideWidth = pWidth;

        QJsonObject jsonFontEl;
        jsonFontEl.insert("w", pWidth);
        jsonFontEl.insert("ww", wideWidth);
        jsonFontEl.insert("mw", pMaxWidth);
        jsonFontEl.insert("h", pHeight);
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
    QScreen *screen = QGuiApplication::primaryScreen();

    font.setBold(false);
    font.setItalic(false);

    while (i.hasNext()) {
        tmp_font = i.next();
        jsonObject[tmp_font.toLower()] = calculateMetric(font, tmp_font, screen);
        font.setBold(true);
        jsonObject[tmp_font.toLower() + " bold"] = calculateMetric(font, tmp_font, screen);
        font.setBold(false);
        font.setItalic(true);
        jsonObject[tmp_font.toLower() + " italic"] = calculateMetric(font, tmp_font, screen);
        font.setItalic(false);
    }

    QJsonDocument jsonDoc(jsonObject);
    saveJson(jsonDoc, output_folder + "out.json");

    return 0;
}
