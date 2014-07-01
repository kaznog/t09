//
//  CocosNetworkLogic.cpp
//  AirHockey
//
//  Created by noguchika on 2014/02/04.
//
//

#include "CocosNetworkLogic.h"
#include "PhotonKeys.h"

USING_NS_CC;
using namespace ExitGames;
using namespace Common;
using namespace LoadBalancing;

CocosNetworkLogic* CocosNetworkLogic::_instance = NULL;

CocosNetworkLogic* CocosNetworkLogic::getInstance()
{
    if (_instance == NULL) {
        _instance = new CocosNetworkLogic();
        _instance->autorelease();
        _instance->retain();
        CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(_instance, 0, false);
    }
    return _instance;
}

void CocosNetworkLogic::dispose()
{
    CC_SAFE_DELETE(_instance);
    _instance = NULL;
}

CocosNetworkLogic::CocosNetworkLogic()
: NetworkLogic(this, L"1.0")
, cocosNetworkDelegate(NULL)
, m_PhotonState(STATE_INITIALIZED)
, m_PlayerNextPos(ccp(0, 0))
, m_PlayerVector(ccp(0, 0))
, m_remotePlayerNextPos(ccp(0, 0))
, m_remotePlayerVector(ccp(0, 0))
{
    registerForStateUpdates(this);
    _output = "";
}

CocosNetworkLogic::~CocosNetworkLogic()
{
    _instance->disconnect();
    if ( _instance == this ) {
        _instance = NULL;
    }
}

void CocosNetworkLogic::setDelegate(CocosNetworkDelegate *pDelegate)
{
    cocosNetworkDelegate = pDelegate;
}

void CocosNetworkLogic::write(const ExitGames::Common::JString& str)
{
    std::string cStr = str.UTF8Representation().cstr();
    this->_output = cStr.c_str();
    CCLOG("%s", cStr.c_str());
}

void CocosNetworkLogic::writeLine(const ExitGames::Common::JString& str)
{
    std::string cStr = str.UTF8Representation().cstr();
    this->_output = cStr.c_str();
    CCLOG("%s", cStr.c_str());
}

void CocosNetworkLogic::stateUpdate(State newState)
{
    CCLOG("CocosNetworkLogic stateUpdate");
    this->setPhotonState(newState);
    if (cocosNetworkDelegate) {
        CCLOG("CocosNetworkLogic stateUpdate new State %d", newState);
        cocosNetworkDelegate->didUpdateState(newState);
    }
}

CCString CocosNetworkLogic::getStateString(State state)
{
	switch(state)
	{
		case STATE_INITIALIZED:
			return "disconnected";
        case STATE_WAITING:
            return "connecting";
		case STATE_CONNECTING:
			return "connecting";
		case STATE_CONNECTED:
			return "connected";
		case STATE_JOINING:
			return "joining";
		case STATE_JOINED:
			return "joined";
		case STATE_LEAVING:
			return "leaving";
		case STATE_LEFT:
			return "left";
		case STATE_DISCONNECTING:
			return "disconnecting";
		case STATE_DISCONNECTED:
			return "disconnected";
		default:
			return "unknown state";
	}
}

JString CocosNetworkLogic::getUserName()
{
    JString playerName = mLoadBalancingClient.getLocalPlayer().getName();
    if (playerName == JString(L"")) {
        const char* UserDefaultName = CCUserDefault::sharedUserDefault()->getStringForKey( "Name" ).c_str();
        if (strlen(UserDefaultName) == 0) {
            State currentState = this->getState();
            if (currentState < STATE_CONNECTED) {
                playerName = JString(L"PLayerName");
            } else {
                playerName = JString(L"PLayer") + this->mLoadBalancingClient.getCountPlayersOnline() + L"";
            }
        } else {
            playerName = JString(L"") + UserDefaultName;
        }
        this->setUserName(playerName);
    }
    return playerName;
}

void CocosNetworkLogic::setUserName(ExitGames::Common::JString& str)
{
    mLoadBalancingClient.getLocalPlayer().setName(str);
    CCUserDefault* pUserDefault = CCUserDefault::sharedUserDefault();
    pUserDefault->setStringForKey("Name", str.UTF8Representation().cstr());
    pUserDefault->flush();
}

void CocosNetworkLogic::update(float dt)
{
    this->run();
}

void CocosNetworkLogic::notify( int note )
{
    const char* notifiString = PhotonNoteKey::getString( (PhotonNoteKey::NoteInt) note );
    CCLog("CocosNetworkLogic notify %s", notifiString);
    CCNotificationCenter* publisher = CCNotificationCenter::sharedNotificationCenter();
    publisher->postNotification( notifiString );
}

int CocosNetworkLogic::getRoomPropertyInt( const char* key )
{
    MutableRoom& room = this->getCurrentRoom();
    Hashtable properties = room.getCustomProperties();
    
    if( properties.contains<JString>(key) )
    {
        JString jStr = properties.getValue<JString>(key)->toString();
        
        if( jStr.length() )
        {
            return jStr.toInt();
        }
    }
    return -1;
}

int CocosNetworkLogic::getRoomPropertyInt( int key )
{
    CCAssert( key >= 0 && key < PhotonHashKey::HASH_KEY_COUNT, "CCNetworkLogic::getRoomPropertyInt() -- Invalid hash key!" );
    MutableRoom& room = this->getCurrentRoom();
    Hashtable properties = room.getCustomProperties();
    
    if( properties.contains<int>(key) )
    {
        JString jStr = properties.getValue<int>(key)->toString();
        
        if( jStr.length() )
        {
            return jStr.toInt();
        }
    }
    return this->getRoomPropertyInt( PhotonHashKey::getString( (PhotonHashKey::HashInt) key ) );
}

cocos2d::CCString CocosNetworkLogic::getRoomPropertyString( const char* key )
{
    MutableRoom& room = this->getCurrentRoom();
    Hashtable properties = room.getCustomProperties();
    const Object* value = properties.getValue<JString>(key);
    
    if( value )
    {
        return value->toString().ANSIRepresentation().cstr();
    }
    return "";
}

cocos2d::CCString CocosNetworkLogic::getRoomPropertyString( int key )
{
    MutableRoom& room = this->getCurrentRoom();
    Hashtable properties = room.getCustomProperties();
    const Object* value = properties.getValue<int>(key);
    
    if( value )
    {
        return value->toString().ANSIRepresentation().cstr();
    }
    return "";
}

void CocosNetworkLogic::run()
{
	if(mLastInput == INPUT_EXIT && mStateAccessor.getState() != STATE_DISCONNECTING && mStateAccessor.getState() != STATE_DISCONNECTED)
	{
		disconnect();
		mStateAccessor.setState(STATE_DISCONNECTING);
		mpOutputListener->writeLine(L"terminating application");
	}
	else if( mLastInput == INPUT_CONNECT && mStateAccessor.getState() == STATE_INITIALIZED )
    {
        mStateAccessor.setState( STATE_CONNECTING );
    }
    else if( mLastInput == INPUT_DECLARE_READY && mStateAccessor.getState() == STATE_JOINED )
    {
        Hashtable* hash = new Hashtable();
        hash->put<int,int>( (int) PhotonHashKey::int_PeerId, this->getPeerId() );
        opRaiseEvent( true, *hash, PHOTON_SENDDATATYPE_READY);
        mStateAccessor.setState( STATE_READY );
    }
	else
	{
		State state = mStateAccessor.getState();
		switch(state)
		{
/*
            case STATE_INITIALIZED:
                connect();
                mStateAccessor.setState(STATE_CONNECTING);
                mpOutputListener->writeLine(L"initialize to connecting");
                break;
            case STATE_CONNECTING:
                mpOutputListener->writeLine(L"connecting");
                break; // wait for callback
 */
            case STATE_CONNECTING:
                connect();
                break;
            case STATE_WAITING:
                break; // wait for callback

            case STATE_CONNECTED:
                mpOutputListener->writeLine(L"connected");
                switch(mLastInput)
                {
                    case INPUT_CREATE_GAME: // create Game
                        mpOutputListener->writeLine(L"create game");
                        opCreateRoom();
                        break;
                    case INPUT_JOIN_RANDOM_GAME: // join Game
                        opJoinRandomRoom();
                        mStateAccessor.setState(STATE_JOINING);
                        mpOutputListener->writeLine(L"joining random game");
                        break;
                    default: // no or illegal input -> stay waiting for legal input
                        break;
                }
                break;
            case STATE_JOINING:
                mpOutputListener->writeLine(L"joining");
                break; // wait for callback
            case STATE_JOINED:
                mpOutputListener->writeLine(L"joined");
//                sendEvent(this->getPlayerNextPos(), this->getPlayerVector());
                break;
            case STATE_READY:
                {
                    unsigned char currentlyJoinedRoomPlayer = this->getCurrentlyJoinedRoom().getPlayerCount();
                    mpOutputListener->writeLine(JString(L"currently Joined room player: ") + currentlyJoinedRoomPlayer);
                    if (currentlyJoinedRoomPlayer == 2) {
                        mStateAccessor.setState(STATE_IN_GAME);
                    }
                    else
                    {
                        switch(mLastInput)
                        {
                            case INPUT_LEAVE_GAME: // leave Game
                                mLoadBalancingClient.opLeaveRoom();
                                mStateAccessor.setState(STATE_LEAVING);
                                mpOutputListener->writeLine(L"");
                                mpOutputListener->writeLine(L"leaving game room");
                                break;
                            default: // no or illegal input -> stay waiting for legal input
                                break;
                        }
                    }
                }
                break;
            case STATE_IN_GAME:
                {
                    unsigned char currentlyJoinedRoomPlayer = this->getCurrentlyJoinedRoom().getPlayerCount();
                    mpOutputListener->writeLine(JString(L"currently Joined room player: ") + currentlyJoinedRoomPlayer);
                    if (currentlyJoinedRoomPlayer == 1) {
                        mLoadBalancingClient.opLeaveRoom();
                        mStateAccessor.setState(STATE_LEAVING);
                        mpOutputListener->writeLine(L"");
                        mpOutputListener->writeLine(L"leaving game room");
                    }
                    else
                    {
                        sendEvent();
                        switch(mLastInput)
                        {
                            case INPUT_LEAVE_GAME: // leave Game
                                mLoadBalancingClient.opLeaveRoom();
                                mStateAccessor.setState(STATE_LEAVING);
                                mpOutputListener->writeLine(L"");
                                mpOutputListener->writeLine(L"leaving game room");
                                break;
                            default: // no or illegal input -> stay waiting for legal input
                                break;
                        }
                    }
                }
                break;
            case STATE_LEAVING:
                opLeaveRoom();
                mpOutputListener->writeLine(L"leaving");
                break; // wait for callback
            case STATE_LEFT:
                mpOutputListener->writeLine(L"left");
                mStateAccessor.setState(STATE_CONNECTED);
                break;
            case STATE_DISCONNECTING:
                mpOutputListener->writeLine(L"disconnecting");
                break; // wait for callback
            case STATE_DISCONNECTED:
                mStateAccessor.setState(STATE_INITIALIZED);
                break;
            default:
//                mpOutputListener->writeLine(L"CocosNetworking run state other");
                break;
		}
	}
	mLastInput = INPUT_NON;
	mLoadBalancingClient.service();
}

void CocosNetworkLogic::sendEvent()
{
    State state = mStateAccessor.getState();
    if (state != STATE_IN_GAME) {
        mLoadBalancingClient.opRaiseEvent(false, ExitGames::Common::Hashtable(), 100);
        mpOutputListener->write(L"s");
    }
    else
    {
        if (this->getDidCreateGame()) {
            sendScore(cocosNetworkDelegate->getHostScore(), cocosNetworkDelegate->getClientScore());
        } else {
            sendEvent(cocosNetworkDelegate->getPlayerNextPosition(), cocosNetworkDelegate->getPlayerVector());
        }
    }
}

void CocosNetworkLogic::sendScore(int HostScore, int ClientScore)
{
    ExitGames::Common::Hashtable *data = new ExitGames::Common::Hashtable;
    data->put<int, int>(0, HostScore);
    data->put<int, int>(1, ClientScore);
    const ExitGames::Common::Hashtable temp = (const ExitGames::Common::Hashtable)*data;
	mLoadBalancingClient.opRaiseEvent(false, temp, PHOTON_SENDDATATYPE_SCORE);
}

void CocosNetworkLogic::sendEvent(cocos2d::CCPoint pos, cocos2d::CCPoint vector)
{
    ExitGames::Common::Hashtable *data = new ExitGames::Common::Hashtable;
    data->put<int, float>(0, pos.x);
    data->put<int, float>(1, pos.y);
    data->put<int, float>(2, vector.y);
    data->put<int, float>(3, vector.y);
    const ExitGames::Common::Hashtable temp = (const ExitGames::Common::Hashtable)*data;
	mLoadBalancingClient.opRaiseEvent(false, temp, PHOTON_SENDDATATYPE_CLIENT);
}

void CocosNetworkLogic::sendGameData(cocos2d::CCPoint HostPos, cocos2d::CCPoint HostNextPos, cocos2d::CCPoint HostVector, cocos2d::CCPoint ClientPos, cocos2d::CCPoint ClientNextPos, cocos2d::CCPoint ClientVector, cocos2d::CCPoint PuckPos, cocos2d::CCPoint PuckNextPos, cocos2d::CCPoint PuckVector)
{
    ExitGames::Common::Hashtable *data = new ExitGames::Common::Hashtable;
    data->put<int, float>(0, HostPos.x);
    data->put<int, float>(1, HostPos.y);
    data->put<int, float>(2, HostNextPos.x);
    data->put<int, float>(3, HostNextPos.y);
    data->put<int, float>(4, HostVector.x);
    data->put<int, float>(5, HostVector.y);
    data->put<int, float>(6, ClientPos.x);
    data->put<int, float>(7, ClientPos.y);
    data->put<int, float>(8, ClientNextPos.x);
    data->put<int, float>(9, ClientNextPos.y);
    data->put<int, float>(10,ClientVector.x);
    data->put<int, float>(11,ClientVector.y);
    data->put<int, float>(12, PuckPos.x);
    data->put<int, float>(13, PuckPos.y);
    data->put<int, float>(14, PuckNextPos.x);
    data->put<int, float>(15, PuckNextPos.y);
    data->put<int, float>(16, PuckVector.x);
    data->put<int, float>(17, PuckVector.y);
    const ExitGames::Common::Hashtable temp = (const ExitGames::Common::Hashtable)*data;
    mLoadBalancingClient.opRaiseEvent(false, temp, PHOTON_SENDDATATYPE_GAMEDATA);
}

void CocosNetworkLogic::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	// you do not receive your own events, unless you specify yourself as one of the receivers explicitly, so you must start 2 clients, to receive the events, which you have sent, as sendEvent() uses the default receivers of opRaiseEvent() (all players in same room like the sender, except the sender itself)
	EGLOG(ExitGames::Common::DebugLevel::ALL, L"CocosNetworkLogic customEventAction");
    switch (eventCode) {
        case PHOTON_SENDDATATYPE_CLIENT:
            {
                if (cocosNetworkDelegate == NULL) {
                    return;
                }
                ExitGames::Common::Hashtable ReceiveHash = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContent).getDataCopy();
                const ExitGames::Common::JVector<ExitGames::Common::Object>& keys = ReceiveHash.getKeys();
                cocos2d::CCPoint remoteNextPos;
                cocos2d::CCPoint remoteVector;
                remoteVector.y = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[0])).getDataCopy();
                remoteVector.x = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[1])).getDataCopy();
                remoteNextPos.y = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[2])).getDataCopy();
                remoteNextPos.x = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[3])).getDataCopy();
                mpOutputListener->write(ExitGames::Common::JString(L"rpx:") + remoteNextPos.x + L" rpy:" + remoteNextPos.y + L"");
//                mpOutputListener->write(ExitGames::Common::JString(L"keys[0]") + keys[0] + L" keys[1]" + keys[1] + L" keys[2]" + keys[2] + L" keys[3]" + keys[3] + L"");
                cocosNetworkDelegate->ReceivedClientData(remoteNextPos, remoteVector);
            }
            break;

        case PHOTON_SENDDATATYPE_GAMEDATA:
            {
                if (cocosNetworkDelegate == NULL) {
                    return;
                }
                ExitGames::Common::Hashtable ReceiveHash = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContent).getDataCopy();
                const ExitGames::Common::JVector<ExitGames::Common::Object>& keys = ReceiveHash.getKeys();
                cocos2d::CCPoint HostPos;
                cocos2d::CCPoint HostNextPos;
                cocos2d::CCPoint HostVector;
                cocos2d::CCPoint ClientPos;
                cocos2d::CCPoint ClientNextPos;
                cocos2d::CCPoint ClientVector;
                cocos2d::CCPoint PuckPos;
                cocos2d::CCPoint PuckNextPos;
                cocos2d::CCPoint PuckVector;
                HostPos.x       = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[17])).getDataCopy();
                HostPos.y       = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[16])).getDataCopy();
                HostNextPos.x   = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[15])).getDataCopy();
                HostNextPos.y   = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[14])).getDataCopy();
                HostVector.x    = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[13])).getDataCopy();
                HostVector.y    = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[12])).getDataCopy();
                ClientPos.x     = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[11])).getDataCopy();
                ClientPos.y     = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[10])).getDataCopy();
                ClientNextPos.x = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[9])).getDataCopy();
                ClientNextPos.y = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[8])).getDataCopy();
                ClientVector.x  = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[7])).getDataCopy();
                ClientVector.y  = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[6])).getDataCopy();
                PuckPos.x       = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[5])).getDataCopy();
                PuckPos.y       = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[4])).getDataCopy();
                PuckNextPos.x   = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[3])).getDataCopy();
                PuckNextPos.y   = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[2])).getDataCopy();
                PuckVector.x    = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[1])).getDataCopy();
                PuckVector.y    = ExitGames::Common::ValueObject<float>(ReceiveHash.getValue(keys[0])).getDataCopy();
                cocosNetworkDelegate->ReceivedGameData(HostPos, HostNextPos, HostVector, ClientPos, ClientNextPos, ClientVector, PuckPos, PuckNextPos, PuckVector);
            }
            break;
        case PHOTON_SENDDATATYPE_SCORE:
            {
                if (cocosNetworkDelegate == NULL) {
                    return;
                }
                ExitGames::Common::Hashtable ReceiveHash = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContent).getDataCopy();
                const ExitGames::Common::JVector<ExitGames::Common::Object>& keys = ReceiveHash.getKeys();
                int HostScore = ExitGames::Common::ValueObject<int>(ReceiveHash.getValue(keys[1])).getDataCopy();
                int ClientScore = ExitGames::Common::ValueObject<int>(ReceiveHash.getValue(keys[0])).getDataCopy();
                cocosNetworkDelegate->ReceivedScore(HostScore, ClientScore);
            }
            break;
        case PHOTON_SENDDATATYPE_READY:
            {
                ExitGames::Common::Hashtable ReceiveHash = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContent).getDataCopy();
                unsigned int readyPlayer = ValueObject<int>(ReceiveHash.getValue( (int) PhotonHashKey::int_PeerId ) ).getDataCopy();
                this->checkReadyPlayer( readyPlayer );
                mpOutputListener->write(L"ready event receieved");
            }
            break;
        default:
            break;
    }
}

cocos2d::CCString CocosNetworkLogic::getGameID()
{
    return _ccGameID = this->_gameID.ANSIRepresentation().cstr();
}
