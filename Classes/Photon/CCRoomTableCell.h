//
//  CCRoomTableCell.h
//  ccs2dx_PhotonAirHockey
//
//  Created by noguchika on 2014/02/10.
//
//

#ifndef __ccs2dx_PhotonAirHockey__CCRoomTableCell__
#define __ccs2dx_PhotonAirHockey__CCRoomTableCell__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosNetworkLogic.h"
#include "CCLobbyView.h"

class CCRoomTableCell : public cocos2d::extension::CCTableViewCell
{
protected:
    CC_SYNTHESIZE_RETAIN(cocos2d::CCLabelTTF*, _label, Label);
    CC_PROPERTY(bool, _hasRoom, HasRoom);
    CC_PROPERTY(ExitGames::LoadBalancing::Room*, _roomInfo, Room);
    virtual void needsLayout();
public:
    CCRoomTableCell();
    ~CCRoomTableCell();
    
    virtual bool init();
    virtual bool init( ExitGames::LoadBalancing::Room* room );

    static CCRoomTableCell* create(CCLobbyView* lobbyView, ExitGames::LoadBalancing::Room* room);
    static CCRoomTableCell* create(CCLobbyView* lobbyView );
    
    CREATE_FUNC(CCRoomTableCell);
    
private:
    CCLobbyView* _lobbyView;
};

#endif /* defined(__ccs2dx_PhotonAirHockey__CCRoomTableCell__) */
