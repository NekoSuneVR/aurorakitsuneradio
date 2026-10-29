class CfgMods
{
	class NekoWave
	{
		name = "NekoWave Radio";
		version = "0.2.2";
		inputs = "NekoWave/inputs.xml";
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"NekoWave/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"NekoWave/5_Mission"};
			};
		};
	};
};
class CfgPatches
{
	class NekoWave
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};
