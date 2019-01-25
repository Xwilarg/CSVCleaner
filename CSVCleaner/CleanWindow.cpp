#include <QDebug>
#include "CleanWindow.hpp"

namespace CSVCleaner
{
    CleanWindow::CleanWindow(QMainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                             const QList<QList<QString>> &allColumns, bool removeDupplicate) noexcept
        : QMainWindow(window),
          _mainWidget(this), _mainTable(&_mainWidget), _mainLayout(&_mainWidget),
          _cleanWindowRef(cleanWindowRef)
    {
        setWindowTitle("CSVCleaner: " + allColumns[0][0]);
        resize(400, 400);

        setCentralWidget(&_mainWidget);
        _mainLayout.addWidget(&_mainTable);
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
            _mainTable.setItem(i++, 0, new QTableWidgetItem(t));
    }

    void CleanWindow::closeEvent(QCloseEvent *event) noexcept
    {
        QMainWindow::closeEvent(event);
        _cleanWindowRef = nullptr;
    }
}
