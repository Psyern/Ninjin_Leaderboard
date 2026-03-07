class CfgPatches
{
	class Ninjins_LeaderBoard
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data"};
	};
};
class CfgMods
{
	class Ninjins_LeaderBoard
	{
		dir="Ninjins_LeaderBoard";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="Ninjins LeaderBoard";
		credits="Ninjin";
		author="Ninjin";
		authorID="0";
		version="1.0.0";
		extra=0;
		type="mod";
		storageVersion=1;
		inputs="Ninjins_LeaderBoard/data/modded_inputs.xml";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"Ninjins_LeaderBoard/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Ninjins_LeaderBoard/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"Ninjins_LeaderBoard/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgVehicles
{
};
