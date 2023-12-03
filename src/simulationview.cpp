#include "simulationview.h"
#include "vendor/FastNoiseLite.h"

#include <QWheelEvent>

namespace {
const float zoomScaleFactor = 1.5f;
}

SimulationView::SimulationView(QWidget* parent)
   : QGraphicsView(parent)
   , m_pixmap(new QGraphicsPixmapItem) {
   auto* scene = new QGraphicsScene(this);
   scene->addItem(m_pixmap);
   setScene(scene);
   setDragMode(ScrollHandDrag);

   initializeMap(1080);
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
   QImage image(size, size, QImage::Format_RGB32);
   FastNoiseLite noise;
   noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
         // normalizing value [0, 1]
         float value = noise.GetNoise((float) i, (float) j);
         value = (value + 1) / 2;

         auto color = value < 0 ? Qt::darkGreen : Qt::green;
         image.setPixelColor(i, j, color);
      }
   }

   m_pixmap->setPixmap(QPixmap::fromImage(image));
}
