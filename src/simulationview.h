#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include "simulationworker.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>

class SimulationView : public QGraphicsView {
   Q_OBJECT
public:
   SimulationView(int size, QWidget* parent = nullptr);
   ~SimulationView();

   void simulate(int grassProb, int treeProb);
   void stop();
   void generateMap(int mapSize);

signals:
   void finished();

protected:
   void wheelEvent(QWheelEvent* event) override;
   void mousePressEvent(QMouseEvent* event) override;
   void mouseReleaseEvent(QMouseEvent* event) override;
   void mouseMoveEvent(QMouseEvent* event) override;

private:
   bool m_simulationRunning = false;
   int m_originX;
   int m_originY;

   QGraphicsPixmapItem* m_pixmap;
   SimulationWorker* m_worker;
   QThread* m_workerThread;

   void updatePixmap(const Matrix& map);

   QPixmap getPixmap(const Matrix& map);
};

#endif // SIMULATIONVIEW_H
