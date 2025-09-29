#pragma once

#include "gr_cheesebridge_platform.h"

class grCheeseBridgeRope : public grCheeseBridgePlatform {
protected:
    soAreaData m_areaData;
    soSet<soAreaData> m_areaDataSet;
    ykAreaData m_ykData;
public:
    grCheeseBridgeRope(const char* taskName) : grCheeseBridgePlatform(taskName)
    {

    };
    virtual ~grCheeseBridgeRope(){};
    virtual void startup(gfArchive* archive, u32 unk1, u32 unk2);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual void getUpperNode(Vec3f* pos);
    virtual void getUnderNode(Vec3f* pos);

    static grCheeseBridgeRope* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
