class CfgPatches 
{
	class AR_Banco
	{
		units[] = 
		{
			"AR_Banco"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = 
		{
			"DZ_Data",
			"JM_CF_Scripts"
		};
	};
};

class CfgMods 
{
	class AR_Banco 
	{
		dir = "AR_Banco";
		picture = "";
		action = "https";
		hideName = 1;
		hidePicture = 1;
		name = "Banco Avanzado";
		credits = "Toxi";
		author = "Toxi";
		authorID = "76561198327232571";
		version = "2.13";
		extra = 0;
		type = "mod";
		inputs = "AR_Banco\data\inputs.xml";
		dependencies[] = {"Mission", "World", "Game"};

	    class defs
	    {
			class gameScriptModule
            {
                value = "";
                files[] = {"AR_Banco/scripts/common", "AR_Banco/scripts/3_Game"};
            };
			class worldScriptModule
            {
                value = "";
                files[] = {"AR_Banco/scripts/common", "AR_Banco/scripts/4_World"};
            };
			class missionScriptModule
            {
                value = "";
                files[] = {"AR_Banco/scripts/common", "AR_Banco/scripts/5_Mission"};
            };
        };
	};
};

class CfgVehicles
{
	class HouseNoDestruct;
	class Inventory_Base;
	class KeyCard_Base: Inventory_Base{};
	class AR_BancoATM: HouseNoDestruct
	{
		scope=1;
		model="AR_Banco\data\ATM\Banco_Avanzado";
		hiddenSelections[]=
		{
			"zybtek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_co"
		};
	};

	class Banco Oxidado1: AR_BancoATM
	{
		scope=1;
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_co.paa"
		};
	};
	class Banco Oxidado: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_co1.paa"
		};
	};
	class Banco Rojo: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_Rojo.paa"
		};
	};
	class Banco Azul: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_Azul.paa"
		};
	};
	class Banco Verde: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_Verde.paa"
		};
	};
	class Banco Amarillo: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"\AR_Banco\data\ATM\data\Banco_Amarillo.paa"
		};
	};
	class Banco Naranja: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_Naranja.paa"
		};
	};
	class Banco Completo: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_cow.paa"
		};
	};
	class Banco Negro: AR_BancoATM
	{
		scope=1;
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATM\data\Banco_Negro.paa"
		};
	};
	class AR_BancoCard: KeyCard_Base
	{
		scope=2;
		displayName="Banking Card";
		descriptionShort="For Logging in into ATM";
		model="AR_Banco\data\ATMCard\AR_BancoTarjeta.p3d";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[] = 
		{
			"AR_Banco\data\ATMCard\data\Tarjeta_co.paa"
		};
	};
};

class CfgSoundShaders
{
	class MoneyDrop_SoundShaders
	{
		samples[]=
		{
			
			{
				"AR_Banco\data\Sounds\MoneyDrop.ogg",
				1
			}
		};
		range=5;
		volume=1.5;
	};
};

class CfgSoundSets
{
	class MoneyDrop_SoundSet
	{
		soundShaders[]=
		{
			"MoneyDrop_SoundShaders"
		};
	};
};