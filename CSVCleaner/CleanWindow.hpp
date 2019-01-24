#ifndef CLEANWINDOW_HPP
# define CLEANWINDOW_HPP

# include <QMainWindow>
# include <QTableWidget>
# include <QVBoxLayout>
# include <memory>

namespace CSVCleaner
{
    class CleanWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        CleanWindow(QMainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                    const QString &name, QList<QString> allColumns) noexcept;
        ~CleanWindow() noexcept = default;

    private:
        void closeEvent(QCloseEvent *event) noexcept override;
        QWidget _mainWidget;
        QTableWidget _mainTable;
        QVBoxLayout _mainLayout;
        std::unique_ptr<CleanWindow>& _cleanWindowRef;
    };
}

#endif // CLEANWINDOW_HPP
