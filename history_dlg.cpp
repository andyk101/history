#include "history_dlg.h"

// -----------------------------------------------------------------------------------------------------------
// HistoryDlg
// -----------------------------------------------------------------------------------------------------------
HistoryDlg::HistoryDlg(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint |
                   Qt::WindowCloseButtonHint);
    setWindowTitle("History state");

    QLabel* plblLabel = new QLabel("Ok:", this);
    m_pbtnOk = new QPushButton("Ok", this);
    m_pLayout = new QGridLayout(this);
    plblLabel->setBuddy(m_pbtnOk);

    connect(m_pbtnOk, SIGNAL(clicked()), SLOT(ok()));

    m_pLayout->addWidget(plblLabel, 0,0, Qt::AlignRight);
    m_pLayout->addWidget(m_pbtnOk,  0,1);

    m_pLayout->setColumnStretch(0,0);
    m_pLayout->setColumnStretch(1,1);
}

// -----------------------------------------------------------------------------------------------------------
void HistoryDlg::resizeEvent ( QResizeEvent * event )
{
    QDialog::resizeEvent(event);

    // ...
}

// -----------------------------------------------------------------------------------------------------------
void HistoryDlg::ok()
{
}

// -----------------------------------------------------------------------------------------------------------
