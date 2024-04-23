#pragma once

#define WIN32_LEAN_AND_MEAN

#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/Logon.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"

#include <random>
#include <string>
#include <chrono>

class DeribitClient 
	: public FIX::Application,
	  public FIX::MessageCracker {
public:
	FIX::SessionID session;
	void run();

private:
	void onCreate(const FIX::SessionID&) override;
	void onLogon(const FIX::SessionID&) override;
	void onLogout(const FIX::SessionID&) override;
	void toAdmin(FIX::Message&, const FIX::SessionID&) override;
	void toApp(FIX::Message&, const FIX::SessionID&) throw(FIX::DoNotSend)override;
	void fromAdmin(const FIX::Message&, const FIX::SessionID&) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) override;
	void fromApp(const FIX::Message&, const FIX::SessionID&) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) override;
	
	void onMessage(const FIX44::MarketDataRequest&, const FIX::SessionID&);
	void onMessage(const FIX44::MarketDataSnapshotFullRefresh&, const FIX::SessionID&);
	void onMessage(const FIX44::MarketDataRequestReject&, const FIX::SessionID&);

	void queryMarketDataRequest();
	void queryMDRHeader(FIX::Header& header);

	FIX44::MarketDataRequest queryMarketDataRequest44();
};