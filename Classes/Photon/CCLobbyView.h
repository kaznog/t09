//
//  CCLobbyView.h
//  ccs2dx_PhotonAirHockey
//
//  Created by noguchika on 2014/02/10.
//
//

#ifndef __ccs2dx_PhotonAirHockey__CCLobbyView__
#define __ccs2dx_PhotonAirHockey__CCLobbyView__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosNetworkLogic.h"

class CCLobbyViewDataSource;
class CCLobbyView : public cocos2d::CCLayer, public cocos2d::extension::CCTableViewDelegate, public cocos2d::extension::CCEditBoxDelegate
{
    CCLobbyViewDataSource* _lobbyViewDataSource;
    
    CC_SYNTHESIZE( cocos2d::CCLabelTTF*, _console, Console );
    CC_SYNTHESIZE( cocos2d::extension::CCEditBox*, _editName, EditName );

    CC_SYNTHESIZE( cocos2d::CCMenu*, pBottomMenu, BottonMenu);
    CC_SYNTHESIZE( cocos2d::CCMenuItemLabel*, _connectMenu, ConnectMenu );
    CC_SYNTHESIZE( cocos2d::CCMenuItemLabel*, _joinRandomRoomMenu, JoinRandomRoomMenu );
    CC_SYNTHESIZE( cocos2d::CCMenuItemLabel*, _createRoomMenu, CreateRoomMenu );

    CC_SYNTHESIZE( unsigned int, _minRoomsInView, MinRoomsInView );

    CC_SYNTHESIZE(ExitGames::LoadBalancing::Room*, _selectedRoom, SelectedRoom);
    CC_SYNTHESIZE(bool, _isJoinRoomConfirming, IsJoinRoomConfirming);
public:
    CCLobbyView();
    ~CCLobbyView();
    
    struct Child
    {
        enum _Tag
        {
            CCMenu_topMenu = 1,
            CCTableView_roomTable,
            CCMenu_bottomMenu,
            CCConfirmDialog,
            COUNT
        } Tag;
    };

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    static CCLobbyView* create(const cocos2d::CCSize& size);
    virtual bool init( const cocos2d::CCSize& size );

    virtual bool initRoomTable();
    virtual bool initBottomMenu();
    virtual bool initTopMenu();
    virtual void removeRoomTable();
    virtual void onEnter();
    virtual void onExit();
    virtual void update( float delta );
    
    virtual void joinGameButtonCallback( cocos2d::CCObject* sender );
    virtual void connectButtonCallback( cocos2d::CCObject* sender );
    virtual void createGameButtonCallback( cocos2d::CCObject* sender );
    virtual const cocos2d::CCPoint getCenter();
    virtual const cocos2d::CCPoint getTopHanger();
    virtual const cocos2d::CCPoint getBottomHanger();
    virtual void onJoinGame( bool isGameMaster );
    virtual void eventJoinGame( cocos2d::CCObject* sender );
    virtual void eventConnected( cocos2d::CCObject* sender );
    virtual void eventDisconnected( cocos2d::CCObject* sender );
    virtual void eventCreatedRoom( cocos2d::CCObject* sender );
    virtual void eventJoinSpecificRoom( cocos2d::CCObject* sender );
    virtual void eventJoinRandomRoom( cocos2d::CCObject* sender );
    
    /**
     * Delegate to respond touch event
     *
     * @param table table contains the given cell
     * @param cell  cell that is touched
     */
    virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
    virtual void tableCellHighlight(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
    virtual void tableCellUnhighlight(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
    virtual void tableCellWillRecycle(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
    
    virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);
    virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view);
    void joinRoom(cocos2d::CCObject* sender);
    void cancelJoinRoom(cocos2d::CCObject* sender);
    
    //CC_IMPLEMENT_BACK_BUTTON_FOR_LAYER_PLEASE_ENABLE_KEYPAD;
    inline virtual void keyBackClicked()
    {
        using namespace cocos2d;
        CCDirector* director = CCDirector::sharedDirector();
        if( director )
        {
            director->popScene();
        }
    }
    /**
     * This method is called when an edit box gains focus after keyboard is shown.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox);
    
    
    /**
     * This method is called when an edit box loses focus after keyboard is hidden.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox);
    
    /**
     * This method is called when the edit box text was changed.
     * @param editBox The edit box object that generated the event.
     * @param text The new text.
     */
    virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);
    
    /**
     * This method is called when the return button was pressed or the outside area of keyboard was touched.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
};

#endif /* defined(__ccs2dx_PhotonAirHockey__CCLobbyView__) */
