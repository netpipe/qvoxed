#include <irrlicht/irrlicht.h>
#include <iostream>
#include <unordered_map>

using namespace irr;

class VoxelNode {
public:
    VoxelNode(scene::IMeshSceneNode* node, int type, float weight);

    scene::IMeshSceneNode* getNode() const;
    void setNode(scene::IMeshSceneNode* node);

    int getAge() const;
    void setAge(int age);

    int getType() const;
    void setType(int type);

    float getLastSize() const;
    void setLastSize(float size);

    float getWeight() const;
    void setWeight(float weight);

    float getAffectorValue() const;
    void setAffectorValue(float value);

private:
    scene::IMeshSceneNode* node;
    int age;
    int type;
    float lastSize;
    float weight;
    float affectorValue;
};


VoxelNode::VoxelNode(scene::IMeshSceneNode* node, int type, float weight)
    : node(node), age(0), type(type), lastSize(1.0f), weight(weight), affectorValue(0.0f) {}

scene::IMeshSceneNode* VoxelNode::getNode() const {
    return node;
}

void VoxelNode::setNode(scene::IMeshSceneNode* node) {
    this->node = node;
}

int VoxelNode::getAge() const {
    return age;
}

void VoxelNode::setAge(int age) {
    this->age = age;
}

int VoxelNode::getType() const {
    return type;
}

void VoxelNode::setType(int type) {
    this->type = type;
}

float VoxelNode::getLastSize() const {
    return lastSize;
}

void VoxelNode::setLastSize(float size) {
    this->lastSize = size;
}

float VoxelNode::getWeight() const {
    return weight;
}

void VoxelNode::setWeight(float weight) {
    this->weight = weight;
}

float VoxelNode::getAffectorValue() const {
    return affectorValue;
}

void VoxelNode::setAffectorValue(float value) {
    this->affectorValue = value;
}



class VoxelEditor : public IEventReceiver {
public:
    VoxelEditor();
    ~VoxelEditor();
    void run();

    // Override the OnEvent function to handle events
    virtual bool OnEvent(const SEvent &event);

private:
    void createScene();
    void placeVoxel(const core::vector3df &position);
    void removeVoxel(scene::ISceneNode* node);
    void selectTexture();
        void scaleVoxel(VoxelNode* voxelNode, float scale);

    IrrlichtDevice *mDevice;
    video::IVideoDriver *mDriver;
    scene::ISceneManager *mSceneMgr;
    scene::ICameraSceneNode *mCamera;
    bool mRunning;
    std::string mCurrentTexture;
scene::ISceneCollisionManager* cm;
    u32 mLastClickTime;
    core::vector3df mLastClickPos;
    std::unordered_map<scene::ISceneNode*, core::vector3df> mVoxelMap;
        bool mLeftMousePressed;
    bool mRightMousePressed;
};

VoxelEditor::VoxelEditor()
    : mDevice(nullptr),
      mDriver(nullptr),
      mSceneMgr(nullptr),
      mCamera(nullptr),
      mRunning(true),
      mLastClickTime(0),
      mLeftMousePressed(false),
      mRightMousePressed(false) {
    mCurrentTexture = "default.png";
}

VoxelEditor::~VoxelEditor() {
    if (mDevice) {
        mDevice->drop();
    }
}



void VoxelEditor::run() {
    mDevice = createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800, 600), 16, false, false, false, nullptr);
    if (!mDevice) {
        throw std::runtime_error("Failed to initialize Irrlicht");
    }

    mDriver = mDevice->getVideoDriver();
    mSceneMgr = mDevice->getSceneManager();
    mCamera = mSceneMgr->addCameraSceneNodeFPS();
    mCamera->setPosition(core::vector3df(0, 30, -40));
  //  mCamera->lookAt(core::vector3df(0, 0, 0));
     cm= mSceneMgr->getSceneCollisionManager();


    createScene();

    while (mDevice->run() && mRunning) {
        mDriver->beginScene(true, true, video::SColor(255, 100, 101, 140));
        mSceneMgr->drawAll();
        mDriver->endScene();
    }
}

bool VoxelEditor::OnEvent(const SEvent &event) {
    if (event.EventType == EET_MOUSE_INPUT_EVENT) {
        switch (event.MouseInput.Event) {
        case EMIE_LMOUSE_PRESSED_DOWN:
            mLeftMousePressed = true;
            break;
        case EMIE_LMOUSE_LEFT_UP:
            mLeftMousePressed = false;
            break;
        case EMIE_RMOUSE_PRESSED_DOWN:
            mRightMousePressed = true;
            break;
        case EMIE_RMOUSE_LEFT_UP:
            mRightMousePressed = false;
            break;
        case EMIE_MOUSE_MOVED:
            if (mLeftMousePressed || mRightMousePressed) {
                core::position2di cursorPos = mDevice->getCursorControl()->getPosition();
                core::line3df ray = cm->getRayFromScreenCoordinates(cursorPos, mCamera);
                core::vector3df intersection;
                core::triangle3df hitTriangle;
                scene::ISceneNode *selectedNode = cm->getSceneNodeAndCollisionPointFromRay(ray, intersection, hitTriangle);

                if (selectedNode) {
                    if (mRightMousePressed) {
                        // Remove voxel on right-click
                        removeVoxel(selectedNode);
                    } else if (mLeftMousePressed) {
                        // Place voxel on left-click
                        placeVoxel(intersection);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    return false;
}

void VoxelEditor::createScene() {
    // Initialize the scene
}

void VoxelEditor::placeVoxel(const core::vector3df &position) {
    scene::IMeshSceneNode *voxel = mSceneMgr->addCubeSceneNode(1.0f, 0, -1, position);
    voxel->setMaterialFlag(video::EMF_LIGHTING, false);
    voxel->setMaterialTexture(0, mDriver->getTexture(mCurrentTexture.c_str()));
    VoxelNode* voxelNode = new VoxelNode(voxel, 0, 1.0f);
    mVoxelMap[voxel] = voxelNode;
}

void VoxelEditor::removeVoxel(scene::ISceneNode *node) {
    auto it = mVoxelMap.find(node);
    if (it != mVoxelMap.end()) {
        node->remove();
        //delete it->second; // Ensure to delete the VoxelNode pointer
        mVoxelMap.erase(it);
    }
}

void VoxelEditor::scaleVoxel(VoxelNode* voxelNode, float scale) {
    if (voxelNode) {
        voxelNode->getNode()->setScale(core::vector3df(scale, scale, scale));
        voxelNode->setLastSize(scale);
    }
}

void VoxelEditor::selectTexture() {
    std::cout << "Enter the path to the texture file: ";
    std::cin >> mCurrentTexture;
}

int main() {
    VoxelEditor editor;
    try {
        editor.run();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

