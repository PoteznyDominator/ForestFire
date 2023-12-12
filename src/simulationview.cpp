#include "simulationview.h"

#include <QThread>
#include <QWheelEvent>

namespace {
const float zoomScaleFactor = 1.5f;

const QMap<CellType, QColor> cellColors = {
      {CellType::Dirt, QColor(73, 60, 43)},
      {CellType::Grass, QColor(68, 137, 26)},
      {CellType::Tree, QColor(17, 94, 51)},
      {CellType::Water, QColor(35, 149, 198)},
      {CellType::Fire, QColor(236, 71, 0)},
      {CellType::Barrier, Qt::gray},
};
} // namespace

SimulationView::SimulationView(int size, QWidget* parent)
   : QGraphicsView(parent)
   , m_pixmap(new QGraphicsPixmapItem) {
   // maybe use opengl for gpu acceleration
   //   setViewport(new QOpenGLWidget);
   auto* scene = new QGraphicsScene(this);
   scene->addItem(m_pixmap);
   setScene(scene);

   // moving simulation logic to the new thread
   m_worker = new SimulationWorker;
   m_workerThread = new QThread();
   m_worker->moveToThread(m_workerThread);

   // initializing pixmap
   generateMap(size);

   connect(m_worker, &SimulationWorker::matrixChanged, this,
           &SimulationView::updatePixmap);
   connect(m_workerThread, &QThread::started, m_worker,
           &SimulationWorker::simulate);
   connect(m_worker, &SimulationWorker::finished, this, &SimulationView::stop);
   connect(m_worker, &SimulationWorker::finished, this,
           &SimulationView::finished);
}

SimulationView::~SimulationView() {
   m_worker->abort();
   m_worker->deleteLater();
   m_workerThread->deleteLater();
}

void SimulationView::simulate(int grassProb, int treeProb) {
   m_worker->setProbability(grassProb, treeProb);
   m_workerThread->start();
   m_simulationRunning = true;
}

void SimulationView::stop() {
   m_worker->abort();
   m_workerThread->quit();
   m_simulationRunning = false;
}

void SimulationView::generateMap(int mapSize) {
   m_pixmap->setPixmap(getPixmap(m_worker->generateMap(mapSize)));

   setSceneRect(m_pixmap->boundingRect());
}

void SimulationView::wheelEvent(QWheelEvent* event) {
   setTransformationAnchor(NoAnchor);

   const auto oldPos = mapToScene(event->position().toPoint());

   if (event->angleDelta().y() > 0) {
      scale(zoomScaleFactor, zoomScaleFactor);
   } else {
      scale(1 / zoomScaleFactor, 1 / zoomScaleFactor);
   }

   const auto newPos = mapToScene(event->position().toPoint());
   const auto delta = newPos - oldPos;
   translate(delta.x(), delta.y());
   setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void SimulationView::mousePressEvent(QMouseEvent* event) {
   if (event->button() == Qt::MiddleButton) {
      // Store original position
      m_originX = event->position().x();
      m_originY = event->position().y();
      setCursor(Qt::ClosedHandCursor);
   }

   if (!m_simulationRunning && event->button() == Qt::LeftButton) {
      const auto pos = mapToScene(event->pos()).toPoint();
      m_worker->setFire(pos);
   }

   if (!m_simulationRunning && event->button() == Qt::RightButton) {
      const auto pos = mapToScene(event->pos()).toPoint();
      m_worker->setBarrier(pos);
   }
}

void SimulationView::mouseReleaseEvent(QMouseEvent* event) {
   if (event->button() == Qt::MiddleButton) {
      setCursor(Qt::ArrowCursor);
   }
}

void SimulationView::mouseMoveEvent(QMouseEvent* event) {
   if (event->buttons() & Qt::MiddleButton) {
      QPointF oldP = mapToScene(m_originX, m_originY);
      QPointF newP = mapToScene(event->pos());
      QPointF translation = newP - oldP;

      setTransformationAnchor(QGraphicsView::NoAnchor);

      translate(translation.x(), translation.y());

      m_originX = event->position().x();
      m_originY = event->position().y();
      setTransformationAnchor(QGraphicsView::AnchorViewCenter);
   }

   if (!m_simulationRunning && event->buttons() & Qt::LeftButton) {
      const auto pos = mapToScene(event->pos()).toPoint();
      m_worker->setFire(pos);
   }

   if (!m_simulationRunning && event->buttons() & Qt::RightButton) {
      const auto pos = mapToScene(event->pos()).toPoint();
      m_worker->setBarrier(pos);
   }
}

void SimulationView::updatePixmap(const Matrix& map) {
   m_pixmap->setPixmap(getPixmap(map));
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
