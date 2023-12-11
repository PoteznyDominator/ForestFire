#ifndef SIMULATIONWORKER_H
#define SIMULATIONWORKER_H

#include <QWidget>

enum class CellType {
   Grass,
   Tree,
   Water,
   Fire,
   Dirt,
};

typedef QVector<QVector<CellType>> Matrix;

class SimulationWorker : public QObject {
   Q_OBJECT

public:
   SimulationWorker(QObject* parent = nullptr);

   void simulate();
   void abort();
   void setFire(const QPoint& pos);

   Matrix generateMap(int size);

signals:
   void matrixChanged(const Matrix& map);
   void finished();

private:
   Matrix m_map;
   int m_size;
   bool m_isAbort = false;
   float m_probability = 0.3f;

   bool isAdjacentFire(int x, int y);
   bool isSimulationEnd();
};

#endif // SIMULATIONWORKER_H
