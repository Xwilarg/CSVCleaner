#ifndef CLEANWINDOW_HPP
# define CLEANWINDOW_HPP

# include <QMainWindow>
# include <QTableWidget>
# include <QGridLayout>
# include <QPushButton>
# include <memory>

namespace CSVCleaner
{
    class MainWindow;

    class CleanWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        CleanWindow(MainWindow *window, std::unique_ptr<CleanWindow>& cleanWindowRef,
                    const QList<QList<QString>> & allColumns, bool removeDupplicate) noexcept;
        ~CleanWindow() noexcept = default;

    private:
        MainWindow &_parent;
        void closeEvent(QCloseEvent *event) noexcept override;
        QWidget _mainWidget;
        QTableWidget _mainTable;
        QGridLayout _mainLayout;
        std::unique_ptr<CleanWindow>& _cleanWindowRef;
        QPushButton _export, _apply;
        QString _name;

    private slots:
        void Apply() noexcept;
    };
}

#endif // CLEANWINDOW_HPP
