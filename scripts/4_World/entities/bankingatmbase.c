class AR_BancoATM extends BuildingSuper
{
    bool m_Banking_CanBeRobbed = false;
    protected bool m_IsRobbed;
    void AR_BancoATM()
    {
        RegisterNetSyncVariableBool("m_Banking_CanBeRobbed");
        RegisterNetSyncVariableBool("m_IsRobbed");
    }

    bool IsATMRobbed()
    {
        return m_IsRobbed;
    }

    void SetATMIsRobbed(bool state)
    {
        m_IsRobbed = state;
    }

    override void SetActions()
	{
        super.SetActions();
        AddAction(ActionOpenBankingMenu);
    }
}

//class AR_Banco: AR_BancoATM{};
//class AR_BancoRusty: AR_BancoATM{};