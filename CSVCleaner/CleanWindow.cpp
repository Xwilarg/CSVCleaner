#include <QDebug>
#include "CleanWindow.hpp"
#include "MainWindow.hpp"

namespace CSVCleaner
{
    CleanWindow::CleanWindow(MainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                             const QList<QList<QString>> &allColumns, bool removeDupplicate) noexcept
        : QMainWindow(window), _parent(*window),
          _mainWidget(this), _mainTable(&_mainWidget), _mainLayout(&_mainWidget),
          _cleanWindowRef(cleanWindowRef),
          _export(tr("Export model"), &_mainWidget), _apply(tr("Apply model"), &_mainWidget),
          _name(allColumns[0][0])
    {
        setWindowTitle("CSVCleaner: " + _name);
        resize(400, 400);

        setCentralWidget(&_mainWidget);
        _mainLayout.addWidget(&_mainTable, 0, 0, 1, 0);
        _mainLayout.addWidget(&_export, 1, 0);
        _mainLayout.addWidget(&_apply, 1, 1);

        QList<QString> tokens;
        for (int i = 1; i < allColumns.size(); i++)
        {
            auto &elem = allColumns[i][0]; // TODO: Crash if all lines don't have the same nb of elements
            if (elem == nullptr)
                continue;
            if (!removeDupplicate ||
                    (removeDupplicate && !tokens.contains(elem)))
                tokens.push_back(elem);
        }
        _mainTable.setColumnCount(2);
        _mainTable.setRowCount(tokens.size());
        _mainTable.setHorizontalHeaderLabels({
            "Old value", "New value"
        });
        int i = 0;
        for (const QString &t : tokens)
        {
            QTableWidgetItem *item = new QTableWidgetItem(t);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            _mainTable.setItem(i++, 0, item);
        }

        connect(&_apply, SIGNAL(clicked()), this, SLOT(Apply()));
    }

    void CleanWindow::closeEvent(QCloseEvent *event) noexcept
    {
        QMainWindow::closeEvent(event);
        _cleanWindowRef = nullptr;
    }

    void CleanWindow::Apply() noexcept
    {
        std::map<QString, QString> allElems;
        for (int i = 0; i < _mainTable.rowCount(); i++)
            if (_mainTable.item(i, 0)->text() != ""
                && _mainTable.item(i, 1)->text() != "")
            {
                allElems.insert(std::pair<QString, QString>(
                                       _mainTable.item(i, 0)->text(),
                                       _mainTable.item(i, 1)->text()
                                       ));
            }
        _parent.ApplyTable(_name, allElems);
        close();
    }
}
