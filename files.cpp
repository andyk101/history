#include "files.h"
#include "history_app.h"

// -----------------------------------------------------------------------------------------------------------
// Ticker
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<Ticker> Ticker::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<Ticker>(name);
}

void Ticker::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);
    parser.xmlNodeVarChange(m_section, element, "Section", XmlParser::nodeAttr, optional);
    parser.xmlNodeVarChange(m_code, element, "Code", XmlParser::nodeAttr, optional);
}

// -----------------------------------------------------------------------------------------------------------
// TickersGroup
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<TickersGroup> TickersGroup::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<TickersGroup>(name);
}

void TickersGroup::addString(QDomElement& elem, int)
{
    QSharedPointer<Ticker> item(Ticker::find(elem.text()));
    if (!m_tikers.contains(item))
        m_tikers << item;
}

void TickersGroup::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    m_tikers.clear();
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);
    parser.xmlParseCollection(this, &TickersGroup::addString, 0, element, "Ticker", optional);
}

// -----------------------------------------------------------------------------------------------------------
// FilesGroup
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<FilesGroup> FilesGroup::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<FilesGroup>(name);
}

void FilesGroup::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);

    QString groupName = parser.xmlNodeVar<QString>(element, "TickersGroup",  XmlParser::nodeChild, optional);
    if (!groupName.isEmpty())
        m_tikersGroup = TickersGroup::find(groupName);

    m_timeFrames.clear();
    parser.xmlParseCollection(this, &FilesGroup::addTimeFrame, 0, element, "TimeFrame", optional);
}

// -----------------------------------------------------------------------------------------------------------
// FileWatcher
// -----------------------------------------------------------------------------------------------------------
FileWatcher::FileWatcher()
{
    connect(&m_watcher, SIGNAL(directoryChanged(QString)), SLOT(directoryChanged(QString)));
}

void FileWatcher::init(QString path, QString fileTemplate, QString dateRegEx)
{
    m_path = QDir::toNativeSeparators(qApp->applicationDirPath() + QDir::separator() + path);
    m_extractor.setDateRegEx(dateRegEx);
    m_extractor.setPattern(fileTemplate);
}

void FileWatcher::reset()
{
    m_watcher.removePaths(m_watcher.directories());
    m_pendingFiles.clear();
    m_parsedFiles.clear();
    m_errorFiles.clear();
}

void FileWatcher::watch()
{
    m_watcher.addPath(m_path);
    directoryChanged(m_path);
}

void FileWatcher::invokeFiles()
{
    while(!m_pendingFiles.empty())
    {
        QString file = m_pendingFiles.first();
        m_pendingFiles.pop_front();

        QVector<QString> params;
        if (!m_extractor.extract(file, params) || !parseFile(file, params))
            m_errorFiles << file;
        else
            m_parsedFiles << file;
    }
}

void FileWatcher::directoryChanged(QString path)
{
    QDir dir(path);
    QStringList files = dir.entryList(QDir::Files, QDir::Name|QDir::Reversed);
    bool wake = false;
    foreach(QString file, files)
    {
        if (m_parsedFiles.contains(file) || m_errorFiles.contains(file))
            continue;

        if (!m_pendingFiles.contains(file))
        {
            if (m_pendingFiles.empty())
                wake = true;

            m_pendingFiles << file;
        }
    }
    if (wake)
        invokeFiles();
}

// -----------------------------------------------------------------------------------------------------------
// StockFileManager
// -----------------------------------------------------------------------------------------------------------
QSharedPointer<StockFileManager> StockFileManager::find(QString name)
{
    return qobject_cast<HistoryApp*>(qApp)->element<StockFileManager>(name);
}

void StockFileManager::addFilesGroup(QDomElement& elem, int)
{
    QSharedPointer<FilesGroup> item = FilesGroup::find(elem.text());
    if (!m_filesGroups.contains(item))
        m_filesGroups << item;
}

void StockFileManager::parse(XmlParser& parser, QDomElement& element, bool optional)
{
    m_filesGroups.clear();
    parser.xmlNodeVarChange(m_name, element, "Name", XmlParser::nodeAttr, optional);
    parser.xmlNodeVarChange(m_filePath, element, "FilePath", XmlParser::nodeChild, optional);
    parser.xmlNodeVarChange(m_dateFormat, element, "DateFormat", XmlParser::nodeChild, optional);
    parser.xmlNodeVarChange(m_dateRegEx, element, "DateRegEx", XmlParser::nodeChild, optional);
    parser.xmlNodeVarChange(m_fileName, element, "FileName", XmlParser::nodeChild, optional);
    parser.xmlParseCollection(this, &StockFileManager::addFilesGroup, 0, element, "FilesGroup", optional);
    parser.xmlNodeVarChange(m_size, element, "Size", XmlParser::nodeChild, optional);
    FileWatcher::init(m_filePath, m_fileName, m_dateRegEx);
}

bool StockFileManager::parseFile(QString fileName, QVector<QString> paramsName)
{
    QString tickerName = paramsName[SymbolExtractor::eTicker];
    QString timeFrameName = paramsName[SymbolExtractor::eTimeFrame];
    QString startDate = paramsName[SymbolExtractor::eStart];
    QString endDate = paramsName[SymbolExtractor::eEnd];
    enum { eTicker, eTimeFrame, eCount };
    Cartesian cartesian(eCount);

    foreach(QSharedPointer<FilesGroup> fileGroup, m_filesGroups)
    {
        QList< QSharedPointer<Ticker> > tickers = fileGroup->tikersGroup()->tikers();
        QList<TimeFrame> timeFrames = fileGroup->timeFrames();

        cartesian.setLimit(eTicker, tickers.count());
        cartesian.setLimit(eTimeFrame, timeFrames.count());
        for (cartesian.first(); cartesian.isValid(); cartesian.next())
        {
            QSharedPointer<Ticker> ticker = tickers[cartesian[eTicker]];
            TimeFrame timeFrame = timeFrames[cartesian[eTimeFrame]];

            if (ticker->m_name == tickerName && timeFrame.name() == timeFrameName)
            {
                QDate start = QDate::fromString(startDate, m_dateFormat);
                QDate end = QDate::fromString(endDate, m_dateFormat);

                if (start.isValid() && end.isValid() && parseFile(fileName, ticker, timeFrame,
                    fileGroup, start, end))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool StockFileManager::parseFile(QString fileName, QSharedPointer<Ticker> ticker, TimeFrame timeFrame,
    QSharedPointer<FilesGroup> filesGroup, QDate start, QDate end)
{
    return true;
}


// -----------------------------------------------------------------------------------------------------------
