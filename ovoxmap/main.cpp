#include <OGRE/Ogre.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <QMainWindow>
#include <QApplication>
#include <QFileDialog>
#include <vector>

class VoxelEditor {
public:
    VoxelEditor();
    ~VoxelEditor();
    void run();

private:
    void initOgre();
    void createScene();
    void handleEvents();
    void renderFrame();
    void loadVoxels();
    void saveVoxels();
    void selectTexture();
    void applyTexture(int x, int y, int z);

    SDL_Window *mWindow;
    SDL_GLContext mGLContext;
    Ogre::Root *mRoot;
    Ogre::RenderWindow *mOgreWindow;
    Ogre::SceneManager *mSceneMgr;
    Ogre::Camera *mCamera;
    Ogre::SceneNode *mVoxelGridNode;
    std::vector<std::vector<std::vector<Ogre::Entity*>>> mVoxels;
    int mGridSize;
    float mVoxelSize;
    bool mRunning;
    QString mCurrentTexture;

    Uint32 mLastClickTime;
    int mLastClickX, mLastClickY;
    float mZoomLevel;
};

VoxelEditor::VoxelEditor()
    : mWindow(nullptr),
      mRoot(nullptr),
      mOgreWindow(nullptr),
      mSceneMgr(nullptr),
      mCamera(nullptr),
      mVoxelGridNode(nullptr),
      mGridSize(10),
      mVoxelSize(1.0f),
      mRunning(true),
      mLastClickTime(0),
      mLastClickX(0),
      mLastClickY(0),
      mZoomLevel(15.0f) {
    mVoxels.resize(mGridSize, std::vector<std::vector<Ogre::Entity*>>(mGridSize, std::vector<Ogre::Entity*>(mGridSize, nullptr)));
    mCurrentTexture = ":/textures/default.png";
}

VoxelEditor::~VoxelEditor() {
    delete mRoot;
    SDL_GL_DeleteContext(mGLContext);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void VoxelEditor::run() {
    SDL_Init(SDL_INIT_VIDEO);
    mWindow = SDL_CreateWindow("Voxel Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
    mGLContext = SDL_GL_CreateContext(mWindow);

    initOgre();
    createScene();

    while (mRunning) {
        handleEvents();
        renderFrame();
    }
}

void VoxelEditor::initOgre() {
    mRoot = new Ogre::Root("plugins.cfg");

    if (!(mRoot->restoreConfig() || mRoot->showConfigDialog())) {
        throw std::runtime_error("Failed to initialize Ogre");
    }

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(mWindow, &wmInfo);
    Ogre::NameValuePairList params;
    params["externalWindowHandle"] = Ogre::StringConverter::toString(size_t(wmInfo.info.win.window));

    mOgreWindow = mRoot->createRenderWindow("OgreWindow", 800, 600, false, &params);
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
    mCamera = mSceneMgr->createCamera("MainCam");
    mCamera->setPosition(Ogre::Vector3(0, 0, 15));
    mCamera->lookAt(Ogre::Vector3(0, 0, 0));
    mCamera->setNearClipDistance(0.1f);

    Ogre::Viewport *vp = mOgreWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void VoxelEditor::createScene() {
    mVoxelGridNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("VoxelGrid");

    for (int x = 0; x < mGridSize; ++x) {
        for (int y = 0; y < mGridSize; ++y) {
            for (int z = 0; z < mGridSize; ++z) {
                Ogre::Entity *voxel = mSceneMgr->createEntity("Cube.mesh");
                voxel->setMaterialName("Examples/Rockwall");
                Ogre::SceneNode *node = mVoxelGridNode->createChildSceneNode(Ogre::Vector3(x, y, z));
                node->attachObject(voxel);
                mVoxels[x][y][z] = voxel;
            }
        }
    }
}

void VoxelEditor::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            mRunning = false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - mLastClickTime < 500) {
                    selectTexture();
                } else {
                    mLastClickTime = currentTime;
                    mLastClickX = event.button.x;
                    mLastClickY = event.button.y;
                }
            }
        }
    }
}

void VoxelEditor::renderFrame() {
    mRoot->renderOneFrame();
}

void VoxelEditor::loadVoxels() {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Voxel File", "", "Voxel Files (*.vox)");
    if (!fileName.isEmpty()) {
        std::ifstream file(fileName.toStdString(), std::ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(mVoxels.data()), mVoxels.size() * sizeof(Ogre::Entity*));
            file.close();
        }
    }
}

void VoxelEditor::saveVoxels() {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Voxel File", "", "Voxel Files (*.vox)");
    if (!fileName.isEmpty()) {
        std::ofstream file(fileName.toStdString(), std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(mVoxels.data()), mVoxels.size() * sizeof(Ogre::Entity*));
            file.close();
        }
    }
}

void VoxelEditor::selectTexture() {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Select Texture", "", "Image Files (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        mCurrentTexture = fileName;
        applyTexture(mLastClickX / mVoxelSize, mLastClickY / mVoxelSize, 0); // Assuming 2D plane for simplicity
    }
}

void VoxelEditor::applyTexture(int x, int y, int z) {
    if (x >= 0 && x < mGridSize && y >= 0 && y < mGridSize && z >= 0 && z < mGridSize) {
        Ogre::Entity *voxel = mVoxels[x][y][z];
        voxel->setMaterialName(mCurrentTexture.toStdString());
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    VoxelEditor editor;
    editor.run();
    return app.exec();
}

