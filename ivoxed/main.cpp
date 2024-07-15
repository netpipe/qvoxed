#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMenuBar>
#include <vector>
#include <fstream>
#include <iostream>
#include <GL/glut.h>


class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr)
        : QOpenGLWidget(parent), gridSize(10), voxelSize(1.0f), snapToGrid(true), zoomLevel(15.0f), cameraX(0.0f), cameraY(0.0f) {
        voxelGrid.resize(gridSize * gridSize * gridSize, false);
    }

public slots:
    // void loadVoxels() {
    //     QString fileName = QFileDialog::getOpenFileName(this, "Open Voxel File", "", "Voxel Files (*.vox)");
    //     if (!fileName.isEmpty()) {
    //         std::ifstream file(fileName.toStdString(), std::ios::binary);
    //         if (file.is_open()) {
    //             file.read(reinterpret_cast<char*>(voxelGrid.data()), voxelGrid.size() * sizeof(bool));
    //             file.close();
    //             update();
    //         }
    //     }
    // }

    // void saveVoxels() {
    //     QString fileName = QFileDialog::getSaveFileName(this, "Save Voxel File", "", "Voxel Files (*.vox)");
    //     if (!fileName.isEmpty()) {
    //         std::ofstream file(fileName.toStdString(), std::ios::binary);
    //         if (file.is_open()) {
    //             file.write(reinterpret_cast<const char*>(voxelGrid.data()), voxelGrid.size() * sizeof(bool));
    //             file.close();
    //         }
    //     }
    // }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, static_cast<double>(w) / h, 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(cameraX, cameraY, zoomLevel, cameraX, cameraY, 0.0, 0.0, 1.0, 0.0);

        for (int x = 0; x < gridSize; ++x) {
            for (int y = 0; y < gridSize; ++y) {
                for (int z = 0; z < gridSize; ++z) {
                    if (voxelGrid[index(x, y, z)]) {
                        drawVoxel(x, y, z);
                    }
                }
            }
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        GLint viewport[4];
        GLdouble modelview[16];
        GLdouble projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;

        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        winX = (float)event->x();
        winY = (float)viewport[3] - (float)event->y() - 1;
        glReadPixels(event->x(), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        int voxelX = static_cast<int>(qRound(posX / voxelSize + gridSize / 2));
        int voxelY = static_cast<int>(qRound(posY / voxelSize + gridSize / 2));
        int voxelZ = static_cast<int>(qRound(posZ / voxelSize + gridSize / 2));

        if (snapToGrid) {
            voxelX = voxelX - (voxelX % 1);
            voxelY = voxelY - (voxelY % 1);
            voxelZ = voxelZ - (voxelZ % 1);
        }

        if (voxelX >= 0 && voxelX < gridSize &&
            voxelY >= 0 && voxelY < gridSize &&
            voxelZ >= 0 && voxelZ < gridSize) {
            if (event->button() == Qt::LeftButton) {
                voxelGrid[index(voxelX, voxelY, voxelZ)] = true;
            } else if (event->button() == Qt::RightButton) {
                voxelGrid[index(voxelX, voxelY, voxelZ)] = false;
            }
            update();
        }
    }

    void wheelEvent(QWheelEvent *event) override {
        zoomLevel -= event->delta() / 120.0f;
        if (zoomLevel < 1.0f) zoomLevel = 1.0f;
        update();
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::MiddleButton) {
            cameraX -= (event->x() - lastMousePosition.x()) / 100.0f;
            cameraY += (event->y() - lastMousePosition.y()) / 100.0f;
            update();
        }
        lastMousePosition = event->pos();
    }

private:
    void drawVoxel(int x, int y, int z) {
        glPushMatrix();
        glTranslatef(x * voxelSize - gridSize / 2 * voxelSize, y * voxelSize - gridSize / 2 * voxelSize, z * voxelSize - gridSize / 2 * voxelSize);

        glBegin(GL_QUADS);
        glColor3f(1.0, 0.0, 0.0);

        // Front face
        glVertex3f(-0.5, -0.5,  0.5);
        glVertex3f( 0.5, -0.5,  0.5);
        glVertex3f( 0.5,  0.5,  0.5);
        glVertex3f(-0.5,  0.5,  0.5);

        // Back face
        glVertex3f(-0.5, -0.5, -0.5);
        glVertex3f( 0.5, -0.5, -0.5);
        glVertex3f( 0.5,  0.5, -0.5);
        glVertex3f(-0.5,  0.5, -0.5);

        // Left face
        glVertex3f(-0.5, -0.5, -0.5);
        glVertex3f(-0.5, -0.5,  0.5);
        glVertex3f(-0.5,  0.5,  0.5);
        glVertex3f(-0.5,  0.5, -0.5);

        // Right face
        glVertex3f(0.5, -0.5, -0.5);
        glVertex3f(0.5, -0.5,  0.5);
        glVertex3f(0.5,  0.5,  0.5);
        glVertex3f(0.5,  0.5, -0.5);

        // Top face
        glVertex3f(-0.5,  0.5, -0.5);
        glVertex3f( 0.5,  0.5, -0.5);
        glVertex3f( 0.5,  0.5,  0.5);
        glVertex3f(-0.5,  0.5,  0.5);

        // Bottom face
        glVertex3f(-0.5, -0.5, -0.5);
        glVertex3f( 0.5, -0.5, -0.5);
        glVertex3f( 0.5, -0.5,  0.5);
        glVertex3f(-0.5, -0.5,  0.5);

        glEnd();
        glPopMatrix();
    }

    int index(int x, int y, int z) const {
        return x + gridSize * (y + gridSize * z);
    }

    int gridSize;
    float voxelSize;
    bool snapToGrid;
    float zoomLevel;
    float cameraX, cameraY;
    QPoint lastMousePosition;
    std::vector<bool> voxelGrid;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent) {
        openGLWidget = new OpenGLWidget(this);
        setCentralWidget(openGLWidget);
        resize(800, 600); // Set initial window size

        QMenuBar *menuBar = new QMenuBar(this);
        setMenuBar(menuBar);

        QMenu *fileMenu = menuBar->addMenu("File");
        QAction *loadAction = fileMenu->addAction("Load");
        QAction *saveAction = fileMenu->addAction("Save");

    //    connect(loadAction, &QAction::triggered, openGLWidget, &OpenGLWidget::loadVoxels);
     //   connect(saveAction, &QAction::triggered, openGLWidget, &OpenGLWidget::saveVoxels);
    }

private:
    OpenGLWidget *openGLWidget;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

#include "main.moc"
