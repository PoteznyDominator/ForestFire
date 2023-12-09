#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

class ControlPanel : public QWidget {
   Q_OBJECT
public:
   ControlPanel(QWidget* parent = nullptr);

   void onSimulationFinished();

signals:
   void simulationRequested();
   void abortRequested();
   void generateMap(int size);

private:
   void setupWidget();
};

#endif // CONTROLPANEL_H
