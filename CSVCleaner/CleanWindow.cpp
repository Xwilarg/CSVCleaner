#include <QDebug>
#include <QMessageBox>
#include "CleanWindow.hpp"
#include "MainWindow.hpp"

namespace CSVCleaner
{
    CleanWindow::CleanWindow(MainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                             const QList<QList<QString>> &allColumns, bool removeDupplicate) noexcept
        : QMainWindow(window), _parent(*window),
          _mainWidget(this), _mainTab(&_mainWidget), _mainLayout(&_mainWidget),
          _cleanWindowRef(cleanWindowRef),
          _export(tr("Export model"), &_mainWidget), _apply(tr("Apply model"), &_mainWidget),
          _tables(), _allNames()
    {
        resize(400, 400);

        setCentralWidget(&_mainWidget);
        _mainLayout.addWidget(&_mainTab, 0, 0, 1, 0);
        _mainLayout.addWidget(&_export, 1, 0);
        _mainLayout.addWidget(&_apply, 1, 1);

        int refSize = allColumns[0].size();
        for (int y = 0; y < allColumns[0].size(); y++)
        {
            QList<QString> tokens;
            for (int i = 1; i < allColumns.size(); i++)
            {
                if (allColumns[i].size() != refSize)
                {
                    QMessageBox::critical(window, tr("Error"), tr("Inconsistant line size.\nMake sure your CSV is well-formatted."));
                    close();
                    return;
                }
                auto &elem = allColumns[i][y]; // TODO: Crash if all lines don't have the same nb of elements
                if (elem == nullptr)
                    continue;
                if (!removeDupplicate ||
                        (removeDupplicate && !tokens.contains(elem)))
                    tokens.push_back(elem);
            }
            QTableWidget *table = new QTableWidget(this);
            table->setColumnCount(2);
            table->setRowCount(tokens.size());
            table->setHorizontalHeaderLabels({
                "Old value", "New value"
            });
            int i = 0;
            for (const QString &t : tokens)
            {
                QTableWidgetItem *item = new QTableWidgetItem(t);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                table->setItem(i++, 0, item);
            }
            _mainTab.addTab(table, allColumns[0][y]);
            _allNames.push_back(allColumns[0][y]);
            _tables.push_back(std::move(table));
        }

        connect(&_apply, SIGNAL(clicked()), this, SLOT(Apply()));
    }

    void CleanWindow::closeEvent(QCloseEvent *event) noexcept
    {
        QMainWindow::closeEvent(event);
        _cleanWindowRef = nullptr;
    }

    bool CleanWindow::IsQTableWidgetItemNullOrEmpty(QTableWidgetItem *item) const noexcept
    {
        return (item == nullptr || item->text() == "");
    }

    void CleanWindow::Apply() noexcept
    {
        QList<std::map<QString, QString>> allElems;
        for (QTableWidget *w : _tables)
        {
            std::map<QString, QString> tmp;
            for (int i = 0; i < w->rowCount(); i++)
                if (!IsQTableWidgetItemNullOrEmpty(w->item(i, 0))
                    && !IsQTableWidgetItemNullOrEmpty(w->item(i, 1)))
                {
                    tmp.insert(std::pair<QString, QString>(
                                           w->item(i, 0)->text(),
                                           w->item(i, 1)->text()
                                           ));
                }
            allElems.push_back(std::move(tmp));
        }
        _parent.ApplyTable(std::move(_allNames), std::move(allElems));
        close();
    }
}
