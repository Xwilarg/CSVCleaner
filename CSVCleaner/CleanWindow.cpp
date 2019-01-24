#include "CleanWindow.hpp"

namespace CSVCleaner
{
    CleanWindow::CleanWindow(QMainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                             const QString &name, QList<QString> allColumns) noexcept
        : QMainWindow(window),
          _mainWidget(this), _mainTable(&_mainWidget), _mainLayout(&_mainWidget),
          _cleanWindowRef(cleanWindowRef)
    {
        setWindowTitle("CSVCleaner: " + name);
        resize(400, 400);

        setCentralWidget(&_mainWidget);
        _mainLayout.addWidget(&_mainTable);

        _mainTable.setColumnCount(2);
        _mainTable.setHorizontalHeaderLabels({
            "Old value", "New value"
        });
    }

    void CleanWindow::closeEvent(QCloseEvent *event) noexcept
    {
        QMainWindow::closeEvent(event);
        _cleanWindowRef = nullptr;
    }
}
