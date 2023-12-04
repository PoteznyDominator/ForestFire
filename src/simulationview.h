#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsView>

class SimulationView : public QGraphicsView {
   Q_OBJECT
public:
   SimulationView(int size, QWidget* parent = nullptr);

   void simulate();

protected:
   void keyPressEvent(QKeyEvent* event) override;
   void wheelEvent(QWheelEvent* event) override;

private:
   enum CellType {
      Grass,
      Water,
      Fire,
      Dirt,
   };

   int m_size;
   QVector<QVector<CellType>> m_matrix;
   QGraphicsPixmapItem* m_pixmap;

   void initializeMap(int size);
   CellType getCellType(float value);
   bool isAdjacentFire(int x, int y);
   QPixmap getPixmap();

signals:
   void simulationTickEnd();
};

#endif // SIMULATIONVIEW_H
