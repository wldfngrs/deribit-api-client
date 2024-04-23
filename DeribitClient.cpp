#pragma warning(disable : 4996)

#include "DeribitClient.h"
#include "quickfix/Session.h"

#include <iostream>
#include <windows.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline std::string base64encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}

static inline std::string getTimeStamp() {
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::string millisecCountString = std::to_string(ms.count());

	return millisecCountString;
}


char queryAction() {
	char value;
	std::cout
		<< "[1] To 'Query Market Data', [2] to 'Quit'" << std::endl;
	std::cout << "[-] ? ";
	std::cin >> value;

	switch (value) {
	case '1': case '2':
		break;
	default: throw std::exception();
	}

	return value;
}

void DeribitClient::run() {
	while (true) {
		try {
			char action = queryAction();

			if (action == '1') {
				std::cout << std::endl;
				queryMarketDataRequest();
			}
			else if (action == '2') {
				break;
			}
		}
		catch (std::exception& e) {
			std::cout << "Message Not Sent: " << e.what() << std::endl;
		}
	}
}

void DeribitClient::onCreate(const FIX::SessionID& sessionID) {
	std::cout << "Created session with ID info: " << sessionID << "..." << std::endl;
	session = sessionID;
}

void DeribitClient::onLogon(const FIX::SessionID& sessionID) {
	std::cout << std::endl << "Logon - " << sessionID << "..." << std::endl;
}

void DeribitClient::onLogout(const FIX::SessionID& sessionID) {
	std::cout << std::endl << "Terminating session with ID info: " << sessionID << "..." << std::endl;
	std::cout <<  "Logout - " << sessionID << "..." << std::endl;
}

// called on outgoing admin messages
void DeribitClient::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) {
	if (FIX::MsgType_Logon == message.getHeader().getField(FIX::FIELD::MsgType)) {
		// Can be recovered from the API tab of the Deribit website
		std::string clientID = "CLIENT ID";
		std::string clientSecret = "CLIENT SECRET";

		unsigned char nonce[32] = {};
		RAND_bytes(nonce, sizeof(nonce));

		std::string base64nonce = base64encode(nonce, sizeof(nonce));
		std::string rawData = getTimeStamp() + "." + base64nonce;
		std::string baseSignatureString = rawData + clientSecret;

		unsigned char hash[SHA256_DIGEST_LENGTH];
		SHA256_CTX sha256;
		SHA256_Init(&sha256);
		SHA256_Update(&sha256, baseSignatureString.c_str(), baseSignatureString.size());
		SHA256_Final(hash, &sha256);

		std::string passwordShaBase64 = base64encode(hash, sizeof(hash));

		message.setField(FIX::RawData(rawData));
		message.setField(FIX::Username(clientID));
		message.setField(FIX::Password(passwordShaBase64));

		// std::cout << std::endl << "(ADMIN) OUTGOING - " << message.toXML() << std::endl;
	}
}

// called on incoming admin messages
void DeribitClient::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
	// std::cout << std::endl << "(ADMIN) INCOMING - " << message.toString() << "..." << std::endl;
}

// called on outgoing app messages
void DeribitClient::toApp(FIX::Message& message, const FIX::SessionID& sessionID) throw (FIX::DoNotSend) {
	try {
		FIX::PossDupFlag possDupFlag;
		message.getHeader().getField(possDupFlag);
		if (possDupFlag) throw FIX::DoNotSend();
	}
	catch (FIX::FieldNotFound&) {}

	// std::cout << std::endl << "(APP) OUTGOING - " << message.toString() << std::endl;
}

// called on incoming app messages
void DeribitClient::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
	crack(message, sessionID);
	std::cout << std::endl << "(APP) INCOMING - " << message.toString() << std::endl;
}

void DeribitClient::onMessage(const FIX44::MarketDataRequest& message, const FIX::SessionID&) {
}

void DeribitClient::onMessage(const FIX44::MarketDataSnapshotFullRefresh& message, const FIX::SessionID&) {
}

void DeribitClient::onMessage(const FIX44::MarketDataRequestReject& message, const FIX::SessionID&) {
}

void DeribitClient::queryMarketDataRequest() {
	std::cout << "Querying Market Data..." << std::endl;
	FIX::Message md = queryMarketDataRequest44();
	FIX::Session::sendToTarget(md, session);
}

FIX44::MarketDataRequest DeribitClient::queryMarketDataRequest44() {
	FIX::MDReqID mdReqID("MARKETDATAID");
	FIX::SubscriptionRequestType subType(FIX::SubscriptionRequestType_SNAPSHOT);
	FIX::MarketDepth marketDepth(0);

	FIX44::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
	FIX::MDEntryType mdEntryType(FIX::MDEntryType_BID);
	marketDataEntryGroup.set(mdEntryType);

	FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
	FIX::Symbol symbol("BTC-26APR24");
	symbolGroup.set(symbol);

	FIX44::MarketDataRequest message(mdReqID, subType, marketDepth);
	message.addGroup(marketDataEntryGroup);
	message.addGroup(symbolGroup);

	queryMDRHeader(message.getHeader());

	return message;
}

void DeribitClient::queryMDRHeader(FIX::Header& header) {
	header.setField(FIX::MsgType(FIX::MsgType_MarketDataRequest));
}