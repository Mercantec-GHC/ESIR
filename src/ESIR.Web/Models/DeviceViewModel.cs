namespace ESIR.Web.Models;

public class DeviceViewModel
{
	public Guid Id { get; set; }
	public string Name { get; set; } = string.Empty;
	public bool IsConnected { get; set; }
	public DateTimeOffset? LastSeen { get; set; }
}
