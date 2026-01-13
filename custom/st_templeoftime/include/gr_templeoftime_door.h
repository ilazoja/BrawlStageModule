#pragma once

#include "gr_templeoftime.h"
#include <memory.h>

class grTempleOfTimeDoor : public grTempleOfTime {

protected:
    enum State {
        State_Closed = 0x0,
        State_Open = 0x1,
        State_Opened = 0x2,
    };

    bool openedWithOcarina;
    State state;
    OcarinaSong<6> songOfTime;

public:
    grTempleOfTimeDoor(const char* taskName) : grTempleOfTime(taskName)
    {
        openedWithOcarina = false;
        state = State_Closed;
        songOfTime.buttons[0].m_appealSR = true;
        songOfTime.buttons[1].m_attack = true;
        songOfTime.buttons[2].m_appealLw = true;
        songOfTime.buttons[3].m_appealSR = true;
        songOfTime.buttons[4].m_attack = true;
        songOfTime.buttons[5].m_appealLw = true;
        ocarinaSong = &songOfTime;
    };
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual ~grTempleOfTimeDoor(){};

    virtual void playOcarinaSong();

    static grTempleOfTimeDoor* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
