class PluginKrBankingClientManager extends PluginBase
{
    protected ref AR_BancoMenu                    m_BankingMenu;
    protected int                                   m_PlayersCurrency;
    protected bool                                  m_IsFirstRequest = true;
    protected bool                                  m_IsWaitingForServersResponse;
    protected string                                m_ClanID;
    protected string                                m_PlainID;
    protected ref AR_BancoClientConfig            m_clientSettings;
    protected ref array<ref bankingplayerlistobj>   m_BankingPlayers;
    protected ref ClanDataBaseManager               m_OwnClan;
    protected bool                                  m_AdminMenuNeedsAnUpdate = false;

    void PluginKrBankingClientManager()
    {
        if(GetGame().IsClient() && !GetGame().IsServer()) //Maybe this will solve wrong rpc call.
        {
            Init();
        }
    }

    void Init()
    {
        GetRPCManager().AddRPC("AR_Banco","PlayerDataResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco","ServerConfigResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco","PlayeristResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco", "ClanSyncRespose", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco", "UIQuitRequest", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco", "MoneyDropRecvied", this, SingleplayerExecutionType.Client);
        GetRPCManager().SendRPC("AR_Banco", "ServerConfigRequest", null, true);
    }

    //!Gets called when client opens the Banking Menu
    void PlayerDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param3<int, string, int> data;
            if ( !ctx.Read( data ) ) return;
            m_PlayersCurrency = data.param1;
            m_ClanID          = data.param2;
            m_clientSettings.IncreaseMaxLimit(data.param3);

        }
    }

    void ServerConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param2<AR_BancoClientConfig, string> data;
            if ( !ctx.Read( data ) ) return;
            m_clientSettings = new AR_BancoClientConfig(data.param1.MaxCurrency, data.param1.InteractDelay, data.param1.isRobActive, data.param1.isBankCardNeeded, data.param1.BankingCurrency, data.param1.CostsToCreateClan, data.param1.MaxClanAccountLimit, data.param1.IsClanAccountActive);
            m_PlainID = data.param2;
            m_clientSettings.TimeInSecToRobATM = data.param1.TimeInSecToRobATM;
            
        }
    }

    void PlayeristResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param1<array<ref bankingplayerlistobj>> data;
            if ( !ctx.Read( data ) ) return;
            if(m_BankingPlayers)
                m_BankingPlayers.Clear();
            m_BankingPlayers = data.param1;
            if(m_BankingMenu && !m_AdminMenuNeedsAnUpdate)
                m_BankingMenu.InvokePlayerList();
            if(m_AdminMenuNeedsAnUpdate)
                GetBankingClientAdminManager().UpdatePlayerlist();
        }
    }

    void ClanSyncRespose(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            Param1<ClanDataBaseManager> data;
            if (!ctx.Read(data)) return;

            m_OwnClan = data.param1;
            m_OwnClan.SetMembers(data.param1.GetClanMembers());
            if(m_BankingMenu)
                m_BankingMenu.LoadClanLogs();
        }
    }

    void UIQuitRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            if(m_BankingMenu && m_BankingMenu.IsVisible())
            {
                CloseBankingMenu();
            }
        }
    }

    void MoneyDropRecvied(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Client)
        {
            SEffectManager.PlaySoundOnObject( "MoneyDrop_SoundSet", GetGame().GetPlayer() );
        }
    }

    void OpenBankingMenu()
    {
        GetRPCManager().SendRPC("AR_Banco", "PlayerDataRequest", null, true); //Send RPC for data.
        if(GetGame().GetUIManager().GetMenu() == NULL && m_BankingMenu == null)
        {
            m_BankingMenu = AR_BancoMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(AR_Banco_MENU, null));
            m_BankingMenu.SetIsBankingMenuOpen(false);
        }
        else
        {
            if(GetGame().GetUIManager().GetMenu() == NULL && !m_BankingMenu.IsBankingMenuOpen())
            {
                GetGame().GetUIManager().ShowScriptedMenu(m_BankingMenu, NULL);
                m_BankingMenu.SetIsBankingMenuOpen(true);
            }
        }
    }

    void CloseBankingMenu()
    {
        if(m_BankingMenu && m_BankingMenu.IsVisible())
        {
            m_BankingMenu.CloseBankingMenu();
        }
    }

    void RequestOnlinePlayers(bool AdminRequest = false)
    {
        GetRPCManager().SendRPC("AR_Banco", "PlayerListRequst", null, true);
        m_AdminMenuNeedsAnUpdate = AdminRequest;
    }

    void RequestRemoteToWithdraw(int amount, int mode)
    {
        GetRPCManager().SendRPC("AR_Banco", "WithdrawRequest", new Param2<int, int>(amount, mode), true);
    }

    void RequestRemoteToDeposit(int amount, int mode)
    {
        GetRPCManager().SendRPC("AR_Banco", "DepositRequest", new Param2<int, int>(amount, mode), true);
    }

    void RequestRemoteForTransfer(string targetPlainID, int amount)
    {
        GetRPCManager().SendRPC("AR_Banco", "TransferRequest", new Param2<string, int>(targetPlainID, amount), true);
    }

    void RequestRemoteLeaveClan()
    {
        GetRPCManager().SendRPC("AR_Banco", "ClanMemberLeave", new Param1<string>(GetSteamID()), true);
    }

    void RequestRemoteClanCreate(string Clanname, string ClanTag)
    {
        if(!Clanname || !ClanTag)
            return;
        GetRPCManager().SendRPC("AR_Banco", "ClanCreateRequest", new Param2<string, string>(Clanname, ClanTag), true);
    }

    void RequestEditPermission(ref PermissionObject newPermission, string TargetsSteamID)
    {
        if(!newPermission || !TargetsSteamID) return;

        GetRPCManager().SendRPC("AR_Banco", "ClanUpdateMember", new Param2<PermissionObject, string>(newPermission, TargetsSteamID), true);
    }

    void RequestRemoteEditClan(string Name, string Tag)
    {
        GetRPCManager().SendRPC("AR_Banco", "ClanUpdate", new Param2<string, string>(Name, Tag), true);
    }

    ClanDataBaseManager GetClientsClanData()
    {
        return m_OwnClan;
    }

    array<ref CurrencySettings> GetServersCurrencyData()
    {
        return m_clientSettings.BankingCurrency;
    }

    AR_BancoClientConfig GetClientSettings()
    {
        return m_clientSettings;
    }

    int GetBankCredits()
    {
        return m_PlayersCurrency;
    }
    
    //!Returns the currency Ammount in Players Invenory
    int GetPlayerCurrencyAmount()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		
		int currencyAmount = 0;
		
		array<EntityAI> itemsArray = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		ItemBase item;
		
		for (int i = 0; i < itemsArray.Count(); i++)
		{
			Class.CastTo(item, itemsArray.Get(i));

			if (!item)
				continue;

			for (int j = 0; j < GetServersCurrencyData().Count(); j++)
			{
				if(item.GetType() == GetServersCurrencyData().Get(j).CurrencyName)
				{
					currencyAmount += GetItemAmount(item) * GetServersCurrencyData().Get(j).CurrencyValue;
				}
			}
		}
		return currencyAmount;
	}

    array<ref bankingplayerlistobj> GetOnlinePlayers()
    {
        return m_BankingPlayers;
    }

    bool hasClan()
    {
        if(m_ClanID && m_ClanID != "NONE")
            return true;
        return false;
    }

    void RequestClanData()
    {
        GetRPCManager().SendRPC("AR_Banco", "ClanSyncRequest", null, true);
    }

    void AddMemberToClan(string SteamID)
    {
        GetRPCManager().SendRPC("AR_Banco", "ClanAddMember", new Param1<string>(SteamID), true);
    }

    void RemoveMember(string SteamID)
    {
        GetRPCManager().SendRPC("AR_Banco", "ClanRemoveMember", new Param1<string>(SteamID), true);
    }

    int GetItemAmount(ItemBase item)
	{
		Magazine mgzn = Magazine.Cast(item);
				
		int itemAmount = 0;
		if( item.IsMagazine() )
		{
			itemAmount = mgzn.GetAmmoCount();
		}
		else
		{
			itemAmount = QuantityConversions.GetItemQuantity(item);
		}
		
		return itemAmount;
	}
    
    bool WaitForServerResponse()
    {
        return m_IsWaitingForServersResponse;
    }

    ref PermissionObject GetClanPermission()
    {
        if(m_OwnClan)
        {
            for(int i = 0; i < m_OwnClan.GetClanMembers().Count(); i++)
            {
                if(m_OwnClan.GetClanMembers().Get(i).GetPlainID() == GetSteamID())
                    return m_OwnClan.GetClanMembers().Get(i).GetPermission();
            }
        }

        return null;
    }
    void SendNotification(string Message, bool IsError = false)
	{
		#ifdef NOTIFICATIONS
			if(IsError)
				NotificationSystem.SimpleNoticiation(" " + Message, "Banking", "AR_Banco/data/Logos/notificationbanking.edds", ARGB(240, 255, 0, 0), 5);
			else
			NotificationSystem.SimpleNoticiation(" " + Message, "Banking", "AR_Banco/data/Logos/notificationbanking.edds", ARGB(240, 255, 13, 55), 5);
		#else
			if(IsError)
				NotificationSystem.AddNotificationExtended(5, "Banking ERROR", Message, "AR_Banco/data/Logos/notificationbanking.edds");
			else
				NotificationSystem.AddNotificationExtended(5, "Banking", Message, "AR_Banco/data/Logos/notificationbanking.edds");
		#endif
	}

    //!returns local players Steamid....
    string GetSteamID()
    {
        return m_PlainID;
    }
};

PluginKrBankingClientManager GetBankingClientManager()
{
	return PluginKrBankingClientManager.Cast(GetPluginManager().GetPluginByType(PluginKrBankingClientManager));
};