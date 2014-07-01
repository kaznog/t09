//
//  CCRoomTableCell.cpp
//  ccs2dx_PhotonAirHockey
//
//  Created by noguchika on 2014/02/10.
//
//

#include "CCRoomTableCell.h"

USING_NS_CC;
using namespace ExitGames;
using namespace LoadBalancing;
using namespace Common;

const float kMyTableViewCellHeight = 128.f;

CCRoomTableCell::CCRoomTableCell()
: _label(NULL)
, _roomInfo(NULL)
{
    
}

CCRoomTableCell::~CCRoomTableCell()
{
    this->removeChild(_label);
    CC_SAFE_RELEASE_NULL(_label);
}

bool CCRoomTableCell::init()
{
    this->_roomInfo = NULL;
    this->setHasRoom(false);
    
    CCLabelTTF *pLabel = CCLabelTTF::create("No Room", f_AmericanTypewriter, FONT_SIZE(kCCRoomTableCellFontSize));
    pLabel->setAnchorPoint(CCPointZero);
    this->addChild(pLabel);
    this->setLabel(pLabel);
    return this->init(NULL);
}

bool CCRoomTableCell::init(ExitGames::LoadBalancing::Room* room)
{
    if (!CCTableViewCell::init()) {
        return false;
    }
    this->setHasRoom((room == NULL) ? false : true);
    this->setRoom(room);
    needsLayout();
    return true;
}

CCRoomTableCell* CCRoomTableCell::create(CCLobbyView* lobbyView, ExitGames::LoadBalancing::Room* room)
{
    CCRoomTableCell *pRet = new CCRoomTableCell();
    if (pRet) {
        pRet->_lobbyView = lobbyView;
        if (pRet->init(room)) {
            pRet->autorelease();
        } else {
            CC_SAFE_RELEASE_NULL(pRet);
        }
    } else {
        CC_SAFE_RELEASE_NULL(pRet);
    }
    return pRet;
}

CCRoomTableCell* CCRoomTableCell::create(CCLobbyView* lobbyView)
{
    CCRoomTableCell *pRet = new CCRoomTableCell();
    if (pRet) {
        pRet->_lobbyView = lobbyView;
        if (pRet->init( NULL )) {
            pRet->autorelease();
        } else {
            CC_SAFE_RELEASE_NULL(pRet);
        }
    } else {
        CC_SAFE_RELEASE_NULL(pRet);
    }
    return pRet;
}

bool CCRoomTableCell::getHasRoom()
{
    return this->_hasRoom;
}

void CCRoomTableCell::setHasRoom(bool hasRoom)
{
    _hasRoom = hasRoom;
}

ExitGames::LoadBalancing::Room* CCRoomTableCell::getRoom()
{
    return _roomInfo;
}

void CCRoomTableCell::setRoom(ExitGames::LoadBalancing::Room *room)
{
	using namespace std;
    
    std::string stdStr = "";
    this->_roomInfo = room;
    if (room == NULL) {
        stdStr = "No Room";
    } else {
        stdStr = room->toString().UTF8Representation().cstr();
        std::ostringstream capacity;
        capacity << this->_roomInfo->getMaxPlayers();
        std::string strCap = capacity.str();
        stdStr = std::string("Room ") + stdStr;
    }
    CCLabelTTF* pLabel = this->getLabel();
    if (pLabel) {
        pLabel->setString(stdStr.c_str());
    } else {
        pLabel = CCLabelTTF::create(stdStr.c_str(), f_AmericanTypewriter, FONT_SIZE(kCCRoomTableCellFontSize));
        pLabel->setAnchorPoint(CCPointZero);
        this->addChild(pLabel);
        this->setLabel(pLabel);
    }
}

void CCRoomTableCell::needsLayout()
{
    float x = 0.0f, y = 0.0f;
    CCSize size = CCSizeMake(_lobbyView->getContentSize().width * 0.9f, FONT_SIZE(kCCRoomTableCellFontSize));
    CCSize labelSize = _label->getContentSize();
    _label->setAnchorPoint(CCPointZero);
    y = (size.height - labelSize.height)/2;
    _label->setPosition(ccp(x, y));
}
