#pragma once

#include <gr/gr_madein.h>
#include "st_bowsercastle_data.h"

class grBowserCastleAshiba : public grYakumono
{
    protected:
    public:
        grBowserCastleAshiba(const char* taskName) : grYakumono(taskName) {
        };
        virtual void startup(gfArchive* archive, u32 unk1, u32 unk2);
        //virtual void update(float frameDiff);
        virtual ~grBowserCastleAshiba() { };

        static grBowserCastleAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
