#include "centralwidget.h"
#include "simulationview.h"

#include <QHBoxLayout>

CentralWidget::CentralWidget(QWidget* parent)
   : QWidget{parent} {
   auto* layout = new QHBoxLayout;
   auto* view = new SimulationView(250, this);

   layout->addWidget(view);
   setLayout(layout);
}
