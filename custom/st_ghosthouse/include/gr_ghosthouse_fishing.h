#pragma once

#include <gr/gr_madein.h>
#include "st_ghosthouse_data.h"
#include <it/it_manager.h>
#include <ft/ft_manager.h>

class grGhostHouseFishing : public grMadein, public ftOutsideEventObserver {
protected:
    enum State {
        State_Inactive = 0x0,
        State_Vanish = 0x1,
        State_Bait = 0x2,
        State_Stalk = 0x3,
        State_Fetch = 0x4,
        State_Out = 0x5,
    };

    State state;
    Vec2f speed;
    Vec2f* southWestPos;
    Vec2f* northEastPos;
    int itemInstanceId;
    int targetEntryId;
    float timer;

public:
    grGhostHouseFishing(const char* taskName) : grMadein(taskName)
    {
        state = State_Inactive;
        addObserver(-1, -1);
        if (m_sendID < 0) {
            setupObserver(g_ftManager->m_eventManageModule.getManageId());
        }
    };
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual ~grGhostHouseFishing(){};

    virtual void setupAttack();
    virtual void setStart(Vec2f* boundarySWPos, Vec2f* boundaryNEPos);
    virtual void setVanish();
    virtual void setStalk(int entryId);
    virtual void setActive(Vec2f* boundarySWPos, Vec2f* boundaryNEPos);
    virtual void updateState(float deltaFrame);
    virtual void moveToTarget(Vec2f* targetPos, Vec2f* maxSpeed, Vec2f* accel, float deltaFrame);
    virtual void moveItem(BaseItem* item);
    virtual void rotateToDisp(Vec2f* disp, float maxRot, float rotateSpeed);
    virtual void changeState(State state);
    virtual void notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId);

    static grGhostHouseFishing* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
