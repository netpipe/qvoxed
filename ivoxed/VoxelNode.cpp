#include "VoxelNode.h"

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

