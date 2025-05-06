#pragma once

#include <gr/gr_madein.h>

class grBowserCastleBackground : public grYakumono
{
    protected:
    public:
        grBowserCastleBackground(const char* taskName) : grYakumono(taskName) {
        };
        virtual ~grBowserCastleBackground() { };

        static grBowserCastleBackground* create(int mdlIndex, const char* tgtNodeName, const char* taskName);

};
