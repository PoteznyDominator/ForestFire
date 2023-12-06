#include "simulationview.h"
#include "vendor/FastNoiseLite.h"

#include <QRandomGenerator>
#include <QThread>
#include <QWheelEvent>

namespace {
const float zoomScaleFactor = 1.5f;

const QMap<CellType, QColor> cellColors = {
      {CellType::Dirt, Qt::black},
      {CellType::Grass, Qt::darkGreen},
      {CellType::Water, Qt::darkBlue},
      {CellType::Fire, Qt::red},
};
} // namespace

SimulationWorker::SimulationWorker(const Matrix& matrix, QObject* parent)
   : QObject(parent)
   , m_size(matrix.size())
   , m_map(matrix) {}

void SimulationWorker::simulate() {
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
}

void SimulationWorker::abort() {
   m_isAbort = true;
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

SimulationView::SimulationView(int size, QWidget* parent)
   : QGraphicsView(parent)
   , m_pixmap(new QGraphicsPixmapItem) {
   // maybe use opengl for gpu acceleration
   //   setViewport(new QOpenGLWidget);
   auto* scene = new QGraphicsScene(this);
   scene->addItem(m_pixmap);
   setScene(scene);
   setDragMode(ScrollHandDrag);

   auto map = initializeMap(size);

   // moving simulation logic to the new thread
   m_worker = new SimulationWorker(map);
   auto* thread = new QThread();
   m_worker->moveToThread(thread);

   connect(m_worker, &SimulationWorker::matrixChanged, this,
           &SimulationView::updatePixmap);
   connect(thread, &QThread::started, m_worker, &SimulationWorker::simulate);
   connect(this, &SimulationView::simulateRequested, thread,
           [thread] { thread->start(); });
}

SimulationView::~SimulationView() {
   m_worker->abort();
   m_worker->deleteLater();
   m_workerThread->deleteLater();
}

void SimulationView::simulate() {
   emit simulateRequested();
}

void SimulationView::wheelEvent(QWheelEvent* event) {
   setTransformationAnchor(AnchorUnderMouse);

   if (event->angleDelta().y() > 0) {
      scale(zoomScaleFactor, zoomScaleFactor);
   } else {
      scale(1 / zoomScaleFactor, 1 / zoomScaleFactor);
   }
}

void SimulationView::updatePixmap(const Matrix& map) {
   m_pixmap->setPixmap(getPixmap(map));
}

Matrix SimulationView::initializeMap(int size) {
   FastNoiseLite noise;
   noise.SetSeed(QRandomGenerator::global()->generate());
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
   //   noise.SetFractalGain(0.3f);
   //   noise.SetFrequency(0.02f);

   Matrix map(size, QVector<CellType>(size, CellType::Grass));

   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
         // normalizing value [0, 1]
         float value = noise.GetNoise((float) i, (float) j);
         value = (value + 1) / 2;

         map[i][j] = getCellType(value);
      }
   }

   map[100][100] = CellType::Fire;

   m_pixmap->setPixmap(getPixmap(map));
   return map;
}

CellType SimulationView::getCellType(float value) {
   if (value < 0.4) {
      return CellType::Water;
   } else {
      return CellType::Grass;
   }
}

QPixmap SimulationView::getPixmap(const Matrix& map) {
   const int size = map.size();
   QImage img(size, size, QImage::Format_RGB32);

   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
         auto value = map[i][j];
         img.setPixelColor(i, j, cellColors.value(value));
      }
   }

   return QPixmap::fromImage(img);
}
