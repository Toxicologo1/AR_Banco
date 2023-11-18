class BankingClientAdminManager extends PluginBase
{
    protected ref KR_AdminMenu                    m_AdminMenu;
    protected ref AdminPermissions                m_ClientPermissions;
    protected ref AR_BancoConfigManager         m_BankingServercfg;
    ref ClanDataBaseManager                       m_LastRequestedClanData;

    void BankingClientAdminManager()
    {
        GetRPCManager().AddRPC("AR_Banco", "AdminDataResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco", "AdminPlayerDataResponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco", "AdminClanDataReponse", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("AR_Banco", "AdminServerConfigResponse", this, SingleplayerExecutionType.Client);
    }

    void ~BankingClientAdminManager()
    {
        IsBankingAdminDataRecived = false;
        IsBankingAdmin = false;
    }

    //======= ADMIN RPC Handle =========
    void AdminDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param2<AdminPermissions, bool> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Client)
        {
            m_ClientPermissions = data.param1;
            IsBankingAdminDataRecived = true;
            IsBankingAdmin = data.param2;//We set this variable from remote.
            if(IsBankingAdmin)
                OpenBankingAdmin();
        }
    }

    void AdminPlayerDataResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param5<int, int, string, string, string> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Client)
        {
            if(m_AdminMenu)
                m_AdminMenu.UpdatePlayerCard(data.param1, data.param2, data.param3, data.param4, data.param5);
        }
    }

    void AdminServerConfigResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<AR_BancoConfigManager> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Client)
        {
            UpdateClientsServerCFGData(data.param1); // Update server cfg in clients memory.
            if(m_AdminMenu)
                m_AdminMenu.InitServerSettings(); // This will call an update method in admin menu to check and load correct values!
        }
    }

    //callis response gets called after an admin requests clandata!
    void AdminClanDataReponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<ClanDataBaseManager> data;
        if(!ctx.Read(data)) return;
        if(type == CallType.Client)
        {
            m_LastRequestedClanData = data.param1;
            if(m_AdminMenu)
                m_AdminMenu.UpdateClanDataCard();
        }
    }

    void InsertNewATM(string classname, bool canRob, vector position, vector orientation)
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminInsertATM", new Param4<string, bool, vector, vector>(classname, canRob, position, orientation), true);
    }

    void SendConfigToRemote(AR_BancoConfigManager cfg)
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminUpdateServerConfig", new Param1<AR_BancoConfigManager>(cfg), true);
        //Update Local config.
        m_BankingServercfg = cfg;
    }


    //This Sends an rpc to remote to request new server settings! Response comes in to method: AdminServerConfigResponse();
    void GetServerSettings()
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminRequestServerSettings", null, true);
    }

    void RequestClanDataById(string ID)
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminRequestClanDataWithID", new Param1<string>(ID), true);
    }

    void JoinLastLoadedClan()
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminJoinClan", new Param1<string>(m_LastRequestedClanData.GetClanID()), true);
    }

    void DeleteClan()
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminDeleteClan", new Param1<string>(m_LastRequestedClanData.GetClanID()), true);
    }

    void UpdatePlayerlist()
    {
        if(m_AdminMenu)
            m_AdminMenu.InvokePlayerList();
    }

    void MakeMoneyDrop(int amount)
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminRequestMoneyDrop", new Param1<int>(amount), true);
    }

    void TeleportToAtm(vector TeleportEndPoint)
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminTeleportToPosition", new Param1<vector>(TeleportEndPoint), true);
    }

    void OpenBankingAdmin()
    {
        if(!IsBankingAdminDataRecived)
            return;

        if(GetGame().GetUIManager().GetMenu() == NULL && m_AdminMenu == null)
        {
            m_AdminMenu = KR_AdminMenu.Cast(GetGame().GetUIManager().EnterScriptedMenu(AR_Banco_ADMIN_MENU, null));
            m_AdminMenu.SetIsAdminMenuOpened(true);
        }
        else
        {
            if(GetGame().GetUIManager().GetMenu() == NULL && !m_AdminMenu.IsAdminMenuOpen())
            {
                    GetGame().GetUIManager().ShowScriptedMenu(m_AdminMenu, NULL);
                    m_AdminMenu.SetIsAdminMenuOpened(true);
            }
        }
    }

    void ChangeRobState(bool robstate, int ATMID)
    {
        for(int i = 0; i < m_BankingServercfg.ATM.Count(); i++)
        {
            if( i== ATMID)
            {
                m_BankingServercfg.ATM.Get(i).SetCanRobbed(robstate);
                GetBankingClientManager().SendNotification("¡Estado de Rob actualizado con éxito! ¡Espere hasta el próximo reinicio del servidor!");
                break;
            }
        }
    }

    void RequestSavePlayerData(string PlainID, int newBankAmount, int newBonusAmount)
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminSafePlayerdata", new Param3<string, int, int>(PlainID, newBankAmount, newBonusAmount), true);
    }

    void DeleteATMWithID(int ID)
    {
        if(ID)
            GetRPCManager().SendRPC("AR_Banco", "AdminDeleteATM", new Param1<int>(ID), true);
    }

    void RequestResetAtmRobs()
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminResetAtmRobs", null, true);
    }

    void RequestPlayerdata(int PlayerArrayIndex)
    {
        if(!GetBankingClientManager().GetOnlinePlayers() || !GetBankingClientManager().GetOnlinePlayers().Get(PlayerArrayIndex) || !GetBankingClientManager().GetOnlinePlayers().Get(PlayerArrayIndex).plainid)
            return;
        
        string trgstid = GetBankingClientManager().GetOnlinePlayers().Get(PlayerArrayIndex).plainid;
        if(trgstid)
            GetRPCManager().SendRPC("AR_Banco", "AdminRequestPlayerdata", new Param1<string>(trgstid), true);
    }

    void RequestPlayerdataWithId(string PlainID)
    {
        if(PlainID)
            GetRPCManager().SendRPC("AR_Banco", "AdminRequestPlayerdata", new Param1<string>(PlainID), true);
    }

    void UpdateClanData(int newBankAmount, string clanname, string clantag)
    {
        GetRPCManager().SendRPC("AR_Banco", "AdminUpdateClanData", new Param4<string, string, string, int>(m_LastRequestedClanData.GetClanID(), clanname, clantag, newBankAmount), true);
    }

    AR_BancoConfigManager Getservercfg()
    {
        return m_BankingServercfg;
    }

    void UpdateClientsServerCFGData(AR_BancoConfigManager cfg)
    {
        m_BankingServercfg = cfg;
    }

    AdminPermissions GetAdminPermissions()
    {
        return m_ClientPermissions;
    }
};

BankingClientAdminManager GetBankingClientAdminManager()
{
	return BankingClientAdminManager.Cast(GetPluginManager().GetPluginByType(BankingClientAdminManager));
};