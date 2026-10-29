class CfgMods
{
	class AuroraKitsuneRadio
	{
		name = "AuroraKitsuneRadio Radio";
		version = "0.2.2";
		inputs = "AuroraKitsuneRadio/inputs.xml";
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"AuroraKitsuneRadio/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"AuroraKitsuneRadio/5_Mission"};
			};
		};
	};
};
class CfgPatches
{
	class AuroraKitsuneRadio
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};
