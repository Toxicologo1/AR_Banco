modded class MissionBase 
{
    override UIScriptedMenu CreateScriptedMenu (int id) 
    {
        UIScriptedMenu menu = NULL;
        menu = super.CreateScriptedMenu(id);
        if (!menu) 
        {
            switch (id) 
            {
                case AR_Banco_MENU:
                    menu = Nuevo AR_BancoMenu;
                    break;
                case AR_Banco_ADMIN_MENU:
                    menu = Nuevo BancoAvanzadoMenu;
                    break;
            }
            if (menu)
            {
                menu.SetID(id);
            }
        }
        return menu;
    }
};