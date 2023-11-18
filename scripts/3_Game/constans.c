static const int BancoAvanzadoMenu = 86721531;
static const int BancoAdminMenu = 32236412;
const protected static string m_BankingProfileDIR = "$profile:";
const protected static string m_BankingConfigDIR = "AR_BancoAvanzado";
const protected static string m_BankingConfigPath = m_BankingProfileDIR + m_BankingConfigDIR + "/" + "server-config.json";
bool IsBankingAdminDataRecived = false; //This sure client has this data already from remote!
bool IsBankingAdmin = false;//This goes to true if a player has admin on the bank!

enum BankType
{
    OWNBANK,
    CLANBANK
};