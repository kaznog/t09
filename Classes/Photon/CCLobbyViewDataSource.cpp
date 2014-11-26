//
//  CCLobbyViewDataSource.cpp
//  ccs2dx_PhotonAirHockey
//
//  Created by noguchika on 2014/02/10.
//
//

#include "CCLobbyViewDataSource.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ExitGames;
using namespace Common;
using namespace LoadBalancing;

CCLobbyViewDataSource::CCLobbyViewDataSource( CCLobbyView* lobbyView )
{
    _lobbyView = lobbyView;
}

CCSize CCLobbyViewDataSource::cellSizeForTable(CCTableView *table)
{
    CCSize cellSize = CCSizeMake( _lobbyView->getContentSize().width * 0.9f, FONT_SIZE(kCCRoomTableCellFontSize));
    CCLog("Cell Size %f, %f", cellSize.width, cellSize.height);
    return cellSize;
}

CCTableViewCell* CCLobbyViewDataSource::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCLOG("ontableCellAtIndex %d", idx);
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    int roomSize = network->getRoomNameList().getSize();
    bool realRoom = idx < roomSize;
    CCLOG("ontableCellAtIndex idx: %d", idx);
    CCLog("ontableCellAtIndex RoomList size: %d", roomSize);
    CCLog("ontableCellAtIndex GamesRuning %d", network->mLoadBalancingClient.getCountGamesRunning());
    CCRoomTableCell *cell = (CCRoomTableCell*) table->dequeueCell();
    const JVector<Room*>& roomList = network->getRoomList();
    if (cell == NULL) {
        if (realRoom) {
            Room* room = roomList.getElementAt( idx );
            cell = CCRoomTableCell::create(_lobbyView, room);
        }
        else
        {
            cell = CCRoomTableCell::create(_lobbyView);
        }
    }
    else
    {
        if (realRoom) {
            Room* room = roomList.getElementAt( idx );
            cell->init( room );
        }
        else
        {
            cell->init( NULL );
        }
    }    
    return cell;
}

unsigned int CCLobbyViewDataSource::numberOfCellsInTableView(CCTableView *table)
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    State state = network->getState();
    CCLOG("CCLobbyViewDataSource:numberOfCellsInTableView state[%d]", state);
    if( state == STATE_DISCONNECTED || state == STATE_INITIALIZED)
    {
        return 0;
    }
    else
    {
        return MAX( _lobbyView->getMinRoomsInView(), network->getRoomList().getSize() );
    }
}
