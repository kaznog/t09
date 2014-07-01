//
//  CCLobbyViewDataSource.h
//  ccs2dx_PhotonAirHockey
//
//  Created by noguchika on 2014/02/10.
//
//

#ifndef __ccs2dx_PhotonAirHockey__CCLobbyViewDataSource__
#define __ccs2dx_PhotonAirHockey__CCLobbyViewDataSource__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosNetworkLogic.h"
#include "CCLobbyView.h"
#include "CCRoomTableCell.h"

class CCLobbyViewDataSource : public cocos2d::extension::CCTableViewDataSource, public cocos2d::CCObject
{
    CCLobbyView* _lobbyView;
public:
    CCLobbyViewDataSource( CCLobbyView* lobbyView );
    
    /**
     * cell height for a given table.
     *
     * @param table table to hold the instances of Class
     * @return cell size
     */
    virtual cocos2d::CCSize cellSizeForTable(cocos2d::extension::CCTableView *table);
    /**
     * a cell instance at a given index
     *
     * @param idx index to search for a cell
     * @return cell found at idx
     */
    virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
    /**
     * Returns number of cells in a given table view.
     *
     * @return number of cells
     */
    virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);
};

#endif /* defined(__ccs2dx_PhotonAirHockey__CCLobbyViewDataSource__) */
