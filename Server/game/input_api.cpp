#include "stdafx.h"
#include <sstream>

#include "constants.h"
#include "config.h"
#include "utils.h"

#include "desc.h"
#include "desc_manager.h"
#include "desc_client.h"
#include "p2p.h"

#include "log.h"
#include "quest.h"
#include "questmanager.h"
#include "priv_manager.h"
#include "fishing.h"
#include "cmd.h"
#include "input.h"

#include "char_manager.h"

ACMD(do_block_chat);

// bool IsEmptyAdminPage()
// {
	// return g_stAdminPageIP.empty();
// }

// bool IsAdminPage(const char * ip)
// {
	// for (size_t n = 0; n < g_stAdminPageIP.size(); ++n)
	// {
		// if (g_stAdminPageIP[n] == ip)
			// return 1;
	// }
	// return 0;
// }

// void ClearAdminPages()
// {
	// for (size_t n = 0; n < g_stAdminPageIP.size(); ++n)
		// g_stAdminPageIP[n].clear();

	// g_stAdminPageIP.clear();
// }

CInputAPI::CInputAPI()
{
	m_desc = nullptr;
}

void CInputAPI::CloseDesc()
{
	m_desc = nullptr;
}

void CInputAPI::Process(std::string cmd)
{
	std::string stResult = "UNKNOWN";
	LPDESC d = m_desc;

	if (!d) {
		sys_err("Empty DESC while processing CMD input");
		return;
	}

	//Detect if we are authing ourselves
	// if (cmd == g_stAdminPagePassword)
	// {
		// if (!IsEmptyAdminPage())
		// {
			// if (!IsAdminPage(inet_ntoa(d->GetAddr().sin_addr)))
			// {
				// char szTmp[64];
				// snprintf(szTmp, sizeof(szTmp), "WEBADMIN : Wrong Connector : %s", inet_ntoa(d->GetAddr().sin_addr));
				// stResult += szTmp;
			// }
			// else
			// {
				
				// stResult = "ADMIN_GRANTED";
			// }
		// }
		// else
		// {
			// d->SetAdminMode();
			// stResult = "ADMIN_GRANTED";
		// }
	// }
	
	d->SetAdminMode();

	//Detect if we are authing ourselves
	// if (cmd == g_stAdminPageSuperPassword)
	// {		
		// d->SetSuperAdminMode();
		// stResult = "SUPER_ADMIN_GRANTED";	
	// }

	if (!d->IsAdminMode()) {
		stResult = "DENIED";
	}
	else if (!cmd.compare("IS_SERVER_OPEN"))
	{
		stResult = (g_bNoMoreClient) ? "NO" : "YES";
	}
	else if (!cmd.compare("USER_COUNT"))
	{
		int iTotalUserCount = 0;

		// auto serverCountOnMap = DESC_MANAGER::instance().GetCountOnServerMap();

		// for (auto server_count_on_map : serverCountOnMap)
		// {
			// iTotalUserCount += server_count_on_map.second;
		// }

		

		char szTmp[64];

		int iTotal;
		int * paiEmpireUserCount;
		int iLocal;
		DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);
		// int shopCount = P2P_MANAGER::instance().GetForeignShopCount() + CHARACTER_MANAGER::instance().CountOfflineShops();

		snprintf(szTmp, sizeof(szTmp), "%d %d %d %d %d %d %d", iTotal, paiEmpireUserCount[1], paiEmpireUserCount[2], paiEmpireUserCount[3], iLocal);
		stResult = szTmp;
	}
	else if (!cmd.compare("CHECK_P2P_CONNECTIONS"))
	{
		std::ostringstream oss(std::ostringstream::out);

		oss << "P2P CONNECTION NUMBER : " << P2P_MANAGER::instance().GetDescCount() << "\n";
		std::string hostNames;
		P2P_MANAGER::Instance().GetP2PHostNames(hostNames);
		oss << hostNames;
		stResult = oss.str();
		TPacketGGCheckAwakeness packet;
		packet.bHeader = HEADER_GG_CHECK_AWAKENESS;

		P2P_MANAGER::instance().Send(&packet, sizeof(packet));
	}
	// else if (!cmd.compare("PACKET_INFO"))
	// {
		// CInputProcessor * p = d->GetInputProcessor();
		// if (p)
		// {
			// p->GetPacketInfo()->Log("packet_info.txt");
			// stResult = "OK";
		// }
	// }
	//gift notify delete command
/* 	else if (!cmd.compare(0, 15, "DELETE_AWARDID "))
	{
		char szTmp[64];
		std::string msg = cmd.substr(15, 26);	// item_award의 id범위?

		TPacketDeleteAwardID p;
		p.dwID = (DWORD)(atoi(msg.c_str()));
		snprintf(szTmp, sizeof(szTmp), "Sent to DB cache to delete ItemAward, id: %d", p.dwID);
		//sys_log(0,"%d",p.dwID);
		// strlcpy(p.login, msg.c_str(), sizeof(p.login));
		db_clientdesc->DBPacket(HEADER_GD_DELETE_AWARDID, 0, &p, sizeof(p));
		stResult += szTmp;
	} */
	else if (!cmd.compare(0, 7, "NOTICE "))
	{
		std::string msg = cmd.substr(7, 50);
		LogManager::instance().CharLog(0, 0, 0, 1, "NOTICE", msg.c_str(), d->GetHostName());
		BroadcastNotice(msg.c_str());
		stResult = "OK";
	}
	else if (!cmd.compare("SHUTDOWN"))
	{
		LogManager::instance().CharLog(0, 0, 0, 2, "SHUTDOWN", "", d->GetHostName());
		TPacketGGShutdown p;
		p.bHeader = HEADER_GG_SHUTDOWN;
		p.secs = 10;
		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));


		
		sys_err("Accept shutdown command from %s.", d->GetHostName());
		Shutdown(10);
		stResult = "OK";
	}
	else if (!cmd.compare("SHUTDOWN_ONLY"))
	{
		LogManager::instance().CharLog(0, 0, 0, 2, "SHUTDOWN", "", d->GetHostName());
		sys_err("Accept shutdown only command from %s.", d->GetHostName());
		Shutdown(10);
		stResult = "OK";
	}
	else if (!cmd.compare(0, 3, "DC "))
	{
		std::string msg = cmd.substr(3, LOGIN_MAX_LEN);

		//Try to disconnect locally and relay if that fails
		LPDESC d = DESC_MANAGER::instance().FindByLoginName(msg);

		if (d) {
			DESC_MANAGER::instance().DestroyDesc(d);
			stResult = "DC_OK";
			sys_log(0, "API: Local DC %s", msg.c_str());
		}
		else
		{
			TPacketGGDisconnect pgg;

			pgg.bHeader = HEADER_GG_DISCONNECT;
			strlcpy(pgg.szLogin, msg.c_str(), sizeof(pgg.szLogin));

			sys_log(0, "API: Remote DC %s", msg.c_str());
			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGDisconnect));

			// delete login key
			{
				TPacketDC p;
				strlcpy(p.login, msg.c_str(), sizeof(p.login));
				db_clientdesc->DBPacket(HEADER_GD_DC, 0, &p, sizeof(p));
			}
			stResult = "DC_AROUND";
		}
	}
/* 	else if (!cmd.compare(0, 11, "CLOSE_SHOP "))
	{
		DWORD pid;

		std::string msg = cmd.substr(11);
		std::istringstream is(msg);
		is >> pid;

		db_clientdesc->DBPacketHeader(HEADER_GD_SHOP, 0, sizeof(BYTE) + sizeof(DWORD));
		BYTE subheader = SHOP_SUBHEADER_GD_CLOSE_REQUEST;
		db_clientdesc->Packet(&subheader, sizeof(BYTE));
		db_clientdesc->Packet(&pid, sizeof(DWORD));

		sys_log(0, "API: Shop closing requested for pid #%d", pid);
		stResult = "CLOSE_REQ_SENT";
	} */
	else if (!cmd.compare(0, 6, "RELOAD"))
	{
		if (cmd.size() == 6)
		{
			LoadStateUserCount();
			db_clientdesc->DBPacket(HEADER_GD_RELOAD_PROTO, 0, NULL, 0);
		}
		else
		{
			char c = cmd[7];

			switch (LOWER(c))
			{
			case 'u':
				LoadStateUserCount();
				stResult = "DUMP_COUNT_OK";
				sys_log(0, "API: Reloading user state count.");
				break;
			case 'v':
				sys_log(0, "API: Reloading client version.");
				TPacketGGUpdateClientVersion p;
				p.bHeader = HEADER_GG_CLIENT_VERSION;
				P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGUpdateClientVersion));

				break;
			case 'p':
				db_clientdesc->DBPacket(HEADER_GD_RELOAD_PROTO, 0, NULL, 0);
				stResult = "RELOAD_PROTO_OK";
				sys_log(0, "API: Reloading proto.");
				break;

			case 'q':
				quest::CQuestManager::instance().Reload();
				stResult = "RELOAD_QUEST_OK";
				sys_log(0, "API: Reloading quest.");
				break;

			case 'f':
				fishing::Initialize();
				stResult = "RELOAD_FISH_OK";
				sys_log(0, "API: Reloading fish.");
				break;

			case 'a':
				TPacketReloadAdmin pack;
				strlcpy(pack.szIP, g_szPublicIP, sizeof(pack.szIP));

				db_clientdesc->DBPacket(HEADER_GD_RELOAD_ADMIN, 0, &pack, sizeof(TPacketReloadAdmin));
				sys_log(0, "API: Reloading admin infomation.");
				stResult = "RELOAD_ADMIN_OK";
				break;
			}
		}
	}
	else if (!cmd.compare(0, 6, "EVENT "))
	{
		std::istringstream is(cmd);
		std::string strEvent, strFlagName;
		long lValue;
		is >> strEvent >> strFlagName >> lValue;

		if (!is.fail())
		{
			sys_log(0, "EXTERNAL EVENT FLAG name %s value %d", strFlagName.c_str(), lValue);
			quest::CQuestManager::instance().RequestSetEventFlag(strFlagName, lValue);
			stResult = "EVENT FLAG CHANGE ";
			stResult += strFlagName;

			sys_log(0, "API: Request event flag change: %s to %d", strFlagName.c_str(), lValue);
		}
		else
		{
			stResult = "EVENT FLAG FAIL";
		}
	}
	// BLOCK_CHAT
	else if (!cmd.compare(0, 11, "BLOCK_CHAT "))
	{
		std::istringstream is(cmd);
		std::string strBlockChat, strCharName;
		long lDuration;
		is >> strBlockChat >> strCharName >> lDuration;

		if (!is.fail())
		{
			sys_log(0, "EXTERNAL BLOCK_CHAT name %s duration %d", strCharName.c_str(), lDuration);

			do_block_chat(NULL, const_cast<char*>(cmd.c_str() + 11), 0, 0);

			stResult = "BLOCK_CHAT ";
			stResult += strCharName;
		}
		else
		{
			stResult = "BLOCK_CHAT FAIL";
		}
	}
	// END_OF_BLOCK_CHAT
	else if (!cmd.compare(0, 12, "PRIV_EMPIRE "))
	{
		int	empire, type, value, duration;
		std::istringstream is(cmd);
		std::string strPrivEmpire;
		is >> strPrivEmpire >> empire >> type >> value >> duration;

		// 최대치 10배
		value = MINMAX(0, value, 1000);
		stResult = "PRIV_EMPIRE FAIL";

		if (!is.fail())
		{
			// check parameter
			if (empire < 0 || 3 < empire);
			else if (type < 1 || 4 < type);
			else if (value < 0);
			else if (duration < 0);
			else
			{
				stResult = "PRIV_EMPIRE SUCCEED";

				// 시간 단위로 변경
				duration = duration * (60 * 60);

				sys_log(0, "_give_empire_privileage(empire=%d, type=%d, value=%d, duration=%d) by web",
					empire, type, value, duration);
				CPrivManager::instance().RequestGiveEmpirePriv(empire, type, value, duration);
			}
		}
	}
	else if (!cmd.compare(0, 11, "HACK_LIMIT "))
	{
		int	value;
		std::istringstream is(cmd);
		std::string command;
		is >> command >> value;

		value = MINMAX(0, value, 1000);
		stResult = "HACK_LIMIT_CHANGE_FAIL";

		if (!is.fail())
		{
			// check params
			if (value > 0)
			{
				stResult = "HACK_LIMIT_CHANGE_OK";
				g_iSyncHackLimitCount = value;
				sys_err("Config change:: Hack limit set to %d", g_iSyncHackLimitCount);
			}
		}
	}
	else if (!cmd.compare(0, 6, "FLUSH "))
	{
		DWORD	pid;
		std::istringstream is(cmd);
		std::string command;
		is >> command >> pid;

		db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
		db_clientdesc->Packet(&pid, sizeof(DWORD));

		stResult = "OK";
		sys_log(0, "CMD flush for %lu", pid);
	}

	sys_log(1, "TEXT %s RESULT %s", cmd.c_str(), stResult.c_str());
	stResult += "\n";
	d->Packet(stResult.c_str(), stResult.length());
}

bool CInputAPI::IsAuthorized()
{
	return false;
}