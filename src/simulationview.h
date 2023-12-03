#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsView>

class SimulationView : public QGraphicsView {
public:
   SimulationView(QWidget* parent = nullptr);

protected:
   void wheelEvent(QWheelEvent* event) override;

private:
   QGraphicsPixmapItem* m_pixmap;

   void initializeMap(int size);
};

#endif // SIMULATIONVIEW_H
