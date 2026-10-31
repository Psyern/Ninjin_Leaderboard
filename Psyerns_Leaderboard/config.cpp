class CfgPatches
{
	class Psyerns_Leaderboard
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data"};
	};
};
class CfgMods
{
	class Psyerns_Leaderboard
	{
		dir="Psyerns_Leaderboard";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="Psyerns Leaderboard";
		credits="Psyern, original mod by Ninjin";
		author="Psyern";
		authorID="0";
		version="1.0.0";
		extra=0;
		type="mod";
		storageVersion=1;
		inputs="Psyerns_Leaderboard/data/modded_inputs.xml";
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
					"Psyerns_Leaderboard/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Psyerns_Leaderboard/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"Psyerns_Leaderboard/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgVehicles
{
};
