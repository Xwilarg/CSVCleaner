#include <QFileDialog>
#include <QMessageBox>
#include <memory>
#include "MainWindow.hpp"

namespace CSVCleaner
{
    MainWindow::MainWindow() noexcept
        : QMainWindow(nullptr), _fileMenu(menuBar()->addMenu(tr("File"))), _helpMenu(menuBar()->addMenu(tr("?"))),
          _previewBox(tr("Preview"), this),
          _previewTab(&_previewBox),
          _previewLayout(&_previewBox),
          _openAction(std::make_unique<QAction>(tr("Open"), this)), _quitAction(std::make_unique<QAction>(tr("Quit"), this)),
          _aboutQtAction(std::make_unique<QAction>(tr("About Qt"), this)),
          _csvText(&_previewTab), _csvTable(&_previewTab)
    {
        constexpr int xSize = 600, ySize = 400;
        setWindowTitle("CSVCleaner");
        resize(xSize, ySize);

        _openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
        _fileMenu->addAction(_openAction.get());
        _fileMenu->addAction(_quitAction.get());
        _helpMenu->addAction(_aboutQtAction.get());

        _previewBox.move(10, 30);
        _previewBox.resize(xSize - 20, ySize - 40);
        _previewLayout.addWidget(&_previewTab);
        _previewTab.addTab(&_csvText, tr("Raw"));
        _previewTab.addTab(&_csvTable, tr("Table"));
        _csvText.setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

        connect(_openAction.get(), SIGNAL(triggered()), this, SLOT(OpenFile()));
        connect(_quitAction.get(), SIGNAL(triggered()), this, SLOT(ExitApp()));
        connect(_aboutQtAction.get(), SIGNAL(triggered()), this, SLOT(AboutQt()));
    }

    void MainWindow::OpenFile()
    {
        QFile file(QFileDialog::getOpenFileName(this, tr("Open CSV"), ".", tr("CSV (*.csv);;Other (*)")));
        file.open(QIODevice::ReadOnly);
        _csvText.document()->setPlainText(QString(file.readAll()));
        file.close();
    }

    void MainWindow::ExitApp() const noexcept
    {
        std::exit(0);
    }

    void MainWindow::AboutQt() const noexcept
    {
        QMessageBox::aboutQt(nullptr);
    }
}
