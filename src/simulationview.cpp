#include "simulationview.h"
#include "vendor/FastNoiseLite.h"

#include <QThread>
#include <QWheelEvent>

namespace {
const float zoomScaleFactor = 1.5f;
}

SimulationView::SimulationView(int size, QWidget* parent)
   : QGraphicsView(parent)
   , m_size(size)
   , m_matrix(m_size, QVector<CellType>(m_size, CellType::Grass))
   , m_pixmap(new QGraphicsPixmapItem) {
   auto* scene = new QGraphicsScene(this);
   scene->addItem(m_pixmap);
   setScene(scene);
   setDragMode(ScrollHandDrag);

   initializeMap(m_size);

   connect(this, &SimulationView::simulationTickEnd, this,
           &SimulationView::simulate);
}

void SimulationView::simulate() {
   QVector<QVector<CellType>> copy(m_matrix);

   for (int x = 0; x < m_size; x++) {
      for (int y = 0; y < m_size; y++) {
         if (isAdjacentFire(x, y) && copy[x][y] == CellType::Grass) {
            copy[x][y] = CellType::Fire;
            continue;
         }

         if (copy[x][y] == CellType::Fire) {
            copy[x][y] = CellType::Dirt;
         }
      }
   }
   m_matrix = copy;

   m_pixmap->setPixmap(getPixmap());
   //   QThread::msleep(50);

   //   emit simulationTickEnd();
}

void SimulationView::keyPressEvent(QKeyEvent* event) {
   if (event->key() == Qt::Key_Space) {
      simulate();
   }

   QWidget::keyPressEvent(event);
}

void SimulationView::wheelEvent(QWheelEvent* event) {
   setTransformationAnchor(AnchorUnderMouse);

   if (event->angleDelta().y() > 0) {
      scale(zoomScaleFactor, zoomScaleFactor);
   } else {
      scale(1 / zoomScaleFactor, 1 / zoomScaleFactor);
   }
}

void SimulationView::initializeMap(int size) {
   FastNoiseLite noise;
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
         // normalizing value [0, 1]
         float value = noise.GetNoise((float) i, (float) j);
         value = (value + 1) / 2;

         m_matrix[i][j] = getCellType(value);
      }
   }

   m_matrix[100][100] = CellType::Fire;

   m_pixmap->setPixmap(getPixmap());
}

SimulationView::CellType SimulationView::getCellType(float value) {
   if (value < 0.5) {
      return CellType::Water;
   } else {
      return CellType::Grass;
   }
}

bool SimulationView::isAdjacentFire(int x, int y) {
   QVector<CellType> adjacents;

   for (int i = x - 1; i <= x + 1; i++) {
      for (int j = y - 1; j <= y + 1; j++) {
         if (i >= 0 && i < m_size && j >= 0 && j < m_size) {
            adjacents << m_matrix[i][j];
         }
      }
   }

   return adjacents.contains(CellType::Fire);
}

QPixmap SimulationView::getPixmap() {
   QImage img(m_size, m_size, QImage::Format_RGB32);

   for (int i = 0; i < m_size; i++) {
      for (int j = 0; j < m_size; j++) {
         auto value = m_matrix[i][j];

         if (value == CellType::Grass) {
            img.setPixelColor(i, j, Qt::darkGreen);
         } else if (value == CellType::Water) {
            img.setPixelColor(i, j, Qt::darkBlue);
         } else if (value == CellType::Fire) {
            img.setPixelColor(i, j, Qt::red);
         } else if (value == CellType::Dirt) {
            img.setPixelColor(i, j, Qt::black);
         }
      }
   }

   return QPixmap::fromImage(img);
}
