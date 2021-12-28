#include "plugin.h"

#include <vector>

using namespace plugin;

std::vector<short> snipers;
unsigned int currentModel = -1;


unsigned int jmp73AB51 = 0x73AB51;
unsigned int jmp73AC48 = 0x73AC48;

void __declspec(naked) changeType()
{
    __asm {
        mov ecx, currentModel
        push 1
        push ecx
        jmp jmp73AB51
    }
}

void __declspec(naked) changeType2()
{
    __asm {
        mov ecx, [ebp]
        push currentModel
        push esi
        jmp jmp73AC48
    }
}

char __fastcall CWeaponFireHooked(CWeapon* weapon, void*, CPed* owner, CVector* vecOrigin, CVector* vecEffectPosn, CEntity* targetEntity, CVector* vecTarget, CVector* arg_14)
{
    if (weapon == NULL)
        return 0;

    BYTE original73AB4B[5] = {0x8B, 0x4D, 0x00, 0x6A, 0x01};
    BYTE original73AC43[5] = {0x8B, 0x4D, 0x00, 0x51, 0x56};

    int weaponType = weapon->m_nType;
    currentModel = weaponType;
    if (std::find(snipers.begin(), snipers.end(), weaponType) != snipers.end())
    {
        *((BYTE*)0x742476) = 0xEB; //Forces gun to fire as sniper
        *((BYTE*)0x742477) = 0x2E; //Forces gun to fire as sniper

        injector::MakeJMP(0x73AB4B, changeType);  //Get actual sniper stats for damage etc.
        injector::MakeJMP(0x73AC43, changeType2); //Get actual sniper stats for damage etc.

        weapon->m_nType = WEAPON_PISTOL_SILENCED;
        char retVal = weapon->Fire(owner, vecOrigin, vecEffectPosn, targetEntity, vecTarget, arg_14);
        weapon->m_nType = (eWeaponType)weaponType;

        //restore original program functionality
        *((BYTE*)0x742476) = 0xFF;
        *((BYTE*)0x742477) = 0x24;

        memcpy((BYTE*)0x73AB4B, original73AB4B, 5);
        memcpy((BYTE*)0x73AC43, original73AC43, 5);


        return retVal;
    }

    return weapon->Fire(owner, vecOrigin, vecEffectPosn, targetEntity, vecTarget, arg_14);
}

class SilencedSnipers {
public:
    SilencedSnipers() {

        std::ifstream file("SilencedSnipers.cfg");
        std::string str;
        while (std::getline(file, str))
            snipers.push_back(atoi(str.c_str()));        
  

        patch::RedirectCall(0x61ECCD, CWeaponFireHooked);
        patch::RedirectCall(0x68626D, CWeaponFireHooked);
        patch::RedirectCall(0x686283, CWeaponFireHooked);
        patch::RedirectCall(0x686787, CWeaponFireHooked);


        //CMessages::AddMessageJumpQ((char*)"Key Pressed", 100, 0, false);
    }
} silencedSnipers;
