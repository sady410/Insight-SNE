#include "menuform.h"
#include "ui_menuform.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QPainter>
#include <QStyleOption>

// Helper class to draw a colored circle
class ColorCircle : public QWidget {
public:
    ColorCircle(const QColor &color, QWidget *parent = nullptr)
        : QWidget(parent), m_color(color) {
        setFixedSize(20, 20); // Small circle size
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(m_color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect());
    }

private:
    QColor m_color;
};

MenuForm::MenuForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::MenuForm) {
    ui->setupUi(this);

    // Initialize the horizontal layout
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(10); // Adjust spacing as needed
    setLayout(verticalLayout);
}

MenuForm::~MenuForm() {
    delete ui;
}

void MenuForm::addSelectionToMenu(int id, const QColor &color) {
    // Create a container widget for the selection
    QWidget *selectionWidget = new QWidget(this);

    // Create a horizontal layout for the selection
    QHBoxLayout *selectionLayout = new QHBoxLayout(selectionWidget);
    selectionLayout->setContentsMargins(0, 0, 0, 0); // Remove margins

    // Add the colored circle
    ColorCircle *circle = new ColorCircle(color, selectionWidget);
    selectionLayout->addWidget(circle);

    // Add the label
    QLabel *label = new QLabel("Selection " + QString::number(id), selectionWidget);
    selectionLayout->addWidget(label);

    // Add the delete button (cross)
    QPushButton *deleteButton = new QPushButton("×", selectionWidget);
    deleteButton->setFixedSize(25, 25); // Larger button size
    deleteButton->setStyleSheet(
        "QPushButton { "
        "   border: none; "
        "   font-size: 30px; "
        "   color: white; "
        "   background-color: transparent; "
        "}"
        "QPushButton:hover { "
        "   color: darkred; "
        "}"
        );
    selectionLayout->addWidget(deleteButton);

    // Add the selection widget to the main layout
    verticalLayout->addWidget(selectionWidget);

    // Connect the delete button to a future slot for deletion
    connect(deleteButton, &QPushButton::clicked, this, [this, id]() {
        emit requestRemoveSelection(id);
    });
}
