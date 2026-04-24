class TrackingModPlayerEntryController: ViewController
{
	string PlayerName = "";
	string PlayerPosition = "0";
	string PVEPoints = "0";
	string ShotsFired = "0";
	string LongestRange = "0";
	string TotalDeaths = "0";
	string Suicides = "0";
	string DistanceOnFoot = "0 m";
	string DistanceInVehicle = "0 m";
	ref ObservableCollection<ref TrackingModKillCategory> KillCategories = new ObservableCollection<ref TrackingModKillCategory>(this);
}
