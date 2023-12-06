#include "centralwidget.h"
#include "simulationview.h"

#include <QHBoxLayout>
#include <QPushButton>

CentralWidget::CentralWidget(QWidget* parent)
   : QWidget{parent} {
   auto* layout = new QHBoxLayout;
   auto* view = new SimulationView(512, this);
   auto* button = new QPushButton("Start", this);

   layout->addWidget(view);
   layout->addWidget(button);
   setLayout(layout);

   connect(button, &QPushButton::clicked, view, &SimulationView::simulate);
}
