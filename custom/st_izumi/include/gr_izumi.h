#pragma once

#include <gr/gr_yakumono.h>

class grIzumi : public grYakumono {
protected:
    snd3DGenerator centerSoundGenerator;
    snd3DGenerator ringSoundGenerators[2];
    int ringSndPtr[2];

public:
    grIzumi(const char* taskName) : grYakumono(taskName)
    {
        ringSndPtr[0] = -1;
        ringSndPtr[1] = -1;
    };
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual ~grIzumi(){};

    static grIzumi* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
