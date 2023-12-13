#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include "simulationworker.h"

#include <QWidget>

class QLabel;

class ControlPanel : public QWidget {
   Q_OBJECT
public:
   ControlPanel(QWidget* parent = nullptr);

   void onSimulationFinished(SimulationResult results);

signals:
   void simulationRequested(int grassProb, int treeProb);
   void abortRequested();
   void generateMap(int size);

private:
   QLabel* m_resultsLabel;

   void setupWidget();
   void setResultMessage(const SimulationResult& results);
};

#endif // CONTROLPANEL_H
