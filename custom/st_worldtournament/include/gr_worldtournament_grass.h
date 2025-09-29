#pragma once

#include <gr/gr_yakumono.h>
#include <ft/ft_manager.h>

class grWorldTournamentGrass : public grYakumono {
protected:
    soAreaData areaData;
    soSet<soAreaData> areaInit;
    ykAreaData areaInfo;
    float collTimer;

public:
    grWorldTournamentGrass(const char* taskName) : grYakumono(taskName)
    {
        collTimer = 0.0;
    };
    virtual void startup(gfArchive* data, u32 unk1, u32 unk2);
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual ~grWorldTournamentGrass(){};
    virtual void toKnockOut(Fighter* fighter);

    static grWorldTournamentGrass* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
