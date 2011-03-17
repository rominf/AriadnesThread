#include "dialogmapproperties.h"

DialogMapProperties::DialogMapProperties(
        QWidget *parent, Map::Properties *p): QDialog(parent)
{
    setWindowTitle(tr("Свойства здания"));

    QString mapTemplateName = "./MapTemplate.html";
    QFile mapTemplateFile(mapTemplateName);
    if (mapTemplateFile.open(QIODevice::ReadOnly))
    {
        QTextStream mapTemplateStream(&mapTemplateFile);
        m_mapTemplate = mapTemplateStream.readAll();
    }
    mapTemplateFile.close();

//    m_tempMapFile = new QTemporaryFile();

    QVBoxLayout *vlayout = new QVBoxLayout(this);
#ifdef EDITOR
    QGridLayout *glayout = new QGridLayout(this);
    vlayout->addLayout(glayout);

    QLabel *label = new QLabel(tr("Название: "));
    glayout->addWidget(label, 0, 0);
    m_ldtName = new QLineEdit(p->name);
    connect(m_ldtName, SIGNAL(editingFinished()), SLOT(mapUpdating()));
    glayout->addWidget(m_ldtName, 0, 1);

    label = new QLabel(tr("Описание: "));
    glayout->addWidget(label, 1, 0);
    m_ptdtDescription = new QPlainTextEdit(p->description);
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            SLOT(focusChanged(QWidget*,QWidget*)));
    glayout->addWidget(m_ptdtDescription, 1, 1);

    label = new QLabel(tr("Координаты (долгота, широта): "));
    glayout->addWidget(label, 2, 0);
    m_ldtCoordinates = new QLineEdit(QString::number(p->longitude) + ", " +
                                     QString::number(p->latitude));
    connect(m_ldtCoordinates, SIGNAL(textEdited(QString)), SLOT(mapUpdating()));
    glayout->addWidget(m_ldtCoordinates, 2, 1);
#endif
    m_webMap = new QWebView(this);
    QString text = "<b>" + p->name + "</b><br>" + p->description;
    QString html = m_mapTemplate.arg(text).arg(p->longitude).arg(p->latitude);
    m_webMap->setHtml(html);
    vlayout->addWidget(m_webMap);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                       QDialogButtonBox::Cancel);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    vlayout->addWidget(m_buttonBox);
}

Map::Properties* DialogMapProperties::properties() const
{
    Map::Properties *properties = new Map::Properties;
    properties->name = m_ldtName->text();
    properties->description = m_ptdtDescription->toPlainText();

    QStringList list = m_ldtCoordinates->text().split(",");
    if (list.size() != 2)
        list = m_ldtCoordinates->text().split(";");
    if (list.size() != 2)
        list = m_ldtCoordinates->text().split(" ");
    if (list.size() == 2)
    {
        properties->longitude = list.at(0).trimmed().toDouble();
        properties->latitude = list.at(1).trimmed().toDouble();
    }
    return properties;
}

void DialogMapProperties::focusChanged(QWidget *old, QWidget *now)
{
    if (qobject_cast<QPlainTextEdit*>(old) == m_ptdtDescription)
        mapUpdating();
}

void DialogMapProperties::mapUpdating()
{
    Map::Properties* p = properties();
    QString label = "<b>" + p->name + "</b><br>" + p->description;
    QString str = m_mapTemplate.arg(label).arg(p->longitude).arg(p->latitude);
    m_webMap->setHtml(str);
}
