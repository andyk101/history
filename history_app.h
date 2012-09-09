#ifndef HISTORY_APP_H
#define HISTORY_APP_H

// -----------------------------------------------------------------------------------------------------------
#include <QtGui>
#include <typeinfo>
#include <andyk/core/core_fwd.h>

#include "files.h"
//#include "deform_model.h"
//#include "deform_plot.h"
//#include "deform_table.h"

// Проводить ли тестирование
// #define TESTING

// -----------------------------------------------------------------------------------------------------------
// HistoryApp
// -----------------------------------------------------------------------------------------------------------
class HistoryApp;
template<class T> QMap<QString, QSharedPointer<T> >& history_collection(HistoryApp* /*app*/)
{
    throw ErrorBase::create(QString("Cannot find '%1' collection").arg(typeid(T).name()));
    static QMap<QString, QSharedPointer<T> > null;
    return null;
}

class HistoryApp : public QApplication, public XmlParser {
Q_OBJECT
    // helpers
private:
    template<class T> friend QMap<QString, QSharedPointer<T> >& history_collection(HistoryApp* app);
    static QDialog* findDialog(QWidget* pWidget);

    QMap<QString, QSharedPointer<Ticker> >  m_tickers;
    QMap<QString, QSharedPointer<TickersGroup> >  m_tickersGroups;
    QMap<QString, QSharedPointer<FilesGroup> >  m_filesGroups;
    QMap<QString, QSharedPointer<StockFileManager> >  m_stockFileManagers;

//    SavingReportModel m_model;
//    SavingCostPlot m_costPlot;
//    SavingPowerPlot m_powerPlot;
//    SavingReportWeb m_reportWeb;

public:
    HistoryApp(int& argc, char** argv);
    ~HistoryApp();

    template<class T> QMap<QString, QSharedPointer<T> >& collection() { return history_collection<T>(this); }
    template<class T> QSharedPointer<T> element(QString name);
    template<class T> void addCollection(QString nodeName);

    virtual void xmlParse(const XmlManager& manager);

    virtual bool notify(QObject* receiver, QEvent* event);
    virtual int exec();
};

// -----------------------------------------------------------------------------------------------------------
template<> inline QMap<QString, QSharedPointer<Ticker> >& history_collection<Ticker>(HistoryApp* app) {
    return app->m_tickers;
}

template<> inline QMap<QString, QSharedPointer<TickersGroup> >& history_collection<TickersGroup>(HistoryApp* app) {
    return app->m_tickersGroups;
}

template<> inline QMap<QString, QSharedPointer<FilesGroup> >& history_collection<FilesGroup>(HistoryApp* app) {
    return app->m_filesGroups;
}

template<> inline QMap<QString, QSharedPointer<StockFileManager> >& history_collection<StockFileManager>(HistoryApp* app) {
    return app->m_stockFileManagers;
}

// -----------------------------------------------------------------------------------------------------------
template<class T> inline QSharedPointer<T> HistoryApp::element(QString name)
{
    QMap<QString, QSharedPointer<T> >& coll = collection<T>();
    if (!coll.contains(name))
        throw ErrorBase::create(QString("Cannot find '%1' for '%2' collection")
                                .arg(name).arg(typeid(T).name()));

    return coll[name];
}

// -----------------------------------------------------------------------------------------------------------
template<class T> void HistoryApp::addCollection(QString nodeName)
{
    QMap<QString, QSharedPointer<T> >& coll = collection<T>();
    QDomElement node = xmlChildNode(xmlBody(), nodeName);
    while(!node.isNull())
    {
        QString name = xmlNodeContent(node, "Name", nodeAttr);

        bool optional = true;
        if (!coll.contains(name))
        {
            coll[name] = QSharedPointer<T>(new T());
            optional = false;
        }

        QString parent = xmlNodeContent(node, "Parent", nodeAttr, true);
        if (!parent.isEmpty())
        {
            try {
                *coll[name] = *element<T>(parent);
            } catch(QSharedPointer<ErrorBase> error) {
                throw ErrorBase::create(QString("Cannot find parent '%1' for %2 with name '%3'")
                                        .arg(parent).arg(nodeName).arg(name), error);
            }
            optional = true;
        }

        coll[name]->parse(*this, node, optional);

        node = node.nextSiblingElement(nodeName);
    }
}

// -----------------------------------------------------------------------------------------------------------
#include <QtTest>
class Test_Fixture : public QObject
{
    Q_OBJECT
public:
    Test_Fixture() {}
    virtual ~Test_Fixture() {}

private slots:
    void unit_test1()
    {
        qWarning() << "testing 1 ...";
        QVERIFY(1==1);
    }
    void unit_test2()
    {
        qWarning() << "testing 2 ...";
        QVERIFY(1==2);
    }
};

// -----------------------------------------------------------------------------------------------------------
#endif // HISTORY_APP_H
