#pragma once

#include <gr/gr_yakumono.h>

class grWorldTournamentConcrete : public grYakumono {
protected:
    soAreaData areaData;
    soSet<soAreaData> areaInit;
    ykAreaData areaInfo;
public:
    grWorldTournamentConcrete(const char* taskName) : grYakumono(taskName)
    {

    };
    virtual void startup(gfArchive* data, u32 unk1, u32 unk2);
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool unk3);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual ~grWorldTournamentConcrete(){};

    static grWorldTournamentConcrete* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
