#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsView>

enum class CellType {
   Grass,
   Water,
   Fire,
   Dirt,
};

typedef QVector<QVector<CellType>> Matrix;

class SimulationWorker : public QObject {
   Q_OBJECT

public:
   SimulationWorker(const Matrix& map, QObject* parent = nullptr);

   void simulate();
   void abort();

signals:
   void matrixChanged(const Matrix& map);
   void finished();

private:
   bool m_isAbort = false;
   float m_probability = 0.3f;
   int m_size;
   Matrix m_map;

   bool isAdjacentFire(int x, int y);
   bool isSimulationEnd();
};

class SimulationView : public QGraphicsView {
   Q_OBJECT
public:
   SimulationView(int size, QWidget* parent = nullptr);
   ~SimulationView();

   void simulate();

protected:
   void wheelEvent(QWheelEvent* event) override;

private:
   QGraphicsPixmapItem* m_pixmap;
   SimulationWorker* m_worker;
   QThread* m_workerThread;

   void updatePixmap(const Matrix& map);

   Matrix initializeMap(int size);
   CellType getCellType(float value);
   QPixmap getPixmap(const Matrix& map);

signals:
   void simulateRequested();
};

#endif // SIMULATIONVIEW_H
