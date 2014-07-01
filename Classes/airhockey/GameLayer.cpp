#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "GameSprite.h"
#include "CocosNetworkLogic.h"

using namespace CocosDenshion;

GameLayer::GameLayer()
//:pPhotonNetwork(NULL)
: isJoind(false)
, isHostOwner(false)
, isMenuInitialized(false)
{
    
}

GameLayer::~GameLayer() {
	//release CCArray 
    CC_SAFE_RELEASE(_players);
}

void GameLayer::initPosition()
{
	//get screen size
	_screenSize = CCDirector::sharedDirector()->getWinSize();
	_player1->setPosition(ccp(_screenSize.width * 0.5, _player1->radius() * 2));
	_player2->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2));
    
}

void GameLayer::initMenu()
{
    if (isMenuInitialized) {
        return;
    }
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();

//    CCLabelBMFont *pCloseBMLabel = CCLabelBMFont::create("Exit Game", "bitmapFontTest3.fnt");
//    pCloseBMLabel->setScale(2.0f);
    CCLabelTTF *pCloseLabel = CCLabelTTF::create("Exit Game", "Arial", 48);
    CCMenuItemLabel *pCloseLabelItem = CCMenuItemLabel::create(pCloseLabel, this, menu_selector(GameLayer::menuCloseCallback));
    pCloseLabelItem->setPosition(ccp(visibleSize.width/4, visibleSize.height - 128));
    
//    CCLabelBMFont *pCreateBMLabel = CCLabelBMFont::create("Create Room", "bitmapFontTest3.fnt");
//    pCreateBMLabel->setScale(2.0f);
    CCLabelTTF *pCreateLabel = CCLabelTTF::create("Create Room", "Arial", 48);
    CCMenuItemLabel *pCreateLabelItem = CCMenuItemLabel::create(pCreateLabel, this, menu_selector(GameLayer::menuCreateGameCallback));
    pCreateLabelItem->setPosition(ccp(visibleSize.width/4, visibleSize.height - (128*2)));

//    CCLabelBMFont *pJoinBMLabel = CCLabelBMFont::create("Join Room", "bitmapFontTest3.fnt");
//    pJoinBMLabel->setScale(2.0f);
    CCLabelTTF *pJoinLabel = CCLabelTTF::create("Join Room", "Arial", 48);
    CCMenuItemLabel *pJoinLabelItem = CCMenuItemLabel::create(pJoinLabel, this, menu_selector(GameLayer::menuJoinGameCallback));
    pJoinLabelItem->setPosition(ccp(visibleSize.width/4, visibleSize.height - (128*3)));
    
    pMenu = CCMenu::create(pCloseLabelItem, pCreateLabelItem, pJoinLabelItem,NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 10);

    CCLabelTTF *pLeaveLabel = CCLabelTTF::create("Leave Room", "Arial", 48);
    CCMenuItemLabel *pLeaveLabelItem = CCMenuItemLabel::create(pLeaveLabel, this, menu_selector(GameLayer::menuLeaveGameCallback));
    pLeaveLabelItem->setPosition(ccp(visibleSize.width - pLeaveLabelItem->getContentSize().width, visibleSize.height - pLeaveLabelItem->getContentSize().height));
//    CCMenuItemImage *pCloseItem2 = CCMenuItemImage::create(
//                                                          "CloseNormal.png",
//                                                          "CloseSelected.png",
//                                                          this,
//                                                          menu_selector(GameLayer::menuLeaveGameCallback));
////    pCloseItem2->setScale(128/pCloseItem->getContentSize().height);
//    pCloseItem2->setPosition(ccp(visibleSize.width - pCloseItem2->getContentSize().width, pCloseItem2->getContentSize().height));
    pMenu2 = CCMenu::create(pLeaveLabelItem, NULL);
    pMenu2->setPosition(CCPointZero);
    this->addChild(pMenu2, 10);
    pMenu2->setVisible(false);
    isMenuInitialized = true;
}



void GameLayer::didUpdateState(State newState)
{
//    CCLOG("GameLayer didUpdateState %d : %d", newState, pPhotonNetwork->getPhotonState());
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
	switch(newState)
	{
		case STATE_READY:
        {
            CCLOG("GameLayer didUpdateState Photon READY");
            if (network->getDidCreateGame()) {
                
                _ball->setNextPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5 + 2 * _ball->radius()));
                
            } else {
                
                _ball->setNextPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5 - 2 * _ball->radius()));
                
            }
            initMenu();
            pMenu->setVisible(false);
            pMenu2->setVisible(true);
            isJoind = true;
			break;
        }
        case STATE_LEAVING:
            CCLOG("GameLayer didUpdateState Leaving room");
            isJoind = false;
//            pMenu2->setVisible(false);
//            pMenu->setVisible(true);
            CCDirector::sharedDirector()->popScene();
            break;
		default:
			break;
	}
    
}

void GameLayer::ReceivedClientData(cocos2d::CCPoint NextPos, cocos2d::CCPoint Vector)
{
    if (NextPos.x == 0.0f && NextPos.y == 0.0f) return;
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    float base_posx = _screenSize.width * 0.5;
    float base_posy = _player1->radius() * 2;
    CCLOG("NextPos x[%f] y[%f]", NextPos.x, NextPos.y);
    CCLOG("Vector x[%f] y[%f]", Vector.x, Vector.y);
    CCLOG("remote player base x[%f] y[%f]", base_posx, base_posy);
    float distance_x = (NextPos.x - base_posx) * -1;
    float distance_y = (NextPos.y - base_posy) * -1;
    CCLOG("remote player distance x[%f] y[%f]", distance_x, distance_y);
    
    CCPoint new_player2_next_pos = ccp(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2);
    new_player2_next_pos.x += distance_x;
    new_player2_next_pos.y += distance_y;
    CCLOG("remote player new x[%f] y[%f]", new_player2_next_pos.x, new_player2_next_pos.y);
    _player2->setNextPosition(new_player2_next_pos);
    Vector.x = Vector.x * -1;
    Vector.y = Vector.y * -1;
    _player2->setVector(Vector);
//    updatePosition();
}

void GameLayer::ReceivedGameData(
                                 cocos2d::CCPoint HostPos,
                                 cocos2d::CCPoint HostNextPos,
                                 cocos2d::CCPoint HostVector,
                                 cocos2d::CCPoint ClientPos,
                                 cocos2d::CCPoint ClientNextPos,
                                 cocos2d::CCPoint ClientVector,
                                 cocos2d::CCPoint PuckPos,
                                 cocos2d::CCPoint PuckNextPos,
                                 cocos2d::CCPoint PuckVector
)
{
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    float host_base_posx = _screenSize.width * 0.5;
    float host_base_posy = _player1->radius() * 2;
    float host_distance_x = (HostPos.x - host_base_posx) * -1;
    float host_distance_y = (HostPos.y - host_base_posy) * -1;
    float host_next_distance_x  = (HostNextPos.x - host_base_posx) * -1;
    float host_next_distance_y  = (HostNextPos.y - host_base_posy) * -1;
    CCPoint new_host_pos        = ccp(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2);
    CCPoint new_host_next_pos   = ccp(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2);
    new_host_pos.x      += host_distance_x;
    new_host_pos.y      += host_distance_y;
    new_host_next_pos.x += host_next_distance_x;
    new_host_next_pos.y += host_next_distance_y;
    HostVector.x = HostVector.x * -1;
    HostVector.y = HostVector.y * -1;
    
    _player2->setPosition(new_host_pos);
    _player2->setNextPosition(new_host_next_pos);
    _player2->setVector(HostVector);
    
//    float client_base_posx = _screenSize.width * 0.5;
//    float client_base_posy = _screenSize.height - _player1->radius() * 2;
//    float client_distance_x = (ClientPos.x - client_base_posx) * -1;
//    float client_distance_y = (ClientPos.y - client_base_posy) * -1;
//    float client_next_distance_x = (ClientNextPos.x - client_base_posx) * -1;
//    float client_next_distance_y = (ClientNextPos.y - client_base_posy) * -1;
//    CCPoint new_client_pos          = ccp(_screenSize.width * 0.5, _player1->radius() * 2);
//    CCPoint new_client_next_pos     = ccp(_screenSize.width * 0.5, _player1->radius() * 2);
//    new_client_pos.x += client_distance_x;
//    new_client_pos.y += client_distance_y;
//    new_client_next_pos.x += client_next_distance_x;
//    new_client_next_pos.y += client_next_distance_y;
//    ClientVector.x = ClientVector.x * -1;
//    ClientVector.y = ClientVector.y * -1;
//    _player1->setPosition(new_client_pos);
//    _player1->setNextPosition(new_client_next_pos);
//    _player1->setVector(ClientVector);
    
    float puck_base_posx = _screenSize.width * 0.5;
    float puck_base_posy = _screenSize.height * 0.5;
    float puck_distance_x = (PuckPos.x - puck_base_posx) * -1;
    float puck_distance_y = (PuckPos.y - puck_base_posy) * -1;
    float puck_next_distance_x = (PuckNextPos.x - puck_base_posx) * -1;
    float puck_next_distance_y = (PuckNextPos.y - puck_base_posy) * -1;
    CCPoint new_puck_pos        = ccp(puck_base_posx, puck_base_posy);
    CCPoint new_puck_next_pos   = ccp(puck_base_posx, puck_base_posy);
    new_puck_pos.x      += puck_distance_x;
    new_puck_pos.y      += puck_distance_y;
    new_puck_next_pos.x += puck_next_distance_x;
    new_puck_next_pos.y += puck_next_distance_y;
    PuckVector.x = PuckVector.x * -1;
    PuckVector.y = PuckVector.y * -1;
    _ball->setPosition(new_puck_pos);
    _ball->setNextPosition(new_puck_next_pos);
    _ball->setVector(PuckVector);
    updatePosition();
}

void GameLayer::ReceivedScore(int HostScore, int ClientScore)
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
//    if (isHostOwner) {
    if (network->getDidCreateGame()) {
        _player1Score = HostScore;
        _player2Score = ClientScore;
    } else {
        _player2Score = HostScore;
        _player1Score = ClientScore;
    }
}

bool GameLayer::isHost()
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
//    return isHostOwner;
    return network->getDidCreateGame();
}

cocos2d::CCPoint GameLayer::getPlayerPosition()
{
    return _player1->getPosition();
}

cocos2d::CCPoint GameLayer::getPlayerNextPosition()
{
    return _player1->getNextPosition();
}

cocos2d::CCPoint GameLayer::getPlayerVector()
{
    return _player1->getVector();
}

cocos2d::CCPoint GameLayer::getClientPosition()
{
    return _player2->getPosition();
}

cocos2d::CCPoint GameLayer::getClientNextPosition()
{
    return _player2->getNextPosition();
}

cocos2d::CCPoint GameLayer::getClientVector()
{
    return _player2->getVector();
}

cocos2d::CCPoint GameLayer::getPuckPosition()
{
    return _ball->getPosition();
}

cocos2d::CCPoint GameLayer::getPuckNextPosition()
{
    return _ball->getNextPosition();
}

cocos2d::CCPoint GameLayer::getPuckVector()
{
    return _ball->getVector();
}

int GameLayer::getHostScore()
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
//    if (isHostOwner) {
    if (network->getDidCreateGame()) {
        return _player1Score;
    } else {
        return _player2Score;
    }
}

int GameLayer::getClientScore()
{
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
//    if (isHostOwner) {
    if (network->getDidCreateGame()) {
        return _player2Score;
    } else {
        return _player1Score;
    }
}

CCScene* GameLayer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameLayer *layer = GameLayer::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
	//Init Game Elements
	_player1Score = 0;
    _player2Score = 0;
        
    
	//get screen size
	_screenSize = CCDirector::sharedDirector()->getWinSize();
	
    
	//1. add court image
    CCSprite * court = CCSprite::create("court.png");
    court->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(court);
    
    
	//2. add players
	_player1 =  GameSprite::gameSpriteWithFile("mallet.png");
	_player1->setPosition(ccp(_screenSize.width * 0.5, _player1->radius() * 2));
    this->addChild(_player1, 0, kPlayer1Tag);
    
    _player2 =  GameSprite::gameSpriteWithFile("mallet.png");
	_player2->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2));
    this->addChild(_player2, 0, kPlayer2Tag);
	
	_players = CCArray::create(_player1, _player2, NULL);
    _players->retain();
	

    
    
	//3. add puck
	_ball = GameSprite::gameSpriteWithFile("puck.png");
    _ball->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(_ball);
    
    
	//4. add score display
    _player1ScoreLabel = CCLabelTTF::create("0", "Arial", 60);
    _player1ScoreLabel->setPosition(ccp(_screenSize.width - 60, _screenSize.height * 0.5 - 80));
    _player1ScoreLabel->setRotation(90);
    this->addChild(_player1ScoreLabel);
	
    _player2ScoreLabel = CCLabelTTF::create("0", "Arial", 60);
    _player2ScoreLabel->setPosition(ccp(_screenSize.width - 60, _screenSize.height * 0.5 + 80));
    _player2ScoreLabel->setRotation(90);
    this->addChild(_player2ScoreLabel);

    
	//listen for touches
    this->setTouchEnabled(true);
    
//    pPhotonNetwork = new CocosNetworkLogic();
//    pPhotonNetwork->setDelegate(this);
//    pPhotonNetwork->setPlayerNextPos(_player1->getPosition());
//    pPhotonNetwork->setPlayerVector(ccp(0, 0));
    // create photon update schedule
//    this->schedule(schedule_selector(GameLayer::PhotonUpdate), 32.0f/1000.0f);
    //create main loop
    this->schedule(schedule_selector(GameLayer::update));
    return true;
}

void GameLayer::onEnter()
{
    CCLayer::onEnter();
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setDelegate(this);
}

void GameLayer::onExit()
{
    CCLayer::onExit();
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setDelegate(NULL);
}

void GameLayer::menuCloseCallback(CCObject* pSender)
{
    CCLog("CloseCallback");
//    CCDirector::sharedDirector()->popScene();
//    delete pPhotonNetwork;
//    CCDirector::sharedDirector()->end();
//    
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    exit(0);
//#endif
}

void GameLayer::menuCreateGameCallback(CCObject* pSender)
{
    CCLog("Create Game");
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
//    isHostOwner = true;
    network->setDidCreateGame(true);
//    pPhotonNetwork->setLastInput(INPUT_CREATE_GAME);
}

void GameLayer::menuJoinGameCallback(CCObject *pSender)
{
    CCLog("Join Game");
//    pPhotonNetwork->setLastInput(INPUT_JOIN_RANDOM_GAME);
}

void GameLayer::menuLeaveGameCallback(CCObject* pSender)
{
//    pPhotonNetwork->setLastInput(INPUT_LEAVE_GAME);
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
    network->setLastInput(INPUT_LEAVE_GAME);
}

//void GameLayer::PhotonUpdate (float dt) {
//    CCLOG("GameLayer PhotonUpdate");
//    pPhotonNetwork->run();
//}

void GameLayer::ccTouchesBegan(CCSet* pTouches, CCEvent* event) {
	
	CCSetIterator i;
	CCTouch* touch;
    CCPoint tap;
    GameSprite * player;

    
	if (isJoind == false) {
        return;
    }
	//loop through all beginning touches
    for( i = pTouches->begin(); i != pTouches->end(); i++) {
		touch = (CCTouch*) (*i);
		
        if(touch) {
            //get location in OpenGL view
    		tap = touch->getLocation();
 
 			//loop through players and check to see if touch is landing on one of them
//        	for (int p = 0; p < 2; p++) {
//        		player = (GameSprite *) _players->objectAtIndex(p);
//            
//                if (player->boundingBox().containsPoint(tap)) {
//                	//store player's touch 
//                	player->setTouch(touch);
//				}
//        	}
            if (_player1->boundingBox().containsPoint(tap)) {
                _player1->setTouch(touch);
            }
		}
	}
}

void GameLayer::ccTouchesMoved(CCSet* pTouches, CCEvent* event) {
    CCSetIterator i;
	CCTouch* touch;
    CCPoint tap;
    GameSprite * player;
    
	if (isJoind == false) {
        return;
    }
    //loop through all moving touches
	for( i = pTouches->begin(); i != pTouches->end(); i++) {
		touch = (CCTouch*) (*i);
		
		if(touch) {
			tap = touch->getLocation();
            if (_player1->getTouch() != NULL && _player1->getTouch() == touch) {
                CCPoint nextPosition = tap;
                //keep player inside screen
                if (nextPosition.x < _player1->radius())
                    nextPosition.x = _player1->radius();
                if (nextPosition.x > _screenSize.width - _player1->radius())
                    nextPosition.x = _screenSize.width - _player1->radius();
                if (nextPosition.y < _player1->radius())
                    nextPosition.y  = _player1->radius();
                if (nextPosition.y > _screenSize.height - _player1->radius())
                    nextPosition.y = _screenSize.height - _player1->radius();
                
                //keep player inside its court
                if (_player1->getPositionY() < _screenSize.height * 0.5f) {
                    if (nextPosition.y > _screenSize.height * 0.5 - _player1->radius()) {
                        nextPosition.y = _screenSize.height * 0.5 - _player1->radius();
                    }
                } else {
                    if (nextPosition.y < _screenSize.height * 0.5 + _player1->radius()) {
                        nextPosition.y = _screenSize.height * 0.5 + _player1->radius();
                    }
                }
                CCLOG("next position x[%f] y[%f]", nextPosition.x, nextPosition.y);
                _player1->setNextPosition(nextPosition);
                cocos2d::CCPoint newVector = ccp(tap.x - _player1->getPositionX(), tap.y - _player1->getPositionY());
                _player1->setVector(newVector);
//                pPhotonNetwork->setPlayerNextPos(nextPosition);
//                pPhotonNetwork->setPlayerVector(newVector);
//                pPhotonNetwork->sendEvent(nextPosition, newVector);
            }
//			for (int p = 0; p < _players->count(); p++) {
//        		
//        		player = (GameSprite *) _players->objectAtIndex(p);
//        		//if touch belongs to player
//        		if (player->getTouch() != NULL && player->getTouch() == touch) {
//                    CCPoint nextPosition = tap;
//                    
//                    //keep player inside screen
//                    if (nextPosition.x < player->radius())
//                    nextPosition.x = player->radius();
//                    if (nextPosition.x > _screenSize.width - player->radius())
//                    nextPosition.x = _screenSize.width - player->radius();
//                    if (nextPosition.y < player->radius())
//                    nextPosition.y  = player->radius();
//                    if (nextPosition.y > _screenSize.height - player->radius())
//                    nextPosition.y = _screenSize.height - player->radius();
//                    
//                    //keep player inside its court
//                    if (player->getPositionY() < _screenSize.height * 0.5f) {
//                        if (nextPosition.y > _screenSize.height * 0.5 - player->radius()) {
//                            nextPosition.y = _screenSize.height * 0.5 - player->radius();
//                        }
//                    } else {
//                        if (nextPosition.y < _screenSize.height * 0.5 + player->radius()) {
//                            nextPosition.y = _screenSize.height * 0.5 + player->radius();
//                        }
//                    }
//                    
//                    player->setNextPosition(nextPosition);
//                    player->setVector(ccp(tap.x - player->getPositionX(), tap.y - player->getPositionY()));
//				}	 
//        	}
		}
	}
}

void GameLayer::ccTouchesEnded(CCSet* pTouches, CCEvent* event) {

	CCSetIterator i;
	CCTouch* touch;
    GameSprite * player;
    
	if (isJoind == false) {
        return;
    }
    //loop through all ending touches
	for( i = pTouches->begin(); i != pTouches->end(); i++) {
		touch = (CCTouch*) (*i);
		
		if(touch) {
            if (_player1->getTouch() != NULL && _player1->getTouch() == touch) {
                //if touch ending belongs to this player, clear it
                _player1->setTouch(NULL);
                _player1->setVector(CCPointZero);
            }
            
//			for (int p = 0; p < _players->count(); p++) {
//        		player = (GameSprite *) _players->objectAtIndex(p);
//        		if (player->getTouch() != NULL && player->getTouch() == touch) {
//					//if touch ending belongs to this player, clear it
//					player->setTouch(NULL);
//                    player->setVector(CCPointZero);
//				}
//        	}
		}
	}
	
}

void GameLayer::updatePosition() {
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
//    if (isHostOwner) {
    if (network->getDidCreateGame()) {
        CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
        network->sendGameData(
                                     _player1->getPosition(),
                                     _player1->getNextPosition(),
                                     _player1->getVector(),
                                     _player2->getPosition(),
                                     _player2->getNextPosition(),
                                     _player2->getVector(),
                                     _ball->getPosition(),
                                     _ball->getNextPosition(),
                                     _ball->getVector()
        );
    }
    //update puck
    CCPoint ballNextPosition = _ball->getNextPosition();
    CCPoint ballVector = _ball->getVector();
    ballVector = ccpMult(ballVector, 0.98f);
    
    ballNextPosition.x += ballVector.x;
    ballNextPosition.y += ballVector.y;
    
    //test for puck and mallet collision
    float squared_radii = pow(_player1->radius() + _ball->radius(), 2);
    
    GameSprite * player;
    CCPoint playerNextPosition;
    CCPoint playerVector;
    for (int p = 0; p < _players->count(); p++) {
        
        player = (GameSprite *) _players->objectAtIndex(p);
        playerNextPosition = player->getNextPosition();
        playerVector = player->getVector();
        
        float diffx = ballNextPosition.x - player->getPositionX();
        float diffy = ballNextPosition.y - player->getPositionY();
        
        float distance1 = pow(diffx, 2) + pow(diffy, 2);
        float distance2 = pow(_ball->getPositionX() - playerNextPosition.x, 2) + pow(_ball->getPositionY() - playerNextPosition.y, 2);
        
        if (distance1 <= squared_radii || distance2 <= squared_radii) {
            
            float mag_ball = pow(ballVector.x, 2) + pow(ballVector.y, 2);
            float mag_player = pow (playerVector.x, 2) + pow (playerVector.y, 2);
            
            float force = sqrt(mag_ball + mag_player);
            float angle = atan2(diffy, diffx);
            
            ballVector.x = force * cos(angle);
            ballVector.y = (force * sin(angle));
            
            ballNextPosition.x = playerNextPosition.x + (player->radius() + _ball->radius() + force) * cos(angle);
            ballNextPosition.y = playerNextPosition.y + (player->radius() + _ball->radius() + force) * sin(angle);
            
            SimpleAudioEngine::sharedEngine()->playEffect("hit.wav");
        }
    }
    
    //check collision of ball and sides
    if (ballNextPosition.x < _ball->radius()) {
        ballNextPosition.x = _ball->radius();
        ballVector.x *= -0.8f;
        SimpleAudioEngine::sharedEngine()->playEffect("hit.wav");
    }
    
    if (ballNextPosition.x > _screenSize.width - _ball->radius()) {
        ballNextPosition.x = _screenSize.width - _ball->radius();
        ballVector.x *= -0.8f;
        SimpleAudioEngine::sharedEngine()->playEffect("hit.wav");
    }
    //ball and top of the court
    if (ballNextPosition.y > _screenSize.height - _ball->radius()) {
        if (_ball->getPosition().x < _screenSize.width * 0.5f - GOAL_WIDTH * 0.5f ||
            _ball->getPosition().x > _screenSize.width * 0.5f + GOAL_WIDTH * 0.5f) {
            ballNextPosition.y = _screenSize.height - _ball->radius();
            ballVector.y *= -0.8f;
            SimpleAudioEngine::sharedEngine()->playEffect("hit.wav");
        }
    }
    //ball and bottom of the court
    if (ballNextPosition.y < _ball->radius() ) {
        if (_ball->getPosition().x < _screenSize.width * 0.5f - GOAL_WIDTH * 0.5f ||
            _ball->getPosition().x > _screenSize.width * 0.5f + GOAL_WIDTH * 0.5f) {
            ballNextPosition.y = _ball->radius();
            ballVector.y *= -0.8f;
            SimpleAudioEngine::sharedEngine()->playEffect("hit.wav");
        }
    }
    
    //finally, after all checks, update ball's vector and next position
    _ball->setVector(ballVector);
    _ball->setNextPosition(ballNextPosition);
    
    
    //check for goals!
    if (ballNextPosition.y  < -_ball->radius() * 2) {
        this->playerScore(2);
        
    }
    if (ballNextPosition.y > _screenSize.height + _ball->radius() * 2) {
        this->playerScore(1);
    }
    
    //move pieces to next position
    _player1->setPosition(_player1->getNextPosition());
    _player2->setPosition(_player2->getNextPosition());
    _ball->setPosition(_ball->getNextPosition());
}
void GameLayer::update (float dt) {
	
	if (isJoind == false) {
        return;
    }
    CocosNetworkLogic* network = CocosNetworkLogic::getInstance();
	if (network->getDidCreateGame()) {
        updatePosition();
    }
}

void GameLayer::playerScore (int player) {
    
    SimpleAudioEngine::sharedEngine()->playEffect("score.wav");
    
    _ball->setVector(CCPointZero);

    char score_buffer[10];
    
    //if player 1 scored...
    if (player == 1) {
    
        _player1Score++;
        sprintf(score_buffer,"%i", _player1Score);
        _player1ScoreLabel->setString(score_buffer);
        //move ball to player 2 court
        _ball->setNextPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5 + 2 * _ball->radius()));
    
    //if player 2 scored...
    } else {
    
        _player2Score++;
        sprintf(score_buffer,"%i", _player2Score);
        _player2ScoreLabel->setString(score_buffer);
        //move ball to player 1 court
        _ball->setNextPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5 - 2 * _ball->radius()));
    
    }
    //move players to original position
    _player1->setPosition(ccp(_screenSize.width * 0.5, _player1->radius() * 2));
    _player2->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height - _player1->radius() * 2));
	
	//clear current touches
    _player1->setTouch(NULL);
    _player2->setTouch(NULL);
}
	