#ifndef SIMULATIONWORKER_H
#define SIMULATIONWORKER_H

#include "vendor/FastNoiseLite.h"

#include <QWidget>

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
   SimulationWorker(QObject* parent = nullptr);

   void simulate();
   void abort();
   void setFire(const QPoint& pos);

   Matrix generateMap(int size);

signals:
   void matrixChanged(const Matrix& map);
   void finished();

private:
   FastNoiseLite noise;
   Matrix m_map;
   int m_size;
   bool m_isAbort = false;
   float m_probability = 0.3f;

   bool isAdjacentFire(int x, int y);
   bool isSimulationEnd();
};

#endif // SIMULATIONWORKER_H
