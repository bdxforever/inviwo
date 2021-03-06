/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2017 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <warn/push>
#include <warn/ignore/all>
#include <QMenu>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QKeyEvent>
#include <QLabel>
#include <QThread>
#include <QCoreApplication>
#include <QFontDatabase>
#include <QHeaderView>
#include <QStandardItem>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QToolButton>
#include <QPixmap>
#include <QSettings>
#include <QScrollBar>
#include <QResizeEvent>
#include <QWheelEvent>
#include <warn/pop>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/qt/editor/consolewidget.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/qt/editor/inviwomainwindow.h>
#include <inviwo/core/util/ostreamjoiner.h>

namespace inviwo {

TextSelectionDelegate::TextSelectionDelegate(QWidget* parent) : QItemDelegate(parent) {}

QWidget* TextSelectionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                             const QModelIndex& index) const {
    if (index.column() == static_cast<int>(LogTableModelEntry::ColumnID::Message)) {
        auto value = index.model()->data(index, Qt::EditRole).toString();
        auto widget = new QLineEdit(parent);
        widget->setReadOnly(true);
        widget->setText(value);
        return widget;
    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void TextSelectionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                         const QModelIndex& index) const {
    IVW_UNUSED_PARAM(editor);
    IVW_UNUSED_PARAM(model);
    IVW_UNUSED_PARAM(index);
    // dummy function to prevent changing the model
}

ConsoleWidget::ConsoleWidget(InviwoMainWindow* parent)
    : InviwoDockWidget(tr("Console"), parent)
    , tableView_(new QTableView(this))
    , model_()
    , filter_(new QSortFilterProxyModel(this))
    , levelFilter_(new QSortFilterProxyModel(this))
    , textSelectionDelegate_(new TextSelectionDelegate(this))
    , filterPattern_(new QLineEdit(this))
    , mainwindow_(parent) {

    setObjectName("ConsoleWidget");
    setAllowedAreas(Qt::BottomDockWidgetArea);

    qRegisterMetaType<LogTableModelEntry>("LogTableModelEntry");

    filter_->setSourceModel(model_.model());
    filter_->setFilterKeyColumn(static_cast<int>(LogTableModelEntry::ColumnID::Message));

    levelFilter_->setSourceModel(filter_);
    levelFilter_->setFilterKeyColumn(static_cast<int>(LogTableModelEntry::ColumnID::Level));

    filterPattern_->setClearButtonEnabled(true);

    tableView_->setModel(levelFilter_);
    tableView_->setGridStyle(Qt::NoPen);
    tableView_->setCornerButtonEnabled(false);

    tableView_->setContextMenuPolicy(Qt::ActionsContextMenu);
    clearAction_ = new QAction(QIcon(":/icons/clear-consolelog.png"), tr("&Clear Log"), this);
    clearAction_->setShortcut(Qt::ControlModifier + Qt::Key_E);
    connect(clearAction_, &QAction::triggered, [&]() { clear(); });

    tableView_->hideColumn(static_cast<int>(LogTableModelEntry::ColumnID::Date));
    tableView_->hideColumn(static_cast<int>(LogTableModelEntry::ColumnID::Level));
    tableView_->hideColumn(static_cast<int>(LogTableModelEntry::ColumnID::Audience));
    tableView_->hideColumn(static_cast<int>(LogTableModelEntry::ColumnID::Path));
    tableView_->hideColumn(static_cast<int>(LogTableModelEntry::ColumnID::File));
    tableView_->hideColumn(static_cast<int>(LogTableModelEntry::ColumnID::Line));
    tableView_->hideColumn(static_cast<int>(LogTableModelEntry::ColumnID::Function));

    tableView_->horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    tableView_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    const auto cols = tableView_->horizontalHeader()->count();

    auto viewColGroup = new QMenu(this);
    for (int i = 0; i < cols; ++i) {
        auto viewCol =
            new QAction(model_.getName(static_cast<LogTableModelEntry::ColumnID>(i)), this);
        viewCol->setCheckable(true);
        viewCol->setChecked(!tableView_->isColumnHidden(i));
        connect(viewCol, &QAction::triggered, [this, i](bool state) {
            if (!state) {
                tableView_->hideColumn(i);
            } else {
                tableView_->showColumn(i);
            }
        });
        tableView_->horizontalHeader()->addAction(viewCol);
        viewColGroup->addAction(viewCol);
    }
    auto visibleColumnsAction = new QAction("Visible Columns", this);
    visibleColumnsAction->setMenu(viewColGroup);

    tableView_->horizontalHeader()->setResizeContentsPrecision(0);
    tableView_->horizontalHeader()->setSectionResizeMode(cols - 1, QHeaderView::Stretch);
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    tableView_->verticalHeader()->setVisible(false);
    tableView_->verticalHeader()->setResizeContentsPrecision(0);
    tableView_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    const auto height = QFontMetrics(QFontDatabase::systemFont(QFontDatabase::FixedFont)).height();
    const int margin = 2;
    tableView_->verticalHeader()->setMinimumSectionSize(height + margin);
    tableView_->verticalHeader()->setDefaultSectionSize(height + margin);

    QHBoxLayout* statusBar = new QHBoxLayout();
    statusBar->setObjectName("StatusBar");

    auto makeIcon = [](const QString& file, bool checkable = false) {
        auto icon = QIcon();
        if (checkable) {
            icon.addPixmap(QPixmap(":/icons/" + file + ".png"), QIcon::Normal, QIcon::On);
            icon.addPixmap(QPixmap(":/icons/" + file + "-bw.png"), QIcon::Normal, QIcon::Off);
        } else {
            icon.addPixmap(QPixmap(":/icons/" + file + ".png"), QIcon::Normal, QIcon::Off);
            icon.addPixmap(QPixmap(":/icons/" + file + "-bw.png"), QIcon::Disabled, QIcon::Off);
        }
        return icon;
    };

    auto makeToolButton = [this, statusBar, makeIcon](const QString& label, const QString& file,
                                                      bool checkable = true) {
        auto button = new QToolButton(this);
        auto action = new QAction(makeIcon(file, checkable), label, this);
        action->setCheckable(checkable);
        if (checkable) action->setChecked(true);

        button->setDefaultAction(action);
        statusBar->addWidget(button);
        return action;
    };

    auto updateRowsHeights = [this, height, margin]() {
        tableView_->setUpdatesEnabled(false);

        auto vrows = tableView_->verticalHeader()->count();
        for (int i = 0; i < vrows; ++i) {
            auto mind = mapToSource(i, static_cast<int>(LogTableModelEntry::ColumnID::Message));
            const auto message = mind.data(Qt::DisplayRole).toString();
            const auto lines = std::count(message.begin(), message.end(), '\n') + 1;
            tableView_->verticalHeader()->resizeSection(i,
                                                        margin + static_cast<int>(lines) * height);
        }
        tableView_->setUpdatesEnabled(true);
    };

    auto levelCallback = [this, updateRowsHeights](bool /*checked*/) {
        if (util::all_of(levels, [](const auto& level) { return level.action->isChecked(); })) {
            levelFilter_->setFilterRegExp("");
        } else {
            std::stringstream ss;
            auto joiner = util::make_ostream_joiner(ss, "|");
            joiner = "None";
            for (const auto& level : levels) {
                if (level.action->isChecked()) joiner = level.level;
            }
            levelFilter_->setFilterRegExp(QString::fromStdString(ss.str()));
        }
        updateRowsHeights();
    };

    auto levelGroup = new QMenu(this);
    for (auto& level : levels) {
        level.action =
            makeToolButton(QString::fromStdString(level.name), QString::fromStdString(level.icon));
        level.label = new QLabel("0", this);
        statusBar->addWidget(level.label);
        statusBar->addSpacing(5);
        levelGroup->addAction(level.action);
        connect(level.action, &QAction::toggled, levelCallback);
    }
    auto viewAction = new QAction("Log Level", this);
    viewAction->setMenu(levelGroup);

    auto clearButton = new QToolButton(this);
    clearButton->setDefaultAction(clearAction_);
    statusBar->addWidget(clearButton);
    statusBar->addSpacing(5);

    statusBar->addStretch(3);
    statusBar->addWidget(new QLabel("Filter", this));

    filterPattern_->setMinimumWidth(200);
    statusBar->addWidget(filterPattern_, 1);
    statusBar->addSpacing(5);

    auto clearFilter = new QAction(makeIcon("clear-filter"), "C&lear Filter", this);
    clearFilter->setEnabled(false);

    connect(filterPattern_, &QLineEdit::textChanged,
            [this, updateRowsHeights, clearFilter](const QString& text) {
                filter_->setFilterRegExp(text);
                updateRowsHeights();
                clearFilter->setEnabled(!text.isEmpty());
            });

    connect(clearFilter, &QAction::triggered, [this]() { filterPattern_->setText(""); });

    auto filterAction = new QAction(makeIcon("filter"), "&Filter", this);
    filterAction->setShortcut(Qt::ControlModifier + Qt::AltModifier + Qt::Key_F);
    connect(filterAction, &QAction::triggered, [this]() {
        raise();
        filterPattern_->setFocus();
        filterPattern_->selectAll();
    });

    // add actions for context menu
    auto createSeparator = [this]() {
        auto separator = new QAction(this);
        separator->setSeparator(true);
        return separator;
    };

    // add separator at the beginning as the "copy" action will be inserted later at the front
    tableView_->addAction(createSeparator());
    tableView_->addAction(visibleColumnsAction);
    tableView_->addAction(viewAction);
    tableView_->addAction(createSeparator());
    tableView_->addAction(clearAction_);
    tableView_->addAction(createSeparator());
    tableView_->addAction(filterAction);
    tableView_->addAction(clearFilter);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(tableView_);
    layout->addLayout(statusBar);

    layout->setContentsMargins(3, 0, 0, 3);

    QWidget* w = new QWidget();
    w->setLayout(layout);
    setWidget(w);
    tableView_->installEventFilter(this);
    tableView_->setAttribute(Qt::WA_Hover);
    tableView_->setItemDelegateForColumn(static_cast<int>(LogTableModelEntry::ColumnID::Message),
                                         textSelectionDelegate_);

    connect(this, &ConsoleWidget::logSignal, this, &ConsoleWidget::logEntry);
    connect(this, &ConsoleWidget::clearSignal, this, &ConsoleWidget::clear);

    // Restore State
    QSettings settings("Inviwo", "Inviwo");
    settings.beginGroup("console");

    {
        auto columnsActive = settings.value("columnsActive", QVariant(QList<QVariant>()));
        auto columnsWidth = settings.value("columnsWidth", QVariant(QList<QVariant>()));

        auto active = columnsActive.toList();
        auto widths = columnsWidth.toList();
        auto count = std::min(active.size(), widths.size());

        for (int i = 0; i < count; ++i) {
            auto hidden = active[i].toBool();
            tableView_->horizontalHeader()->setSectionHidden(i, hidden);
            if (!hidden) tableView_->horizontalHeader()->resizeSection(i, widths[i].toInt());
        }
    }

    {
        auto levelsActive = settings.value("levelsActive", QVariant(QList<QVariant>()));
        int i = 0;
        for (const auto& level : levelsActive.toList()) {
            levels[i++].action->setChecked(level.toBool());
        }
    }

    auto filterText = settings.value("filterText", "");
    filterPattern_->setText(filterText.toString());

    settings.endGroup();
}

ConsoleWidget::~ConsoleWidget() = default;

QAction* ConsoleWidget::getClearAction() { return clearAction_; }

void ConsoleWidget::clear() {
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        emit clearSignal();
        return;
    }

    model_.clear();
    for (auto& level : levels) {
        level.label->setText("0");
        level.count = 0;
    }
}

void ConsoleWidget::updateIndicators(LogLevel level) {
    auto it = util::find_if(levels, [&](const auto& l) { return l.level == level; });
    if (it != levels.end()) {
        it->label->setText(toString(++(it->count)).c_str());
    }
}

void ConsoleWidget::log(std::string source, LogLevel level, LogAudience audience, const char* file,
                        const char* function, int line, std::string msg) {
    LogTableModelEntry e = {
        std::chrono::system_clock::now(), source, level, audience, file ? file : "", line,
        function ? function : "",         msg};
    logEntry(std::move(e));
}

void ConsoleWidget::logProcessor(Processor* processor, LogLevel level, LogAudience audience,
                                 std::string msg, const char* file, const char* function,
                                 int line) {
    LogTableModelEntry e = {std::chrono::system_clock::now(),
                            processor->getIdentifier(),
                            level,
                            audience,
                            file ? file : "",
                            line,
                            function ? function : "",
                            msg};
    logEntry(std::move(e));
}

void ConsoleWidget::logNetwork(LogLevel level, LogAudience audience, std::string msg,
                               const char* file, const char* function, int line) {
    LogTableModelEntry e = {std::chrono::system_clock::now(),
                            "ProcessorNetwork",
                            level,
                            audience,
                            file ? file : "",
                            line,
                            function ? function : "",
                            msg};
    logEntry(std::move(e));
}

void ConsoleWidget::logEntry(LogTableModelEntry e) {
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        emit logSignal(e);
        return;
    }

    auto lines = std::count(e.message.begin(), e.message.end(), '\n') + 1;
    auto height = QFontMetrics(QFontDatabase::systemFont(QFontDatabase::FixedFont)).height();

    tableView_->setUpdatesEnabled(false);

    model_.log(e);
    updateIndicators(e.level);

    // Faster but messes with filters.
    if (lines != 1) {
        auto vind = mapFromSource(model_.model()->rowCount() - 1, 0);
        if (vind.isValid()) {
            tableView_->verticalHeader()->resizeSection(vind.row(),
                                                        2 + static_cast<int>(lines) * height);
        }
    }

    tableView_->scrollToBottom();
    tableView_->setUpdatesEnabled(true);
}

void ConsoleWidget::keyPressEvent(QKeyEvent* keyEvent) {
    if (keyEvent->key() == Qt::Key_E && keyEvent->modifiers() == Qt::ControlModifier) {
        clear();
    }
}

bool ConsoleWidget::eventFilter(QObject* /*object*/, QEvent* event) {
    if (event->type() == QEvent::FocusIn) {
        focus_ = true;

        auto enable = tableView_->selectionModel()->hasSelection();
        auto action = mainwindow_->getActions().find("Copy")->second;

        if (!connections_["Copy"]) {
            connections_["Copy"] = connect(action, &QAction::triggered, [&]() {
                const auto& inds = tableView_->selectionModel()->selectedIndexes();
                int prevrow = inds.first().row();
                bool first = true;
                QString text;
                for (const auto& ind : inds) {
                    if (!tableView_->isColumnHidden(ind.column())) {
                        if (!first && ind.row() == prevrow) {
                            text.append('\t');
                        } else if (!first) {
                            text.append('\n');
                        }
                        text.append(ind.data(Qt::DisplayRole).toString());
                        first = false;
                    }
                    prevrow = ind.row();
                }
                auto mimedata = util::make_unique<QMimeData>();
                mimedata->setData(QString("text/plain"), text.toUtf8());
                QApplication::clipboard()->setMimeData(mimedata.release());
            });
        }
        action->setEnabled(enable);

    } else if (event->type() == QEvent::FocusOut) {
        focus_ = false;
        auto action = mainwindow_->getActions().find("Copy")->second;
        disconnect(connections_["Copy"]);
        action->setEnabled(focus_ || hover_);
    } else if (event->type() == QEvent::HoverEnter) {
        hover_ = true;
        auto enable = tableView_->selectionModel()->hasSelection();
        auto action = mainwindow_->getActions().find("Copy")->second;
        action->setEnabled(enable);
        mainwindow_->getActions().find("Paste")->second->setEnabled(false);
        mainwindow_->getActions().find("Cut")->second->setEnabled(false);
        mainwindow_->getActions().find("Delete")->second->setEnabled(false);

    } else if (event->type() == QEvent::HoverLeave) {
        hover_ = false;
        auto action = mainwindow_->getActions().find("Copy")->second;
        action->setEnabled(focus_ || hover_);
    }
    return false;
}

QModelIndex ConsoleWidget::mapToSource(int row, int col) {
    auto ind = levelFilter_->index(row, col);
    auto lind = levelFilter_->mapToSource(ind);
    return filter_->mapToSource(lind);
}

QModelIndex ConsoleWidget::mapFromSource(int row, int col) {
    auto mind = model_.model()->index(row, col);
    auto lind = filter_->mapFromSource(mind);
    return levelFilter_->mapFromSource(lind);
}

void ConsoleWidget::closeEvent(QCloseEvent* event) {
    QSettings settings("Inviwo", "Inviwo");
    settings.beginGroup("console");
    settings.setValue("geometry", saveGeometry());

    const auto cols = tableView_->horizontalHeader()->count();
    QList<QVariant> columnsActive;
    QList<QVariant> columnsWidth;
    columnsActive.reserve(cols);
    columnsWidth.reserve(cols);
    for (int i = 0; i < cols; ++i) {
        columnsActive.append(tableView_->horizontalHeader()->isSectionHidden(i));
        columnsWidth.append(tableView_->horizontalHeader()->sectionSize(i));
    }
    QList<QVariant> levelsActive;
    for (const auto& level : levels) {
        levelsActive.append(level.action->isChecked());
    }

    settings.setValue("columnsActive", QVariant(columnsActive));
    settings.setValue("columnsWidth", QVariant(columnsWidth));
    settings.setValue("levelsActive", QVariant(levelsActive));
    settings.setValue("filterText", QVariant(filterPattern_->text()));
    settings.endGroup();

    InviwoDockWidget::closeEvent(event);
}

LogTableModel::LogTableModel() : model_(0, static_cast<int>(LogTableModelEntry::size())) {
    for (size_t i = 0; i < LogTableModelEntry::size(); ++i) {
        auto item = new QStandardItem(getName(static_cast<LogTableModelEntry::ColumnID>(i)));
        item->setTextAlignment(Qt::AlignLeft);
        model_.setHorizontalHeaderItem(static_cast<int>(i), item);
    }
}

void LogTableModel::log(LogTableModelEntry entry) {
    QList<QStandardItem*> items;
    items.reserve(static_cast<int>(LogTableModelEntry::size()));
    for (size_t i = 0; i < LogTableModelEntry::size(); ++i) {
        items.append(entry.get(static_cast<LogTableModelEntry::ColumnID>(i)));
        items.last()->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        items.last()->setTextAlignment(Qt::AlignLeft);
        items.last()->setEditable(false);
        // items.last()->setSizeHint(QSize(1, lines * height));

        switch (entry.level) {
            case LogLevel::Info:
                items.last()->setForeground(QBrush(infoTextColor_));
                break;
            case LogLevel::Warn:
                items.last()->setForeground(QBrush(warnTextColor_));
                break;
            case LogLevel::Error:
                items.last()->setForeground(QBrush(errorTextColor_));
                break;
            default:
                items.last()->setForeground(QBrush(infoTextColor_));
                break;
        }
    }

    model_.appendRow(items);
}

LogModel* LogTableModel::model() { return &model_; }

void LogTableModel::clear() { model_.removeRows(0, model_.rowCount()); }

QString LogTableModel::getName(LogTableModelEntry::ColumnID ind) const {
    switch (ind) {
        case LogTableModelEntry::ColumnID::Date:
            return QString("Date");
        case LogTableModelEntry::ColumnID::Time:
            return QString("Time");
        case LogTableModelEntry::ColumnID::Source:
            return QString("Source");
        case LogTableModelEntry::ColumnID::Level:
            return QString("Level");
        case LogTableModelEntry::ColumnID::Audience:
            return QString("Audience");
        case LogTableModelEntry::ColumnID::Path:
            return QString("Path");
        case LogTableModelEntry::ColumnID::File:
            return QString("File");
        case LogTableModelEntry::ColumnID::Line:
            return QString("Line");
        case LogTableModelEntry::ColumnID::Function:
            return QString("Function");
        case LogTableModelEntry::ColumnID::Message:
            return QString("Message");
        default:
            return QString();
    }
}

std::string LogTableModelEntry::getDate() const {
    auto in_time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%F");
    return ss.str();
}
std::string LogTableModelEntry::getTime() const {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()) % 1000;

    auto in_time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%T");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

QStandardItem* LogTableModelEntry::get(ColumnID ind) const {
    switch (ind) {
        case ColumnID::Date:
            return new QStandardItem(utilqt::toQString(getDate()));
        case ColumnID::Time:
            return new QStandardItem(utilqt::toQString(getTime()));
        case ColumnID::Source:
            return new QStandardItem(utilqt::toQString(source));
        case ColumnID::Level:
            return new QStandardItem(utilqt::toQString(toString(level)));
        case ColumnID::Audience:
            return new QStandardItem(utilqt::toQString(toString(audience)));
        case ColumnID::Path:
            return new QStandardItem(utilqt::toQString(filesystem::getFileDirectory(fileName)));
        case ColumnID::File:
            return new QStandardItem(
                utilqt::toQString(filesystem::getFileNameWithExtension(fileName)));
        case ColumnID::Line:
            return new QStandardItem(utilqt::toQString(toString(lineNumber)));
        case ColumnID::Function:
            return new QStandardItem(utilqt::toQString(funcionName));
        case ColumnID::Message:
            return new QStandardItem(utilqt::toQString(message));
        default:
            return new QStandardItem();
    }
}

LogModel::LogModel(int rows, int columns, QObject* parent)
    : QStandardItemModel(rows, columns, parent) {}

Qt::ItemFlags LogModel::flags(const QModelIndex& index) const {
    auto flags = QStandardItemModel::flags(index);
    // make only the message column editable
    const auto col = static_cast<LogTableModelEntry::ColumnID>(index.column());
    if (col == LogTableModelEntry::ColumnID::Message) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

}  // namespace
