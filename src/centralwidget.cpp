#include "centralwidget.h"
#include "controlpanel.h"
#include "simulationview.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>

CentralWidget::CentralWidget(QWidget* parent)
   : QWidget{parent} {
   auto* layout = new QHBoxLayout;
   auto* view = new SimulationView(512, this);
   auto* controlPanel = new ControlPanel(this);

   layout->addWidget(view);
   layout->addWidget(controlPanel);
   setLayout(layout);

   connect(controlPanel, &ControlPanel::simulationRequested, view,
           &SimulationView::simulate);
   connect(controlPanel, &ControlPanel::abortRequested, view,
           &SimulationView::stop);
   connect(controlPanel, &ControlPanel::generateMap, view,
           &SimulationView::generateMap);
   connect(view, &SimulationView::finished, controlPanel,
           &ControlPanel::onSimulationFinished);
}
