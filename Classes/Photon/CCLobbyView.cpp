//
//  CCLobbyView.cpp
//  ccs2dx_PhotonAirHockey
//
//  Created by noguchika on 2014/02/10.
//
//

#include "CCLobbyView.h"
#include "CCLobbyViewDataSource.h"
#include "PhotonKeys.h"
#include "CCConfirmMessage.h"
#include "GameLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ExitGames;
using namespace Common;
using namespace LoadBalancing;

CCScene* CCLobbyView::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    // 'layer' is an autorelease object
    CCLobbyView *layer = CCLobbyView::create(winSize);
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}


CCLobbyView::CCLobbyView()
: _lobbyViewDataSource(NULL)
, _console(NULL)
, _connectMenu(NULL)
, _joinRandomRoomMenu(NULL)
, _createRoomMenu(NULL)
, _selectedRoom(NULL)
, _isJoinRoomConfirming(false)
{
    
}

CCLobbyView::~CCLobbyView()
{
    CC_SAFE_RELEASE_NULL(this->_lobbyViewDataSource);
    CocosNetworkLogic::dispose();
    this->_console = NULL;
    this->_connectMenu = NULL;
    this->_joinRandomRoomMenu = NULL;
    this->_createRoomMenu = NULL;
    this->_selectedRoom = NULL;
    this->_isJoinRoomConfirming = false;
}

CCLobbyView* CCLobbyView::create(const cocos2d::CCSize& size)
{
    CCLobbyView* pRet = new CCLobbyView();
    if (pRet && pRet->init(size)) {
        pRet->autorelease();
    } else {
        CC_SAFE_RELEASE_NULL(pRet);
    }
    return pRet;
}

bool CCLobbyView::init( const CCSize& mySize )
{
    if( CCLayer::init() )
    {
        
        this->setContentSize( mySize );
        this->setKeypadEnabled( true );
        
        this->initBottomMenu();
        this->initTopMenu();
        this->initRoomTable();
        
        this->scheduleUpdate();
        
        return true;
    }
    
    return false;
}

void CCLobbyView::removeRoomTable()
{
    CCNode *roomTable = NULL;
    while ( (roomTable = this->getChildByTag( Child::CCTableView_roomTable)) ) {
        this->removeChild(roomTable, true);
    }
    
    CC_SAFE_RELEASE_NULL( this->_lobbyViewDataSource );
}

bool CCLobbyView::initRoomTable()
{
    const CCPoint center = this->getCenter();
    this->removeRoomTable();
    this->_lobbyViewDataSource = new CCLobbyViewDataSource(this);
    
    if (this->_lobbyViewDataSource) {
        this->_lobbyViewDataSource->autorelease();
        CC_SAFE_RETAIN(this->_lobbyViewDataSource);
        CCSize tableSize = this->_lobbyViewDataSource->cellSizeForTable( NULL );
        tableSize.height *= 10.0f;
        CCTableView* table = CCTableView::create( this->_lobbyViewDataSource, tableSize );
        if( table )
        {
            CCSize winSize = CCDirector::sharedDirector()->getWinSize();
            float posy = this->getEditName()->getContentSize().height;
            posy += this->getConsole()->getContentSize().height;
            table->setContentOffset( CCPointZero );
            table->setDirection(kCCScrollViewDirectionVertical);
            table->setPosition( ccp( center.x - ( tableSize.width * 0.5f ), winSize.height - posy - tableSize.height - FONT_SIZE(24) ) );
            table->setDelegate( this );
            table->setVerticalFillOrder(kCCTableViewFillTopDown);
            table->setTouchEnabled( true );
            this->addChild( table, 0, Child::CCTableView_roomTable );
            table->reloadData();
            
            this->setMinRoomsInView( 10 );
            return true;
        }
    }
    return false;
}

const cocos2d::CCPoint CCLobbyView::getCenter()
{
    return ccpMult( ccpFromSize(this->getContentSize()), 0.5f);
}

const cocos2d::CCPoint CCLobbyView::getTopHanger()
{
    const CCPoint& center = this->getCenter();
    return CCPointMake( center.x, center.y * 1.75f );
}

const cocos2d::CCPoint CCLobbyView::getBottomHanger()
{
    const CCPoint& center = this->getCenter();
    return CCPointMake( center.x, center.y * 0.25f );
}

bool CCLobbyView::initBottomMenu()
{
    CCDirector* director = CCDirector::sharedDirector();
    const CCSize mySize = this->getContentSize();
    const CCPoint& center = ccpMult( ccpFromSize( mySize ), 0.5f );
    
    CCLabelTTF* connectLabel = CCLabelTTF::create( "Connect", "Arial", FONT_SIZE(48) );
    CCMenuItemLabel* connectItem = CCMenuItemLabel::create( connectLabel, this, menu_selector( CCLobbyView::connectButtonCallback ) );
    
    CCLabelTTF* createLabel = CCLabelTTF::create( "Create Room", "Arial", FONT_SIZE(48) );
    CCMenuItemLabel* createItem = CCMenuItemLabel::create( createLabel, this, menu_selector( CCLobbyView::createGameButtonCallback ) );
    
    CCLabelTTF* randomLabel = CCLabelTTF::create( "Random Room", "Arial", FONT_SIZE(48) );
    CCMenuItemLabel* randomItem = CCMenuItemLabel::create( randomLabel, this, menu_selector( CCLobbyView::joinGameButtonCallback ) );
    
    pBottomMenu = CCMenu::create( connectItem, createItem, randomItem, NULL );
    if( pBottomMenu )
    {
        this->setConnectMenu( connectItem );
        this->setJoinRandomRoomMenu( randomItem );
        this->setCreateRoomMenu( createItem );
        pBottomMenu->alignItemsVerticallyWithPadding(20.0f);
        float posy = connectItem->getContentSize().height;
        posy += createItem->getContentSize().height;
        posy += randomItem->getContentSize().height;
        posy += 20.0f + 20.0f;
//        bottomMenu->alignItemsHorizontally();
        pBottomMenu->setPosition( CCPointMake( center.x, posy ) );
        this->addChild( pBottomMenu, 0, Child::CCMenu_bottomMenu );
    }
    
    return pBottomMenu;
}

bool CCLobbyView::initTopMenu()
{
    CCDirector* director = CCDirector::sharedDirector();
    const CCSize winSize = director->getWinSize();
    const CCSize mySize = this->getContentSize();
    const CCPoint center = ccpMult( ccpFromSize( mySize ), 0.5f );
    
    CCLabelTTF* consoleLabel = CCLabelTTF::create( "disconnected", "Arial", FONT_SIZE(24), CCSizeMake( winSize.width, FONT_SIZE(24) ), kCCTextAlignmentCenter );
    consoleLabel->setColor( ccc3(255, 255, 255) );
    CCMenuItemLabel* consoleItem = CCMenuItemLabel::create( consoleLabel );
    
    // bottom
    const CCSize editSize = CCSizeMake( winSize.width, FONT_SIZE(48) );
    CCMenuItem* editItem = CCMenuItem::create();
    editItem->setContentSize( editSize );
    
    CCMenu* topMenu = CCMenu::create( consoleItem, editItem, NULL );
    if( topMenu )
    {
        this->setConsole( consoleLabel );
        
        topMenu->alignItemsVertically();
        topMenu->setPosition( CCPointMake( center.x, winSize.height - (editItem->getContentSize().height + consoleItem->getContentSize().height)/2 ) );
        this->addChild( topMenu, 0, Child::CCMenu_topMenu );
        
        const CCPoint editItemPosition = editItem->getParent()->convertToWorldSpace( editItem->getPosition() );
        editItem->setContentSize( winSize );
        editItem->ignoreAnchorPointForPosition( false );
        editItem->setAnchorPoint( ccp( 0.5f, (winSize.height / (editItemPosition.y - winSize.height)) * 0.5f ) );
        
        CCScale9Sprite* editSprite = CCScale9Sprite::create("extensions/yellow_edit.png");
        CCEditBox* edit = CCEditBox::create( editSize, editSprite );
        edit->setPlaceHolder("PlayerName");
        edit->setReturnType(kKeyboardReturnTypeDone);
        edit->setFontColor(ccGRAY);
        edit->setMaxLength( 20 );
        edit->setDelegate(this);
        edit->setTouchEnabled( true );
        edit->setPosition( editItemPosition );
        CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
        JString userName = network->getUserName();
        edit->setText(userName.UTF8Representation().cstr());
        this->addChild( edit );
        this->setEditName(edit);
    }
    
    return topMenu;
}

void CCLobbyView::connectButtonCallback( cocos2d::CCObject* sender )
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    State state = network->getState();
    CCLog("connectButtonCallback");
    if( state == STATE_INITIALIZED )
    {
        CCLog("State initialized");
        network->setLastInput( INPUT_CONNECT );
    }
    else
    {
        CCLog("State not initialized");
        network->setLastInput( INPUT_EXIT );
    }
}

void CCLobbyView::createGameButtonCallback( cocos2d::CCObject* sender )
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setDidCreateGame( true );
    network->setLastInput(INPUT_CREATE_GAME);
}

void CCLobbyView::joinGameButtonCallback( cocos2d::CCObject* sender )
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    State state = network->getState();
    if ( state == STATE_JOINED )
    {
        network->setLastInput( INPUT_LEAVE_GAME );
    }
    else if( state == STATE_INITIALIZED )
    {
//        this->keyBackClicked();
        CCDirector::sharedDirector()->end();
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
#endif
    }
    else
    {
        network->setLastInput( INPUT_JOIN_RANDOM_GAME );
    }
}


void CCLobbyView::onEnter()
{
    CCLayer::onEnter();
    
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setDelegate(NULL);
    network->setDidCreateGame( false );
    CCNotificationCenter* publisher = CCNotificationCenter::sharedNotificationCenter();
    publisher->addObserver( this, callfuncO_selector( CCLobbyView::eventCreatedRoom ), PhotonNoteKey::getString(PhotonNoteKey::CreateRoomReturn), network );
    publisher->addObserver( this, callfuncO_selector( CCLobbyView::eventJoinSpecificRoom ), PhotonNoteKey::getString(PhotonNoteKey::JoinSpecificRoomReturn), network );
    publisher->addObserver( this, callfuncO_selector( CCLobbyView::eventJoinRandomRoom ), PhotonNoteKey::getString(PhotonNoteKey::JoinRandomRoomReturn), network );
}

void CCLobbyView::onExit()
{
    CCLayer::onExit();
    CCNotificationCenter* publisher = CCNotificationCenter::sharedNotificationCenter();
    publisher->removeObserver( this, PhotonNoteKey::getString(PhotonNoteKey::CreateRoomReturn) );
    publisher->removeObserver( this, PhotonNoteKey::getString(PhotonNoteKey::JoinSpecificRoomReturn) );
    publisher->removeObserver( this, PhotonNoteKey::getString(PhotonNoteKey::JoinRandomRoomReturn) );
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setDelegate(NULL);
//    network->setDidCreateGame( false );
}

void CCLobbyView::eventCreatedRoom( cocos2d::CCObject* sender )
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setLastInput(INPUT_DECLARE_READY);
    this->onJoinGame( true );
}

void CCLobbyView::eventJoinGame( cocos2d::CCObject* sender )
{
}

void CCLobbyView::eventConnected( cocos2d::CCObject* sender )
{
    JString userName = "";
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    CCEditBox *eb = this->getEditName();
    const char* ebtext = eb->getText();
    if (strlen(ebtext) != 0) {
        userName = ebtext;
        network->setUserName(userName);
    } else {
        userName = network->getUserName();
        eb->setText(userName.UTF8Representation().cstr());
    }
}

void CCLobbyView::eventDisconnected( cocos2d::CCObject* sender )
{
    
}

void CCLobbyView::eventJoinSpecificRoom( cocos2d::CCObject* sender )
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setLastInput(INPUT_DECLARE_READY);
    this->onJoinGame( false );
}

void CCLobbyView::eventJoinRandomRoom( cocos2d::CCObject* sender )
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setLastInput(INPUT_DECLARE_READY);
    this->onJoinGame( false );
}

void CCLobbyView::onJoinGame( bool isGameMaster )
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setDidCreateGame(isGameMaster);
    CCDirector* director = CCDirector::sharedDirector();
    CCScene* scene = GameLayer::scene();
    director->pushScene( scene );
}

void CCLobbyView::update( float delta )
{
    CCLayer::update( delta );
    
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    
    const CCString& output = network->getOutput();
    this->getConsole()->setString( output.getCString() );
    
    State state = network->getState();
    CCMenuItemLabel* connectButton = this->getConnectMenu();
    CCMenuItemLabel* gameButton = this->getJoinRandomRoomMenu();
    CCMenuItemLabel* createGameButton = this->getCreateRoomMenu();
    
    CCTableView* roomTable = (CCTableView*) this->getChildByTag( Child::CCTableView_roomTable );
    
    if( connectButton )
    {
        if( state == STATE_INITIALIZED )
        {
            this->removeRoomTable();
            connectButton->setString( "Connect" );
            gameButton->setVisible(true);
            gameButton->setString( "Exit Lobby" );
            createGameButton->setVisible( false );
        }
        else
        {
            if( !roomTable )
            {
                this->initRoomTable();
            }
            connectButton->setString( "Disconnect" );
            gameButton->setVisible(true);
            if( state == STATE_JOINED )
            {
                gameButton->setString( "Leave Room" );
                createGameButton->setVisible(false);
            }
            else if (network->getDidCreateGame())
            {
            }
            else
            {
                gameButton->setString( "Random Room" );
                createGameButton->setVisible(true);
            }
        }
    }
    
    roomTable = (CCTableView*) this->getChildByTag( Child::CCTableView_roomTable );
    if( roomTable )
    {
        roomTable->reloadData();
    }
}

void CCLobbyView::tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell)
{
    if (this->getIsJoinRoomConfirming()) {
        return;
    }
    using namespace ExitGames::LoadBalancing;
    CCLog("CCLobbyView::tableCellTouched() -- cell touched at index: %i", cell->getIdx());
    
    CCRoomTableCell* roomCell = (CCRoomTableCell*) cell;
    ExitGames::LoadBalancing::Room* room = roomCell->getRoom();
    
    if( roomCell->getHasRoom() && room && room->getMaxPlayers() > room->getPlayerCount() )
    {
        CCLog("CCLobbyView::tableCellTouched() HasRoom");
        CCLog("room name %s", room->getName().UTF8Representation().cstr());
        this->setSelectedRoom(room);
//        CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
//        network->opJoinRoom( room->getName() );

        std::string stdStr = room->toString().UTF8Representation().cstr();
        stdStr = "Join this room?";
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        CCSprite* background = CCSprite::create( "frame.png" );

        CCScale9Sprite* controlCancelBackground = CCScale9Sprite::create("extensions/yellow_edit.png");
        CCScale9Sprite* controlOkayBackground = CCScale9Sprite::create("extensions/yellow_edit.png");
        CCLabelTTF* label = CCLabelTTF::create( stdStr.c_str(), "Arial", FONT_SIZE(24) );
        label->setColor(ccc3(0, 0, 0));
        CCLabelTTF* cancelLabel = CCLabelTTF::create(". Cancel .", "Arial", FONT_SIZE(24));
        CCLabelTTF* okayLabel     = CCLabelTTF::create(".  Okay  .", "Arial", FONT_SIZE(24));
        cancelLabel->setColor(ccc3(0, 0, 0));
        okayLabel->setColor(ccc3(0, 0, 0));
        CCControlButton* cancel = CCControlButton::create( cancelLabel, controlCancelBackground );
        CCControlButton* okay = CCControlButton::create( okayLabel, controlOkayBackground );
        okay->addTargetWithActionForControlEvents(this, cccontrol_selector(CCLobbyView::joinRoom), CCControlEventTouchUpInside);
        
        CCFadeIn* in = CCFadeIn::create(1.0f);
        CCFadeOut* out = CCFadeOut::create(1.0f);
        
        CCConfirmMessage* message = CCConfirmMessage::create( background, label, cancel, okay, in, out );
        CCSize messageSize = message->getContentSize();
        message->setPosition( ccp(winSize.width/2 - messageSize.width/2, winSize.height/2 - messageSize.height/2) );

        pBottomMenu->setVisible(false);
        this->setIsJoinRoomConfirming(true);
        this->addChild(message, 10, Child::CCConfirmDialog);
    }
}

void CCLobbyView::tableCellHighlight(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell)
{
    CCLOG("cell tableCellHighlight at index: %i", cell->getIdx());
}

void CCLobbyView::tableCellUnhighlight(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell)
{
    CCLOG("cell tableCellUnhighlight at index: %i", cell->getIdx());
}

void CCLobbyView::tableCellWillRecycle(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell)
{
    CCLOG("cell tableCellWillRecycle at index: %i", cell->getIdx());
}

void CCLobbyView::joinRoom(cocos2d::CCObject* sender)
{
    this->setIsJoinRoomConfirming(false);
    pBottomMenu->setVisible(true);
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();

    Room* room = this->getSelectedRoom();
    if( room && room->getMaxPlayers() > room->getPlayerCount() )
    {
        CCLog("join room %s", room->getName().UTF8Representation().cstr());
        network->opJoinRoom( room->getName() );
    }
}

void CCLobbyView::cancelJoinRoom(cocos2d::CCObject* sender)
{
    this->setIsJoinRoomConfirming(false);
    pBottomMenu->setVisible(true);
}

void CCLobbyView::scrollViewDidScroll(cocos2d::extension::CCScrollView* view)
{
    
}

void CCLobbyView::scrollViewDidZoom(cocos2d::extension::CCScrollView* view)
{
    
}

void CCLobbyView::editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox)
{
    CCLog("editBox %p DidBegin !", editBox);
}


void CCLobbyView::editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox)
{
    CCLog("editBox %p DidEnd !", editBox);
}

void CCLobbyView::editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text)
{
    CCLog("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

void CCLobbyView::editBoxReturn(cocos2d::extension::CCEditBox* editBox)
{
    CCLog("editBox %p was returned !", editBox);
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    JString userName = editBox->getText();
    network->setUserName(userName);
}
