#include "controlpanel.h"

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

ControlPanel::ControlPanel(QWidget* parent)
   : QWidget{parent} {
   setupWidget();
}

void ControlPanel::onSimulationFinished() {
   emit abortRequested();
}

void ControlPanel::setupWidget() {
   // Setting map generation
   auto* mapGenSection = new QGroupBox("Map generation");
   auto* mapGenLayout = new QGridLayout;

   auto* mapSize = new QSpinBox;
   mapSize->setMinimum(20);
   mapSize->setMaximum(1080);
   mapSize->setSingleStep(1);
   mapSize->setValue(512);

   mapGenLayout->addWidget(new QLabel("Map size:"), 0, 0);
   mapGenLayout->addWidget(mapSize, 0, 1);

   auto* mapGenBtn = new QPushButton("Generate");
   mapGenLayout->addWidget(mapGenBtn, 1, 0, 1, -1);
   mapGenSection->setLayout(mapGenLayout);

   connect(mapGenBtn, &QPushButton::clicked, this,
           [=] { emit generateMap(mapSize->value()); });

   // Setting control panel
   auto* controlLayout = new QHBoxLayout;
   auto* startBtn = new QPushButton("Start", this);
   auto* stopBtn = new QPushButton("Stop", this);
   stopBtn->setDisabled(true);

   controlLayout->addWidget(startBtn);
   controlLayout->addWidget(stopBtn);

   connect(startBtn, &QPushButton::clicked, this,
           &ControlPanel::simulationRequested);
   connect(stopBtn, &QPushButton::clicked, this, &ControlPanel::abortRequested);
   connect(this, &ControlPanel::simulationRequested, this, [=] {
      mapGenSection->setDisabled(true);
      startBtn->setDisabled(true);
      stopBtn->setEnabled(true);
   });
   connect(this, &ControlPanel::abortRequested, this, [=] {
      mapGenSection->setEnabled(true);
      startBtn->setEnabled(true);
      stopBtn->setDisabled(true);
   });

   // Setting main layout
   auto* mainLayout = new QVBoxLayout;
   mainLayout->addWidget(mapGenSection);
   mainLayout->addStretch();
   mainLayout->addLayout(controlLayout);

   setLayout(mainLayout);
}
