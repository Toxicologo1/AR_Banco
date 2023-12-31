class BankingAdminManager extends PluginBase
{
    void BankingAdminManager()
    {
        if(GetGame() && GetGame().IsServer())
        {
            Print("[Advanced Banking] -> Internal Admin Module starting....");
            Print(" Found: " + GetKR_AdminConfig().m_Admins.Count().ToString());
            Init();
        }
    }

    void Init()
    {
        GetRPCManager().AddRPC("AR_Banco", "AdminDataRequest", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminRequestPlayerdata", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminRequestServerSettings", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminSafePlayerdata", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminTeleportToPosition", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminDeleteATM", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminInsertATM", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminUpdateServerConfig", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminReloadConfig", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminRequestClanDataWithID", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminJoinClan", 	this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminDeleteClan", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminRequestMoneyDrop", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminResetAtmRobs", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("AR_Banco", "AdminUpdateClanData", this, SingleplayerExecutionType.Server);
    }

    void AdminDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            foreach(AdminUsers admin: GetKR_AdminConfig().m_Admins)
            {
                if(sender.GetPlainId() == admin.m_PlainID)
                {
                    if(!admin.m_permissions.m_CanIgnoreNamePermission)
                    {
                        if(admin.m_Name != sender.GetName())
                        {
                            GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                            return;
                        }
                    }
                    GetRPCManager().SendRPC("AR_Banco", "AdminDataResponse", new Param2<AdminPermissions, bool >( admin.m_permissions, true ), true, sender);
                    return;
                }
            }
            GetRPCManager().SendRPC("AR_Banco", "AdminDataResponse", new Param2<AdminPermissions, bool >( null, false ), true, sender);
        }
    }

    void AdminRequestPlayerdata(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;

            string targetsPlainId = data.param1;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUsePlayersBankData)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                KR_JsonDatabaseHandler targetdata = KR_JsonDatabaseHandler.LoadPlayerData(targetsPlainId, "", true);//true because search only otherwise it will create a new json lol!
                if(targetdata)
                {
                    GetRPCManager().SendRPC("AR_Banco", "AdminPlayerDataResponse", new Param5< int, int, string, string, string >( targetdata.GetBankCredit(), targetdata.GetBonusAmount(), targetdata.GetName(), targetdata.GetSteamID(), targetdata.GetClanID()), true, sender);
                }
                else
                {
                    GetBankingServerManager().SendNotification("Error while loading playerdata from: " + targetsPlainId, sender, true);
                }
            }
            else
            {
                GetBankingServerManager().SendNotification("You dont have enought permission to do that!", sender, true);
            }
        }
    }

    void AdminSafePlayerdata(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param3<string, int, int> data;
            if(!ctx.Read(data)) return;
            string targetsPlainId = data.param1;
            AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUsePlayersBankData)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                KR_JsonDatabaseHandler targetdata = KR_JsonDatabaseHandler.LoadPlayerData(targetsPlainId, "");
                if(targetdata)
                {
                    targetdata.UpdateCurrency(data.param2, false);
                    targetdata.UpdateBonus(data.param3, true);
                    GetBankingServerManager().SendNotification("Sucesfully Update Playerdata from: " + targetdata.GetName(), sender);

                    if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdatePlayerData)
                        GetWebhookManager().AdminLog(sender, " Admin Updated player data from player: " + targetdata.GetName() + " with id: " + targetsPlainId);
                }
                else
                {
                    GetBankingServerManager().SendNotification("Error while loading playerdata from: " + targetsPlainId, sender, true);
                }
            }
            else
            {
                GetBankingServerManager().SendNotification("You dont have enought permission to do that!", sender, true);
            }
        }
    }

    void AdminRequestServerSettings(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                GetRPCManager().SendRPC("AR_Banco", "AdminServerConfigResponse", new Param1<AR_BancoConfigManager >( GetAR_BancoServerConfig() ), true, sender);
            }
            else
            {
                GetBankingServerManager().SendNotification("You dont have enought permission to do that! Permission needed: m_CanUseServerConfig", sender, true);
            }
        }
    }

    void AdminReloadConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {

            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                GetBankingServerManager().InternalConfigReload();
                array<Man> allPlayers = new array<Man>;
                GetGame().GetPlayers(allPlayers);
                PlayerBase player;
                foreach(Man curman : allPlayers)
                {
                    if(Class.CastTo(player, curman))
                    {
                        GetBankingServerManager().SendConfigToClient(player.GetIdentity());
                    }
                }
                GetBankingServerManager().SendNotification("Config reloadet & sended to every connected client!", sender);
            }
        }
    }

    //function ready!
    void AdminDeleteATM(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<int> data;
            if(!ctx.Read(data)) return;
            AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                ATMPosition CurrentATM;
                CurrentATM = GetAR_BancoServerConfig().ATM.Get(data.param1);
                if(CurrentATM)
                {
                    array<Object> excluded_objects = new array<Object>;
                    array<Object> nearby_objects = new array<Object>;
                    AR_BancoATM atm;
                    GetGame().IsBoxColliding(CurrentATM.GetATMPosition() , CurrentATM.GetATMDirectory(), Vector(3, 5, 9), excluded_objects, nearby_objects);
                    if(nearby_objects.Count() > 0)
                    {
                        foreach(Object nerbyobj : nearby_objects)
                        {

                            if(Class.CastTo(atm, nerbyobj))
                            {
                                //Found an ATM on this position!
                                if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
                                    GetWebhookManager().AdminLog(sender, " Deleted ATM: " + atm.GetType() + " on position: " + atm.GetPosition().ToString());
                                
                                GetGame().ObjectDelete(atm);
                                GetAR_BancoServerConfig().RemoveATMWithIndex(data.param1);
                                GetBankingServerManager().SendNotification("ATM Deleted!", sender, true);
                                return;
                            }
                        }
                    }
                    else
                    {
                        GetBankingServerManager().SendNotification("ATM On Map Already deleted failed to delete ATM!", sender, true);
                    }
                }
                else
                {
                    GetBankingServerManager().SendNotification("Error cant find this Atm!", sender, true);
                }
            }
        }
    }

    //function ready!
    void AdminInsertATM(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param4<string, bool, vector, vector> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                GetAR_BancoServerConfig().AddATM(data.param1, data.param2, data.param3, data.param4);

                EntityAI SpawnedATM = EntityAI.Cast(GetGame().CreateObject(data.param1, data.param3));
                SpawnedATM.SetPosition(data.param3);
                SpawnedATM.SetOrientation(data.param4);
                AR_BancoATM AdvATMS;
                if(Class.CastTo(AdvATMS, SpawnedATM))
                {
                    AdvATMS.m_Banking_CanBeRobbed = data.param2;
                }

                if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
                    GetWebhookManager().AdminLog(sender, " Inserted an new ATM at position: " + SpawnedATM.GetPosition().ToString());
            }
        }
    }

    //Function Ready
    void AdminUpdateServerConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<ref AR_BancoConfigManager> data;
            if(!ctx.Read(data)) return;
            AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
                   GetWebhookManager().AdminLog(sender, " Updated Server config.");
                GetAR_BancoServerConfig().SetConfig(data.param1);
            }
        }
    }

    //function ready
    void AdminRequestMoneyDrop(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<int> data;
            if(!ctx.Read(data)) return;
            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUseMiscServer)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                
                array<Man> allOnlinePlayers = new array<Man>;
                GetGame().GetPlayers(allOnlinePlayers);

                PlayerBase ntarplayer;
                foreach(Man man : allOnlinePlayers)
                {
                    if(Class.CastTo(ntarplayer, man))
                    {
                        KR_JsonDatabaseHandler playerdata = KR_JsonDatabaseHandler.LoadPlayerData(ntarplayer.GetIdentity().GetPlainId(), ntarplayer.GetIdentity().GetName());
                        if(playerdata)
                        {
                            playerdata.DepositMoney(data.param1);
                            GetRPCManager().SendRPC("AR_Banco", "MoneyDropRecvied", null, true, ntarplayer.GetIdentity());
                            GetBankingServerManager().SendNotification("You reviced an Money Drop (" + data.param1 + ") from Admin: " + sender.GetName(), ntarplayer.GetIdentity(), true);
                        }
                    }
                }

                if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUseMisc)
                   GetWebhookManager().AdminLog(sender, " Maked an money drop with amount: " + data.param1);
            }
        }
    }

    //Function ready
    void AdminTeleportToPosition(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<vector> data;
            if(!ctx.Read(data)) return;
            AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanIgnoreNamePermission)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                PlayerBase player = GetBankingServerManager().RemoteFindPlayer(sender.GetPlainId());
                if(player)
                {
                    player.SetPosition(data.param1);

                    if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdateServerConfig)
                        GetWebhookManager().AdminLog(sender, " Admin used Teleport to ATM command to Position: " + player.GetPosition().ToString());
                }
            }
        }
    }

    void AdminRequestClanDataWithID(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;
            AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUsePlayersClans)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }
                KR_JsonDatabaseHandler targetPlayer = KR_JsonDatabaseHandler.LoadPlayerData(data.param1); 
                if(!targetPlayer) return;
                ClanDataBaseManager clanData = ClanDataBaseManager.LoadClanData(targetPlayer.GetClanID());
                if(clanData)
                {
                    GetRPCManager().SendRPC("AR_Banco", "AdminClanDataReponse", new Param1<ClanDataBaseManager >( clanData ), true, sender);
                }
            }
        }
    }

    //Function ready!
    void AdminJoinClan(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;

            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUsePlayersClans)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(data.param1);
				if(clandata)
				{
                    KR_JsonDatabaseHandler adminsData = KR_JsonDatabaseHandler.LoadPlayerData(sender.GetPlainId(), sender.GetName());
                    if(adminsData)
                    {
                        adminsData.SetClan(data.param1);
                        PermissionObject perms = new PermissionObject();
                        perms.GiveClanOwner();
                        GetBankingServerManager().AddClanMember(clandata, perms, sender.GetPlainId(), sender.GetName());
                        GetBankingServerManager().SendNotification("Sucesfully joined clan!", sender);

                        if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdateClanData)
                            GetWebhookManager().AdminLog(sender, " Admin joined clan: " + clandata.GetClanID());
                    }
                }
            }
        }
    }

    void AdminDeleteClan(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param1<string> data;
            if(!ctx.Read(data)) return;

            ref AdminUsers admin = GetAdminByPlainId(sender.GetPlainId());
            if(admin && admin.m_permissions.m_CanUseServerConfig)
            {
                if(!admin.m_permissions.m_CanUsePlayersClans)
                {
                    if(admin.m_Name != sender.GetName())
                    {
                        GetBankingServerManager().SendNotification("Action can only executed with name: " + admin.m_Name, sender, true);
                        return;
                    }
                }

                ClanDataBaseManager clandata = ClanDataBaseManager.LoadClanData(data.param1);
				if(clandata)
				{
                    for(int i = 0; i < clandata.GetClanMembers().Count(); i++)
                    {
                        string plainId = clandata.GetClanMembers().Get(i).GetPlainID();
                        if(plainId)
                        {
                            KR_JsonDatabaseHandler membersData = KR_JsonDatabaseHandler.LoadPlayerData(plainId);
                            if(membersData)
                            {
                                membersData.SetClan("NONE");
                            }
                        }
                    }
                    if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdateClanData)
                        GetWebhookManager().AdminLog(sender, " Admin deleted clan: " + clandata.GetClanID());
                    
                    clandata.DeleteClan();
                    GetBankingServerManager().SendNotification("Sucesfully deleted clan!", sender);
                }
            }
        }
    }

    void AdminResetAtmRobs(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            int ResetCounter = 0;
            for(int i = 0; i < GetAR_BancoServerConfig().ATM.Count(); i++)
            {
                if(!GetAR_BancoServerConfig().ATM.Get(i).GetCanRobbed())
                    continue;
                
                vector tempATMPosition = GetAR_BancoServerConfig().ATM.Get(i).GetATMPosition();
                vector tempATMOrientation = GetAR_BancoServerConfig().ATM.Get(i).GetATMDirectory();
                array<Object> excluded_objects = new array<Object>;
			    array<Object> nearby_objects = new array<Object>;

                if(tempATMPosition && tempATMOrientation)
                {
                    if(GetGame().IsBoxColliding(tempATMPosition, tempATMOrientation, Vector(4,4,4), excluded_objects, nearby_objects))
			        {
                        for(int k = 0; k < nearby_objects.Count(); k++)
                        {
                            AR_BancoATM AtmObj;
                            if(Class.CastTo(AtmObj, nearby_objects.Get(k)))
                            {
                                AtmObj.SetATMIsRobbed(false);
                                AtmObj.m_Banking_CanBeRobbed = true;
                                AtmObj.SetSynchDirty();
                                ResetCounter++;
                                break;
                            }
                        }
			        }
                }
                else 
                {
                    Error("ATM Position was empty! Cannot Handle Reset Rob!");
                }
            }
            GetBankingServerManager().SendNotification(ResetCounter.ToString() + " ATMs have been resetted!", sender);

            if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUseMisc)
                GetWebhookManager().AdminLog(sender, " Admin reseted all robbed ATMS! (" + ResetCounter.ToString() + ")");
        }
    }

    void AdminUpdateClanData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if(type == CallType.Server)
        {
            Param4<string, string, string, int> data;
            if(!ctx.Read(data)) return;

            ClanDataBaseManager editedClan = ClanDataBaseManager.LoadClanData(data.param1);
            if(editedClan)
            {
                string newClanName  = data.param2;
                string newClanTag   = data.param3;
                int    Amount       = data.param4;
                editedClan.SetName(newClanName);
                editedClan.SetMoney(Amount);
                editedClan.SetPrefix(newClanTag);//here is a json safe method in so thats why we call this at last! 
                GetBankingServerManager().SendNotification("Sucesfully updated clan data!", sender);

                if(GetAR_BancoServerConfig().m_DiscordWebhook.m_AdminLogUpdateClanData)
                    GetWebhookManager().AdminLog(sender, " Admin Updated clandata from clan: " + editedClan.GetClanID());
            }
            else
            {
                Error("can`t load clandata (AdminMode)");
            }
        }
    }

    //!This returns the Admin Object of a user! if a user is no admin it returns null.
    ref AdminUsers GetAdminByPlainId(string PlainId)
    {
        foreach(AdminUsers admin: GetKR_AdminConfig().m_Admins)
        {
            if(PlainId == PlainId)
            {
                return admin;
            }
        }
        return null;
    }
};