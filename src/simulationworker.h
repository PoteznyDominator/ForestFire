#ifndef SIMULATIONWORKER_H
#define SIMULATIONWORKER_H

#include <QWidget>

struct SimulationResult {
   int iterationCount = 0;
   int burnedGround = 0;
   int grassCount = 0;
};

enum class CellType {
   Grass,
   Tree,
   Water,
   Fire,
   Dirt,
   Barrier,
};

typedef QVector<QVector<CellType>> Matrix;

class SimulationWorker : public QObject {
   Q_OBJECT

public:
   SimulationWorker(QObject* parent = nullptr);

   void setProbability(int grassProb, int treeProb);
   void simulate();
   void abort();
   void setFire(const QPoint& pos);
   void setBarrier(const QPoint& pos);

   Matrix generateMap(int size);

signals:
   void matrixChanged(const Matrix& map);
   void finished(SimulationResult results);

private:
   SimulationResult m_results;
   Matrix m_map;
   int m_size;
   bool m_isAbort = false;
   // probabilities with default parameters
   QMap<CellType, float> m_probabilities = {
         {CellType::Tree, 0.4},
         {CellType::Grass, 0.8},
   };

   bool isAdjacentFire(int x, int y);
   bool isSimulationEnd();
};

#endif // SIMULATIONWORKER_H
