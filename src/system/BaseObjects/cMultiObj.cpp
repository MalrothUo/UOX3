//o--------------------------------------------------------------------------o
//| File         - cMultiObj.cpp
//| Date         - 28th July, 2000
//| Developers   - Abaddon/EviLDeD
//| Organization - UOX3 DevTeam
//| Status       - Currently under development
//o--------------------------------------------------------------------------o
//| Description  - Version History
//|
//|                1.0        Abaddon        28th July, 2000
//|                Very basically fleshed out
//|                Owner, ban list and object inside support included
//|                Note that lockdown stuff is NOT persistent yet, needs support later on
//|                Best place would be world loading, any locked down objects flagged at time added to multi
//|
//|                1.1        Abaddon        28th August, 2000
//|                Function declaration headers
//|                Addition of SetMaxLockDowns
//|
//|                1.2        Abaddon        30th November, 2000
//|                Changed lock downs to a vector, and full vector support added
//|                Save needs modification
//|
//|                1.3        giwo        25th September 2003
//|                Added CBoatObj as a derived class of CMultiObj to simplify some processes in the cBoat class
//o--------------------------------------------------------------------------o
#include "uox3.h"
#include "mapstuff.h"

namespace UOX
{

const UI16 DEFMULTI_MAXLOCKEDDOWN = 256;

const UI08 HOUSEPRIV_OWNER  = 0;
const UI08 HOUSEPRIV_BANNED = 1;

CMultiObj::CMultiObj() : CItem(), deed(""), maxLockedDown(DEFMULTI_MAXLOCKEDDOWN)
{
    objType = OT_MULTI;
    housePrivList.clear();
}

CMultiObj::~CMultiObj()
{
    housePrivList.clear();
}

//o--------------------------------------------------------------------------o
//| Function   - AddToBanList(CChar *toban)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Adds a character to the ban list, if he's not already on it
//o--------------------------------------------------------------------------o
void CMultiObj::AddToBanList(CChar *toBan)
{
    if (!IsOnBanList(toBan) && !IsOwner(toBan))
        housePrivList[toBan] = HOUSEPRIV_BANNED;
}

//o--------------------------------------------------------------------------o
//| Function   - RemoveFromBanList(CChar *toRemove)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Removes a character from a ban list
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveFromBanList(CChar *toRemove)
{
    std::map< CChar *, UI08 >::iterator rIter = housePrivList.begin();
    if (rIter != housePrivList.end())
        if (rIter->second == HOUSEPRIV_BANNED)
            housePrivList.erase(rIter);
}

//o--------------------------------------------------------------------------o
//| Function   - AddAsOwner(CChar *newOwner)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Adds a player as an owner, so long as they aren't already
//|              and aren't already on the ban list
//o--------------------------------------------------------------------------o
void CMultiObj::AddAsOwner(CChar *newOwner)
{
    if (!IsOwner(newOwner) && !IsOnBanList(newOwner))
        housePrivList[newOwner] = HOUSEPRIV_OWNER;
}

//o--------------------------------------------------------------------------o
//| Function   - RemoveAsOwner(CChar *toRemove)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Removes a character from the owner's list
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveAsOwner(CChar *toRemove)
{
    std::map< CChar *, UI08 >::iterator rIter = housePrivList.find(toRemove);
    if (rIter != housePrivList.end())
        if (rIter->second == HOUSEPRIV_OWNER)
            housePrivList.erase(rIter);
}

//o--------------------------------------------------------------------------o
//| Function   - bool IsOnBanList(CChar *toBan)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Returns true if a character is on the ban list
//o--------------------------------------------------------------------------o
bool CMultiObj::IsOnBanList(CChar *toBan) const
{
    std::map< CChar *, UI08 >::const_iterator bIter = housePrivList.find(toBan);
    if (bIter != housePrivList.end())
        if (bIter->second == HOUSEPRIV_BANNED)
            return true;
    return false;
}

//o--------------------------------------------------------------------------o
//| Function   - bool IsOwner(CChar *toFind)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Returns true if the character is an owner
//o--------------------------------------------------------------------------o
bool CMultiObj::IsOwner(CChar *toFind) const
{
    std::map< CChar *, UI08 >::const_iterator oIter = housePrivList.find(toFind);
    if (oIter != housePrivList.end())
        if (oIter->second == HOUSEPRIV_OWNER)
            return true;
    return false;
}

//o--------------------------------------------------------------------------o
//| Function   - AddToMulti(CBaseObject *toAdd)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Adds an item or character to the multi
//o--------------------------------------------------------------------------o
void CMultiObj::AddToMulti(CBaseObject *toAdd)
{
    if (toAdd->GetObjType() == OT_CHAR)
        charInMulti.Add(static_cast< CChar * >(toAdd));
    else
        itemInMulti.Add(static_cast< CItem * >(toAdd));
}

//o--------------------------------------------------------------------------o
//| Function   - RemoveFromMulti(CBaseObject *toRemove)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Removes an item or character from the multi, if possible
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveFromMulti(CBaseObject *toRemove)
{
    if (toRemove->GetObjType() == OT_CHAR)
    {
        charInMulti.Remove(static_cast< CChar * >(toRemove));
        if (CanBeObjType(OT_BOAT) && charInMulti.Num() == 0)
            (static_cast<CBoatObj *>(this))->SetMoveType(0);
    }
    else
        itemInMulti.Remove(static_cast< CItem * >(toRemove));
}

//o--------------------------------------------------------------------------o
//| Function   - UI16 GetLockDownCount(void)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Returns a count of the locked down items
//o--------------------------------------------------------------------------o
size_t CMultiObj::GetLockDownCount(void) const
{
    return lockedList.size();
}

//o--------------------------------------------------------------------------o
//| Function   - UI16 GetMaxLockDowns(void)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Returns the maximum number of possible lockdowns
//o--------------------------------------------------------------------------o
UI32 CMultiObj::GetMaxLockDowns(void) const
{
    return maxLockedDown;
}

//o--------------------------------------------------------------------------o
//| Function   - LockDownItem(CItem *toLock)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Locks down an item toLock
//o--------------------------------------------------------------------------o
void CMultiObj::LockDownItem(CItem *toLock)
{
    if (lockedList.size() < maxLockedDown)
    {
        for (ITEMLIST_CITERATOR lIter = lockedList.begin(); lIter != lockedList.end(); ++lIter)
            if ((*lIter) == toLock)
                return;
        toLock->LockDown();
        toLock->Dirty(UT_UPDATE);
        lockedList.push_back(toLock);
    }
}

//o--------------------------------------------------------------------------o
//| Function   - RemoveLockDown(CItem *toRemove)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Unlocks a locked down item
//o--------------------------------------------------------------------------o
void CMultiObj::RemoveLockDown(CItem *toRemove)
{
    for (ITEMLIST_ITERATOR rIter = lockedList.begin(); rIter != lockedList.end(); ++rIter)
        if ((*rIter) == toRemove)
        {
            toRemove->Dirty(UT_UPDATE);
            lockedList.erase(rIter);
            toRemove->SetMovable(0);
            return;
        }
}

//o--------------------------------------------------------------------------o
//| Function   - bool Save(ofstream &outStream)
//| Date       - 28th July, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Saves a multi out to disk
//|              outStream is the file to write to
//o--------------------------------------------------------------------------o
UString CMultiObj::Save(void)
{
    UString uStr;
    if (!isFree())
    {
        MapData_st& mMap = Map->GetMapData(worldNumber);
        if (GetCont() != NULL || (GetX() > 0 && GetX() < mMap.xBlock && GetY() < mMap.yBlock))
            uStr = DumpBody().str();
    }
    return uStr;
}

//o--------------------------------------------------------------------------o
//| Function   - bool LoadRemnants(UI32 arrayOffset)
//| Date       - 20th January, 2002
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - After handling data specific load, other parts go here
//o--------------------------------------------------------------------------o
bool CMultiObj::LoadRemnants(void)
{
    bool rValue = CItem::LoadRemnants();

    // Add item weight if item doesn't have it yet
    if (GetWeight() <= 0)
        SetWeight(0);
    return rValue;
}

//o--------------------------------------------------------------------------o
//| Function   - SetMaxLockDowns(UI16 newValue)
//| Date       - 28th August, 2000
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Sets the maximum number of lockdowns possible
//o--------------------------------------------------------------------------o
void CMultiObj::SetMaxLockDowns(UI16 newValue)
{
    maxLockedDown = newValue;
}

//o--------------------------------------------------------------------------o
//| Function   - bool DumpBody(ofstream &outStream)
//| Date       - Unknown
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Writes out all CMultiObj specific information to a world
//|                     file.  Also prints out the Item and BaseObject portions
//o--------------------------------------------------------------------------o
std::stringstream CMultiObj::DumpBody() const
{
    std::stringstream Str = CItem::DumpBody();
    Str << "INSERT INTO houses VALUES (";
    Str << "'" << serial << "', ";

    if (housePrivList.empty())
        Str << "NULL, NULL, ";
    else
    {
        std::stringstream priv[2];
        bool Started = false;
        for (std::map<CChar *, UI08>::const_iterator itr = housePrivList.begin(); itr != housePrivList.end(); ++itr)
            if (ValidateObject(itr->first))
                if (itr->second == HOUSEPRIV_OWNER || itr->second == HOUSEPRIV_BANNED)
                {
                    priv[itr->second == HOUSEPRIV_OWNER ? 0 : 1] << (Started ? "," : "") << itr->first->GetSerial();
                    Started = true;
                }

        for (int i = 0; i < 2; ++i)
            Str << "'" << priv[i].str() << "',";
    }

    if (lockedList.empty())
        Str << "NULL, ";
    else
    {
        Str << "'";
        for (ITEMLIST_CITERATOR itr = lockedList.begin(); itr != lockedList.end(); ++itr)
            if (ValidateObject((*itr)))
            {
                Str << (*itr)->GetSerial();
                if (itr-lockedList.begin() != lockedList.size()-1)
                    Str << ",";
            }
        Str << "', ";
    }
    
    Str << "'" << maxLockedDown << "','" << deed << "')\n";
    return Str;
}

//o--------------------------------------------------------------------------o
//| Function   - bool HandleLine(UString &UTag, UString &data)
//| Date       - Unknown
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Processes a tag/data pair if it can.  If it can, returns
//|              true.  Otherwise, returns false.
//o--------------------------------------------------------------------------o
void CMultiObj::HandleLine(std::vector<UString> dataList)
{
    CItem::HandleLine(dataList);
    for (std::vector<UString>::iterator itr = dataList.begin(); itr != dataList.end(); ++itr)
    {
        switch (itr-dataList.begin())
        {
        case 60: // house.coowner
        case 61: // house.banned
            if (!itr->empty())
            {
                int count = itr->sectionCount(",");
                if (count != 0)
                {
                    for (int i = 0; i < count+1; ++i)
                    {
                        UString uStr = itr->section(",", i, i).stripWhiteSpace();
                        if (!uStr.empty())
                        {
                            CChar* List = calcCharObjFromSer(uStr.toULong());
                            if (ValidateObject(List))
                                if (itr-dataList.begin() == 60)
                                    AddAsOwner(List);
                                else
                                    AddToBanList(List);
                        }
                    }
                }
                else
                {
                    CChar* List = calcCharObjFromSer(itr->toULong());
                    if (ValidateObject(List))
                        if (itr-dataList.begin() == 60)
                            AddAsOwner(List);
                        else
                            AddToBanList(List);
                }
            }
            break;
        case 62: // house.lockeditem
            if (!itr->empty())
            {
                int count = itr->sectionCount(",");
                if (count != 0)
                {
                    for (int i = 0; i < count+1; ++i)
                    {
                        UString uStr = itr->section(",", i, i).stripWhiteSpace();
                        if (!uStr.empty())
                        {
                            CItem* List = calcItemObjFromSer(uStr.toULong());
                            if (ValidateObject(List))
                                LockDownItem(List);
                        }
                    }
                }
                else
                {
                    CItem* iList = calcItemObjFromSer(itr->toULong());
                    if (ValidateObject(iList))
                        LockDownItem(iList);
                }
            }
            break;
        case 63: // house.maxlockeddown
            maxLockedDown = itr->empty() ? DEFMULTI_MAXLOCKEDDOWN : itr->toUShort();
            break;
        case 64: // house.deedname
            SetDeed(itr->empty() ? "" : itr->c_str());
            break;
        }
    }
}
//o--------------------------------------------------------------------------o
//| Function   - SetOwner(CChar *newOwner)
//| Date       - Unknown
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Ensures that newOwner is also on the house ownership
//|              list.
//o--------------------------------------------------------------------------o
void CMultiObj::SetOwner(CChar *newOwner)
{
    if (ValidateObject(newOwner))
    {
        owner = newOwner->GetSerial();
        AddAsOwner(newOwner);
    }
    else
        owner = INVALIDSERIAL;
}

//o--------------------------------------------------------------------------o
//| Function   - Cleanup(void)
//| Date       - 26th September, 2001
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Makes sure that any items and chars inside the multi
//|              are removed
//o--------------------------------------------------------------------------o
void CMultiObj::Cleanup(void)
{
    for (CItem *iRemove = itemInMulti.First(); !itemInMulti.Finished(); iRemove = itemInMulti.Next())
        if (ValidateObject(iRemove))
        {
            ItemTypes iType = iRemove->GetType();
            if (iType == IT_DOOR || iType == IT_LOCKEDDOOR || iType == IT_HOUSESIGN)
                iRemove->Delete();
            else
            {
                if (iRemove->IsLockedDown())
                    iRemove->SetMovable(1);
                iRemove->SetMulti(INVALIDSERIAL);
                iRemove->SetZ(GetZ());
            }
        }

    for (CChar *cRemove = charInMulti.First(); !charInMulti.Finished(); cRemove = charInMulti.Next())
        if (ValidateObject(cRemove))
        {
            cRemove->SetMulti(INVALIDSERIAL);
            cRemove->SetZ(GetZ());
        }

    CItem::Cleanup();
}

std::string CMultiObj::GetDeed(void) const
{
    return deed;
}

void CMultiObj::SetDeed(std::string newDeed)
{
    deed = newDeed;
}

//o--------------------------------------------------------------------------
//| Function   - bool CanBeObjType()
//| Date       - 24 June, 2004
//| Programmer - Maarc
//o--------------------------------------------------------------------------
//| Purpose    - Indicates whether an object can behave as a
//|              particular type
//o--------------------------------------------------------------------------
bool CMultiObj::CanBeObjType(ObjectType toCompare) const
{
    bool rvalue = CItem::CanBeObjType(toCompare);
    if (!rvalue)
    {
        if (toCompare == OT_MULTI)
            rvalue = true;
    }
    return rvalue;
}

CDataList<CChar *> * CMultiObj::GetCharsInMultiList(void)
{
    return &charInMulti;
}

CDataList<CItem *> * CMultiObj::GetItemsInMultiList(void)
{
    return &itemInMulti;
}

//o--------------------------------------------------------------------------o
//| Class        - CBoatObj
//| Date         - 25th September, 2003
//| Developers   - giwo
//| Organization - UOX3 DevTeam
//| Status       - Currently under development
//o--------------------------------------------------------------------------o
//| Description  - Version History
//|
//|                1.0        giwo        25th September, 2003
//|                Very basic, currently
//|                Simplifies some processes in cBoat class, eventually should allow
//|                easy removal of some generic containers in CItem class
//|
//|                1.1        giwo        28th October, 2003
//|                Added MoveType() to remove the need for CItem::Type2()
//|                Added MoveTime() instead of using CItem::GateTime()
//|                Changed Plank1() and Plank2() to Plank() using an array
//o--------------------------------------------------------------------------o

const SERIAL DEFBOAT_TILLER = INVALIDSERIAL;
const SERIAL DEFBOAT_HOLD = INVALIDSERIAL;
const UI08 DEFBOAT_MOVETYPE = 0;
const TIMERVAL DEFBOAT_MOVETIME = 0;

CBoatObj::CBoatObj() : CMultiObj(), tiller(DEFBOAT_TILLER), hold(DEFBOAT_HOLD), moveType(DEFBOAT_MOVETYPE),
    nextMoveTime(DEFBOAT_MOVETIME)
{
    planks[0] = planks[1] = INVALIDSERIAL;
    objType = OT_BOAT;
}

CBoatObj::~CBoatObj() {}

//o--------------------------------------------------------------------------o
//| Function   - bool DumpBody(ofstream &outStream)
//| Date       - Unknown
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Writes out all CMultiObj specific information to a world
//|              file.  Also prints out the Item and BaseObject portions
//o--------------------------------------------------------------------------o
std::stringstream CBoatObj::DumpBody() const
{
    std::stringstream Str = CMultiObj::DumpBody();
    Str << "INSERT INTO boats VALUES (";
    Str << "'" << serial << "', ";
    Str << "'" << hold << "', ";
    Str << "'" << planks[0] << "," << planks[1] << "', ";
    Str << "'" << tiller << "')\n ";
    return Str;
}

//o--------------------------------------------------------------------------o
//| Function   - bool HandleLine(UString &UTag, UString &data)
//| Date       - Unknown
//| Programmer - Abaddon
//o--------------------------------------------------------------------------o
//| Purpose    - Processes a tag/data pair if it can.  If it can, returns
//|              true.  Otherwise, returns false.
//o--------------------------------------------------------------------------o
void CBoatObj::HandleLine(std::vector<UString> dataList)
{
    CMultiObj::HandleLine(dataList);
    for (std::vector<UString>::iterator itr = dataList.begin(); itr != dataList.end(); ++itr)
    {
        switch (itr-dataList.begin())
        {
        case 65: // boats.hold
            if (!itr->empty())
                SetHold(itr->toULong());
            break;
        case 66: // boats.planks
            if (!itr->empty())
            {
                SetPlank(0, itr->section(",", 0, 0).stripWhiteSpace().toULong());
                SetPlank(1, itr->section(",", 1, 1).stripWhiteSpace().toULong());
            }
            break;
        case 67: // boats.tiller
            if (!itr->empty())
                SetTiller(itr->toULong());
            break;
        }
    }
}

//o--------------------------------------------------------------------------o
//| Function   - SERIAL Tiller()
//| Date       - September 25, 2003
//| Programmer - giwo
//o--------------------------------------------------------------------------o
//| Purpose    - Boats Tiller Item Refrence
//o--------------------------------------------------------------------------o
SERIAL CBoatObj::GetTiller(void) const
{
    return tiller;
}

void CBoatObj::SetTiller(SERIAL newVal)
{
    tiller = newVal;
}

//o--------------------------------------------------------------------------o
//| Function   - SERIAL Plank(UI08 plankNum)
//| Date       - September 25, 2003
//| Programmer - giwo
//o--------------------------------------------------------------------------o
//| Purpose    - Boats Plank Item Refrences
//o--------------------------------------------------------------------------o
SERIAL CBoatObj::GetPlank(UI08 plankNum) const
{
    SERIAL rvalue = INVALIDSERIAL;
    if (plankNum < 2)
        rvalue = planks[plankNum];
    return rvalue;
}

void CBoatObj::SetPlank(UI08 plankNum, SERIAL newVal)
{
    if (plankNum < 2)
        planks[plankNum] = newVal;
}

//o--------------------------------------------------------------------------o
//| Function   - SERIAL Hold()
//| Date       - September 25, 2003
//| Programmer - giwo
//o--------------------------------------------------------------------------o
//| Purpose    - Boats Hold Item Refrence
//o--------------------------------------------------------------------------o
SERIAL CBoatObj::GetHold(void) const
{
    return hold;
}

void CBoatObj::SetHold(SERIAL newVal)
{
    hold = newVal;
}

//o--------------------------------------------------------------------------o
//| Function   - UI08 MoveType()
//| Date       - October 28, 2003
//| Programmer - giwo
//o--------------------------------------------------------------------------o
//| Purpose    - Boats movement type (0 = Not moving, 1 = Moving forward, 2 = Moving backward)
//o--------------------------------------------------------------------------o
UI08 CBoatObj::GetMoveType(void) const
{
    return moveType;
}

void CBoatObj::SetMoveType(UI08 newVal)
{
    moveType = newVal;
}

//o--------------------------------------------------------------------------o
//| Function   - TIMERVAL MoveTime()
//| Date       - October 28, 2003
//| Programmer - giwo
//o--------------------------------------------------------------------------o
//| Purpose    - Next time a boat will move
//o--------------------------------------------------------------------------o
TIMERVAL CBoatObj::GetMoveTime(void) const
{
    return nextMoveTime;
}

void CBoatObj::SetMoveTime(TIMERVAL newVal)
{
    nextMoveTime = newVal;
}

//o--------------------------------------------------------------------------o
//| Function   - bool CanBeObjType()
//| Date       - 24 June, 2004
//| Programmer - Maarc
//o--------------------------------------------------------------------------o
//| Purpose    - Indicates whether an object can behave as a
//|              particular type
//o--------------------------------------------------------------------------o
bool CBoatObj::CanBeObjType(ObjectType toCompare) const
{
    bool rvalue = CMultiObj::CanBeObjType(toCompare);
    if (!rvalue)
        if (toCompare == OT_BOAT)
            rvalue = true;

    return rvalue;
}

}
