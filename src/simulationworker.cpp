#include "simulationworker.h"

#include <QRandomGenerator>
#include <QThread>

namespace {
CellType getCellType(float value) {
   if (value < 0.4) {
      return CellType::Water;
   } else {
      return CellType::Grass;
   }
}
} // namespace

SimulationWorker::SimulationWorker(QObject* parent)
   : QObject(parent) {}

void SimulationWorker::simulate() {
   // init value
   m_isAbort = false;

   Matrix copy;
   while (!isSimulationEnd() && !m_isAbort) {
      copy = m_map;
      for (int x = 0; x < m_size; x++) {
         for (int y = 0; y < m_size; y++) {
            if (isAdjacentFire(x, y) && copy[x][y] == CellType::Grass &&
                QRandomGenerator::global()->generateDouble() < m_probability) {
               copy[x][y] = CellType::Fire;
               continue;
            }

            if (copy[x][y] == CellType::Fire) {
               copy[x][y] = CellType::Dirt;
            }
         }
      }
      m_map = copy;

      emit matrixChanged(m_map);
      QThread::msleep(100);
   }

   emit finished();
}

void SimulationWorker::abort() {
   m_isAbort = true;
}

void SimulationWorker::setFire(const QPoint& pos) {
   // only apply change in the size of the map
   if (pos.x() >= m_size || pos.x() < 0 || pos.y() >= m_size || pos.y() < 0) {
      return;
   }
   qDebug() << pos;

   if (m_map[pos.x()][pos.y()] == CellType::Grass) {
      m_map[pos.x()][pos.y()] = CellType::Fire;
      emit matrixChanged(m_map);
   }
}

Matrix SimulationWorker::generateMap(int size) {
   noise.SetSeed(QRandomGenerator::global()->generate());
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   noise.SetFractalOctaves(5);
   noise.SetFrequency(0.02f);
   //   noise.SetFractalGain(0.3f);
   //   noise.SetFrequency(0.02f);

   m_size = size;
   Matrix map(size, QVector<CellType>(size, CellType::Grass));

   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
         // normalizing value [0, 1]
         float value = noise.GetNoise((float) i, (float) j);
         value = (value + 1) / 2;

         map[i][j] = getCellType(value);
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
