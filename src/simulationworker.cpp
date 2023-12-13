#include "simulationworker.h"
#include "vendor/FastNoiseLite.h"

#include <QRandomGenerator>
#include <QThread>

namespace {
CellType getCellType(float value, float moist) {
   if (value < 0.35) {
      return CellType::Water;
   } else {
      if (moist < 0.5) {
         return CellType::Tree;
      } else {
         return CellType::Grass;
      }
   }
}
} // namespace

SimulationWorker::SimulationWorker(QObject* parent)
   : QObject(parent) {}

void SimulationWorker::setProbability(int grassProb, int treeProb) {
   m_probabilities[CellType::Grass] = grassProb / 100.f;
   m_probabilities[CellType::Tree] = treeProb / 100.f;
}

void SimulationWorker::simulate() {
   // init value
   m_isAbort = false;
   m_results = SimulationResult{};

   Matrix copy;
   while (!isSimulationEnd() && !m_isAbort) {
      copy = m_map;
      m_results.iterationCount++;
      for (int x = 0; x < m_size; x++) {
         for (int y = 0; y < m_size; y++) {
            const auto cell = copy[x][y];
            if (isAdjacentFire(x, y) &&
                (cell == CellType::Grass || cell == CellType::Tree) &&
                QRandomGenerator::global()->generateDouble() <
                      m_probabilities.value(cell)) {
               copy[x][y] = CellType::Fire;
               continue;
            }

            if (cell == CellType::Fire) {
               copy[x][y] = CellType::Dirt;
            }
         }
      }
      m_map = copy;

      emit matrixChanged(m_map);
      QThread::msleep(10);
   }

   for (int x = 0; x < m_size; x++) {
      for (int y = 0; y < m_size; y++) {
         if (m_map[x][y] == CellType::Dirt) {
            m_results.burnedGround++;
         } else if (m_map[x][y] == CellType::Grass ||
                    m_map[x][y] == CellType::Tree) {
            m_results.grassCount++;
         }
      }
   }

   emit finished(m_results);
}

void SimulationWorker::abort() {
   m_isAbort = true;
}

void SimulationWorker::setFire(const QPoint& pos) {
   // only apply change in the size of the map
   if (pos.x() >= m_size || pos.x() < 0 || pos.y() >= m_size || pos.y() < 0) {
      return;
   }

   if (m_map[pos.x()][pos.y()] == CellType::Grass ||
       m_map[pos.x()][pos.y()] == CellType::Tree) {
      m_map[pos.x()][pos.y()] = CellType::Fire;
      emit matrixChanged(m_map);
   }
}

void SimulationWorker::setBarrier(const QPoint& pos) {
   if (pos.x() >= m_size || pos.x() < 0 || pos.y() >= m_size || pos.y() < 0) {
      return;
   }

   if (m_map[pos.x()][pos.y()] == CellType::Grass ||
       m_map[pos.x()][pos.y()] == CellType::Tree) {
      m_map[pos.x()][pos.y()] = CellType::Barrier;
      emit matrixChanged(m_map);
   }
}

Matrix SimulationWorker::generateMap(int size) {
   FastNoiseLite alt;
   alt.SetSeed(QRandomGenerator::global()->generate());
   alt.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   alt.SetFractalOctaves(5);
   alt.SetFrequency(0.03f);

   FastNoiseLite moist;
   moist.SetSeed(QRandomGenerator::global()->generate());
   moist.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   moist.SetFractalOctaves(5);
   moist.SetFrequency(0.0185f);
   moist.SetFractalGain(8.f);

   m_size = size;
   Matrix map(size, QVector<CellType>(size, CellType::Grass));

   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
         // normalizing values [0, 1]
         float a = (alt.GetNoise((float) i, (float) j) + 1) / 2;
         float m = (moist.GetNoise((float) i, (float) j) + 1) / 2;

         map[i][j] = getCellType(a, m);
      }
   }

   m_map = map;
   return m_map;
}

bool SimulationWorker::isAdjacentFire(int x, int y) {
   QVector<CellType> adjacents;

   for (int i = x - 1; i <= x + 1; i++) {
      for (int j = y - 1; j <= y + 1; j++) {
         if (i >= 0 && i < m_size && j >= 0 && j < m_size) {
            adjacents << m_map[i][j];
         }
      }
   }

   return adjacents.contains(CellType::Fire);
}

bool SimulationWorker::isSimulationEnd() {
   for (int x = 0; x < m_size; x++) {
      for (int y = 0; y < m_size; y++) {
         if (m_map[x][y] == CellType::Fire) {
            return false;
         }
      }
   }

   return true;
}
