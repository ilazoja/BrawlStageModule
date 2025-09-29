#pragma once

#include <types.h>
#include <gr/gr_gimmick_ladder.h>

class grKrazoaLadder : public grGimmickLadder {
    bool m_restrictUpExit;
    bool m_51;
    grGimmickMotionPathData motionPathData;
public:
    grKrazoaLadder(const char* taskName) : grGimmickLadder(taskName) {

    }
    virtual ~grKrazoaLadder() { }
    virtual void startupLadder(gfArchive* archive, u32 unk1, u32 unk2, grGimmickLadderData* ladderData);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual void setMotionPathData(int mdlIndex);

    static grKrazoaLadder* create(int mdlIndex, const char* taskName);
};
