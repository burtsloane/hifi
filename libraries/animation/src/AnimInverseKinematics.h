//
//  AnimInverseKinematics.h
//
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_AnimInverseKinematics_h
#define hifi_AnimInverseKinematics_h

#include <string>

#include "AnimNode.h"

class RotationConstraint;

class RotationAccumulator {
public:
    RotationAccumulator() {}

    uint32_t size() const { return _rotations.size(); }

    void add(const glm::quat& rotation) { _rotations.push_back(rotation); }

    glm::quat getAverage() {
        glm::quat average;
        uint32_t numRotations = _rotations.size();
        if (numRotations > 0) {
            average = _rotations[0];
            for (uint32_t i = 1; i < numRotations; ++i) {
                glm::quat rotation = _rotations[i];
                if (glm::dot(average, rotation) < 0.0f) {
                    rotation = -rotation;
                }
                average += rotation;
            }
            average = glm::normalize(average);
        }
        return average;
    }

    void clear() { _rotations.clear(); }

private:
    std::vector<glm::quat> _rotations;
};

class AnimInverseKinematics : public AnimNode {
public:

    AnimInverseKinematics(const QString& id);
    virtual ~AnimInverseKinematics() override;

    void loadDefaultPoses(const AnimPoseVec& poses);
    void loadPoses(const AnimPoseVec& poses);
    void computeAbsolutePoses(AnimPoseVec& absolutePoses) const;

    void setTargetVars(const QString& jointName, const QString& positionVar, const QString& rotationVar);

    virtual const AnimPoseVec& evaluate(const AnimVariantMap& animVars, float dt, AnimNode::Triggers& triggersOut) override;
    virtual const AnimPoseVec& overlay(const AnimVariantMap& animVars, float dt, Triggers& triggersOut, const AnimPoseVec& underPoses) override;

protected:
    virtual void setSkeletonInternal(AnimSkeleton::ConstPointer skeleton);

    // for AnimDebugDraw rendering
    virtual const AnimPoseVec& getPosesInternal() const override { return _relativePoses; }

    void relaxTowardDefaults(float dt);

    RotationConstraint* getConstraint(int index);
    void clearConstraints();
    void initConstraints();

    struct IKTargetVar {
        IKTargetVar(const QString& jointNameIn, const QString& positionVarIn, const QString& rotationVarIn) :
            positionVar(positionVarIn),
            rotationVar(rotationVarIn),
            jointName(jointNameIn),
            jointIndex(-1),
            rootIndex(-1) {}

        QString positionVar;
        QString rotationVar;
        QString jointName;
        int jointIndex; // cached joint index
        int rootIndex; // cached root index
    };

    std::map<int, RotationConstraint*> _constraints;
    std::map<int, RotationAccumulator> _accumulators;
    std::vector<IKTargetVar> _targetVarVec;
    AnimPoseVec _defaultRelativePoses; // poses of the relaxed state
    AnimPoseVec _relativePoses; // current relative poses

    // no copies
    AnimInverseKinematics(const AnimInverseKinematics&) = delete;
    AnimInverseKinematics& operator=(const AnimInverseKinematics&) = delete;

    // _maxTargetIndex is tracked to help optimize the recalculation of absolute poses
    // during the the cyclic coordinate descent algorithm
    int _maxTargetIndex = 0;
};

#endif // hifi_AnimInverseKinematics_h
