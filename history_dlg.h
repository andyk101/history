#ifndef HISTORY_DLG_H
#define HISTORY_DLG_H

#include <QtGui>

// -----------------------------------------------------------------------------------------------------------
// HistoryDlg
// -----------------------------------------------------------------------------------------------------------
class HistoryDlg : public QDialog {
    Q_OBJECT
public:
    // static

private:
    QGridLayout* m_pLayout;
    QPushButton* m_pbtnOk;

public:
    HistoryDlg(QWidget* parent = 0);

protected:
    virtual void resizeEvent ( QResizeEvent * );

public slots:
    void ok();
};

// -----------------------------------------------------------------------------------------------------------
#endif // HISTORY_DLG_H
