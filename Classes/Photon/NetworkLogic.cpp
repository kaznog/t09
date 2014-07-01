#include "cocos2d.h"
#include "NetworkLogic.h"
#include "PhotonKeys.h"
#include "Constants.h"

#if defined _EG_MARMALADE_PLATFORM
#	if defined I3D_ARCH_X86
#		if defined _EG_MS_COMPILER
#			define PLAYER_NAME L"Marmalade X86 Windows"
#		else
#			define PLAYER_NAME L"Marmalade X86 OS X"
#		endif
#	elif defined I3D_ARCH_ARM
#		define PLAYER_NAME L"Marmalade ARM"
#	elif defined I3D_ARCH_MIPS
#		define PLAYER_NAME L"Marmalade MIPS"
#	else
#		define PLAYER_NAME L"unknown Marmalade platform"
#	endif
#elif defined _EG_WINDOWS_PLATFORM
#	define PLAYER_NAME L"Windows"
#elif defined _EG_IPHONE_PLATFORM
#	define PLAYER_NAME L"iOS"
#elif defined _EG_IMAC_PLATFORM
#	define PLAYER_NAME L"OS X"
#elif defined _EG_ANDROID_PLATFORM
#	define PLAYER_NAME L"Android"
#elif defined _EG_BLACKBERRY_PLATFORM
#	define PLAYER_NAME L"Blackberry"
#elif defined _EG_PS3_PLATFORM
#	define PLAYER_NAME L"PS3"
#elif defined _EG_LINUX_PLATFORM
#	define PLAYER_NAME L"Linux"
#else
#	define PLAYER_NAME L"unknown platform"
#endif

using namespace ExitGames;
using namespace LoadBalancing;
using namespace Common;

ExitGames::Common::JString& NetworkLogicListener::toString(ExitGames::Common::JString& retStr, bool withTypes) const
{
	return retStr;
}

State StateAccessor::getState(void) const
{
	return mState;
}

void StateAccessor::setState(State newState)
{
	mState = newState;
	for(unsigned int i=0; i<mStateUpdateListeners.getSize(); i++)
		mStateUpdateListeners[i]->stateUpdate(newState);
}

void StateAccessor::registerForStateUpdates(NetworkLogicListener* listener)
{
	mStateUpdateListeners.addElement(listener);
}

Input NetworkLogic::getLastInput(void) const
{
	return mLastInput;
}

void NetworkLogic::setLastInput(Input newInput)
{
	mLastInput = newInput;
}

State NetworkLogic::getState(void) const
{
	return mStateAccessor.getState();
}

const ExitGames::Common::JVector<ExitGames::LoadBalancing::Room*>& NetworkLogic::getRoomList()
{
    return mLoadBalancingClient.getRoomList();
}

const ExitGames::Common::JVector<ExitGames::Common::JString>& NetworkLogic::getRoomNameList()
{
    return mLoadBalancingClient.getRoomNameList();
}

// functions
NetworkLogic::NetworkLogic(OutputListener* listener, const wchar_t* appVersion,	ExitGames::LoadBalancing::AuthenticationValues authenticationValues)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mLoadBalancingClient(*this, L"if connecting to the PhotonCloud, replace this string with the application id", appVersion, PLAYER_NAME, false, authenticationValues)
	, mLastInput(INPUT_NON)
	, mpOutputListener(listener)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
    _readyPlayerChecklist = NULL;
    _gameID = 0;
	mStateAccessor.setState(STATE_INITIALIZED);
	mLoadBalancingClient.setDebugOutputLevel(ExitGames::Common::DebugLevel::INFO); // that instance of LoadBalancingClient and its implementation details
	ExitGames::Common::Base::setListener(this);
}

void NetworkLogic::registerForStateUpdates(NetworkLogicListener* listener)
{
	mStateAccessor.registerForStateUpdates(listener);
}

void NetworkLogic::connect(void)
{
	/* Specify the address of your Photon Server master server or the url of a Photon Cloud region here.
	If you don't specify a port, then Photons default master server ports (UDP: 5055, TCP: 4530) will be used.
	Valid cloud regions are:
	Western Europe: app-eu.exitgamescloud.com
	US: app-us.exitgamescloud.com
	Asia: app-asia.exitgamescloud.com
	Japan: app-jp.exitgamescloud.com
	Eastern Europe: app-eu-east.exitgamescloud.com
	The parameterless overload defaults to app-eu.exitgamescloud.com. */
	mLoadBalancingClient.connect(L"app-jp.exitgamescloud.com");
	mStateAccessor.setState(STATE_WAITING);
}

void NetworkLogic::disconnect(void)
{
	mLoadBalancingClient.disconnect();
}

ExitGames::LoadBalancing::Room* NetworkLogic::getRoomWithID( ExitGames::Common::JString gameID )
{
    const ExitGames::Common::JVector<ExitGames::LoadBalancing::Room*>& roomList = this->getRoomList();
    Room* room = NULL;
    
    for( unsigned int i = 0; i < roomList.getSize(); i++ )
    {
        room = roomList.getElementAt( i );
        if( room->getName() == gameID )
        {
            break;
        }
    }
    
    return room;
}

ExitGames::LoadBalancing::MutableRoom& NetworkLogic::getCurrentRoom()
{
    return mLoadBalancingClient.getCurrentlyJoinedRoom();
}

ExitGames::LoadBalancing::MutableRoom& NetworkLogic::getCurrentlyJoinedRoom()
{
    return mLoadBalancingClient.getCurrentlyJoinedRoom();
}

short NetworkLogic::getPeerId()
{
    return this->mLoadBalancingClient.getPeerId();
}

int NetworkLogic::getCountPlayersInGame()
{
    return this->mLoadBalancingClient.getCountPlayersIngame();
}

bool NetworkLogic::getIsInGameRoom()
{
    return this->mLoadBalancingClient.getIsInGameRoom();
}

void NetworkLogic::opCreateRoom(void)
{
	// if last digits are always nearly the same, this is because of the timer calling this function being triggered every x ms with x being a factor of 10
	ExitGames::Common::JString roomName;
    roomName = GETUPTIMEMS();
//    ExitGames::Common::JVector<ExitGames::Common::JString> roomPropsInLobby = Common::JVector<Common::JString>();
//    roomPropsInLobby.addElement(Common::JString(L"map"));
//    roomPropsInLobby.addElement(Common::JString(L"ai"));
//    Common::Hashtable customRoomProperties = Common::Hashtable();
//    customRoomProperties.put("map", 1);
	mLoadBalancingClient.opCreateRoom(roomName, true, true, 2);
	mStateAccessor.setState(STATE_JOINING);
	mpOutputListener->writeLine(ExitGames::Common::JString(L"creating game \"") + roomName + L"\"");
//    char id[64] = { '\0' };
//    sprintf( id, "%u", GETTIMEMS() );
//    _gameID = id;
//    
//    Hashtable roomProperties = Hashtable();
//    int peerID = this->getPeerId();
//    
//    Hashtable playerProperties = Hashtable();
//    
//    JVector<JString> publicRoomProperties = JVector<JString>();
//    
//	mStateAccessor.setState(STATE_JOINING);
//	mpOutputListener->writeLine(ExitGames::Common::JString(L"creating game \"") + _gameID + L"\"");
}

void NetworkLogic::opJoinRandomRoom(void)
{
//    Common::Hashtable customRoomProperties = Common::Hashtable();
//    customRoomProperties.put("map", 1);
    mLoadBalancingClient.opJoinRandomRoom();
}

void NetworkLogic::opJoinRoom(const ExitGames::Common::JString &gameID)
{
    mLoadBalancingClient.opJoinRoom(gameID);
}

void NetworkLogic::opLeaveRoom(void)
{
    mLoadBalancingClient.opLeaveRoom();
}

void NetworkLogic::opRaiseEvent(bool reliable, const ExitGames::Common::Hashtable &parameters, nByte eventCode)
{
    mLoadBalancingClient.opRaiseEvent(reliable, parameters, eventCode);
    mpOutputListener->writeLine(ExitGames::Common::JString(L"sending Event"));
}

void NetworkLogic::run(void)
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
                /*		case STATE_INITIALIZED:
                 connect();
                 mStateAccessor.setState(STATE_CONNECTING);
                 mpOutputListener->writeLine(L"connecting");
                 break;
                 case STATE_CONNECTING:
                 break; // wait for callback
                 */
            case STATE_CONNECTING:
                connect();
                mpOutputListener->writeLine(L"connecting");
                break; // wait for callback
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
                break; // wait for callback
            case STATE_JOINED:
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
                break;
            case STATE_READY:
                break;
            case STATE_IN_GAME:
                break;
            case STATE_LEAVING:
                opLeaveRoom();
                break; // wait for callback
            case STATE_LEFT:
                //change button back to Join Game
                mStateAccessor.setState(STATE_CONNECTED);
                break;
            case STATE_DISCONNECTING:
                break; // wait for callback
            case STATE_DISCONNECTED:
                mStateAccessor.setState(STATE_INITIALIZED);
                break; // wait for callback
            default:
                break;
		}
	}
	mLastInput = INPUT_NON;
	mLoadBalancingClient.service();
}

void NetworkLogic::sendEvent(void)
{
	mLoadBalancingClient.opRaiseEvent(false, ExitGames::Common::Hashtable(), 100);
	mpOutputListener->write(ExitGames::Common::JString(L"s"));
}

// protocol implementations

void NetworkLogic::debugReturn(ExitGames::Common::DebugLevel::DebugLevel /*debugLevel*/, const ExitGames::Common::JString& string)
{
	mpOutputListener->debugReturn(string);
}

void NetworkLogic::connectionErrorReturn(int errorCode)
{
    mpOutputListener->writeLine(ExitGames::Common::JString(L"connection failed with error ") + errorCode );
	mStateAccessor.setState(STATE_DISCONNECTED);
}

void NetworkLogic::clientErrorReturn(int errorCode)
{
	mpOutputListener->writeLine(ExitGames::Common::JString(L"received error ") + errorCode + L" from client");
}

void NetworkLogic::warningReturn(int warningCode)
{
	mpOutputListener->writeLine(ExitGames::Common::JString(L"received warning ") + warningCode + L" from client");
}

void NetworkLogic::serverErrorReturn(int errorCode)
{
	mpOutputListener->writeLine(ExitGames::Common::JString(L"received error ") + errorCode + " from server");
}

void NetworkLogic::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
	mpOutputListener->writeLine(L"");
	mpOutputListener->writeLine(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");
}

void NetworkLogic::leaveRoomEventAction(int playerNr)
{
	mpOutputListener->writeLine(L"");
	mpOutputListener->writeLine(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game");
}

void NetworkLogic::checkReadyPlayer( unsigned int readyPlayer )
{
    this->_readyPlayerChecklist |= 1 >> readyPlayer;
}

void NetworkLogic::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	// you do not receive your own events, unless you specify yourself as one of the receivers explicitly, so you must start 2 clients, to receive the events, which you have sent, as sendEvent() uses the default receivers of opRaiseEvent() (all players in same room like the sender, except the sender itself)
	EGLOG(ExitGames::Common::DebugLevel::ALL, L"NetworkLogic customEventAction");
	mpOutputListener->write(L"r");
    switch (eventCode)
    {
        case EVENT_CHAT:
            mpOutputListener->write(L"chat event receieved");
            break;
        case EVENT_DECLARE_READY:
        {
            ExitGames::Common::Hashtable ReceiveHash = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContent).getDataCopy();
            unsigned int readyPlayer = ValueObject<int>(ReceiveHash.getValue( (int) PhotonHashKey::int_PeerId ) ).getDataCopy();
            this->checkReadyPlayer( readyPlayer );
            mpOutputListener->write(L"ready event receieved");
        }
            break;
        default:
            return;
    }
    char code[2] = { '\0' };
    sprintf( code, "%d", eventCode );
    mpOutputListener->write( code );
}

bool NetworkLogic::allPlayersReady()
{
    unsigned int checklist = this->_readyPlayerChecklist;
    switch( this->mLoadBalancingClient.getCountPlayersIngame() )
    {
        case 2:
            return checklist == 0x3;
        case 3:
            return checklist == 0x7;
        case 4:
            return checklist == 0xf;
        default:
            return false;
    }
}

unsigned int NetworkLogic::countMyGames()
{
    return this->_myGamesCount;
}

void NetworkLogic::connectReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
        mpOutputListener->writeLine(ExitGames::Common::JString(L"connectReturn ") + errorString.cstr() + L"");
		mStateAccessor.setState(STATE_DISCONNECTING);
		return;
	}
	mpOutputListener->writeLine(L"connected");
	mStateAccessor.setState(STATE_CONNECTED);
    
    mLoadBalancingClient.opJoinLobby();
    mpOutputListener->notify(PhotonNoteKey::ConnectReturn);
}

void NetworkLogic::disconnectReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeLine(L"disconnected");
	mStateAccessor.setState(STATE_DISCONNECTED);
    mpOutputListener->notify(PhotonNoteKey::DisconnectReturn);
}

void NetworkLogic::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeLine(L"opCreateRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_CONNECTED);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeLine(L"game room \"" + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + "\" has been created");
	mpOutputListener->writeLine(L"regularly sending dummy events now");
	mStateAccessor.setState(STATE_JOINED);
    mpOutputListener->notify(PhotonNoteKey::CreateRoomReturn);
}

void NetworkLogic::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeLine(L"opJoinRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_CONNECTED);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeLine(L"game room \"" + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + "\" has been successfully joined");
	mpOutputListener->writeLine(L"regularly sending dummy events now");
	mStateAccessor.setState(STATE_JOINED);
    mpOutputListener->notify(PhotonNoteKey::JoinRoomReturn);
    mpOutputListener->notify(PhotonNoteKey::JoinSpecificRoomReturn);
}

void NetworkLogic::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeLine(L"opJoinRandomRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_CONNECTED);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeLine(L"game room \"" + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + "\" has been successfully joined");
	mpOutputListener->writeLine(L"regularly sending dummy events now");
	mStateAccessor.setState(STATE_JOINED);
    mpOutputListener->notify(PhotonNoteKey::JoinRoomReturn);
    mpOutputListener->notify(PhotonNoteKey::JoinRandomRoomReturn);
}

void NetworkLogic::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeLine(L"opLeaveRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_DISCONNECTING);
		return;
	}
	mpOutputListener->writeLine(L"game room has been successfully left");
	mStateAccessor.setState(STATE_LEFT);
    mpOutputListener->notify(PhotonNoteKey::LeaveRoomReturn);
}

void NetworkLogic::joinLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeLine(L"joined lobby");
    
    mpOutputListener->notify(PhotonNoteKey::JoinLobbyReturn);
}

void NetworkLogic::leaveLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeLine(L"left lobby");
    
    mpOutputListener->notify(PhotonNoteKey::LeaveLobbyReturn);
}
